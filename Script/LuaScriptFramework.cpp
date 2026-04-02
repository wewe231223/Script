#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "LuaScriptFramework.h"

namespace Script {
    LuaBehaviorFramework::BehaviorContext::BehaviorContext()
        : mWorld{},
          mOwnerEntity{},
          mOwnerFramework{} {
    }

    LuaBehaviorFramework::BehaviorContext::~BehaviorContext() {
    }

    LuaBehaviorFramework::BehaviorContext::BehaviorContext(BehaviorContext&& Other) noexcept
        : mWorld{ Other.mWorld },
          mOwnerEntity{ Other.mOwnerEntity },
          mOwnerFramework{ Other.mOwnerFramework } {
    }

    LuaBehaviorFramework::BehaviorContext& LuaBehaviorFramework::BehaviorContext::operator=(BehaviorContext&& Other) noexcept {
        if (this == &Other) {
            return *this;
        }

        mWorld = Other.mWorld;
        mOwnerEntity = Other.mOwnerEntity;
        mOwnerFramework = Other.mOwnerFramework;
        return *this;
    }

    void LuaBehaviorFramework::BehaviorContext::Bind(Arche::World* TargetWorld, Arche::EntityID OwnerEntity, LuaBehaviorFramework* OwnerFramework) {
        mWorld = TargetWorld;
        mOwnerEntity = OwnerEntity;
        mOwnerFramework = OwnerFramework;
    }

    Arche::EntityID LuaBehaviorFramework::BehaviorContext::GetEntityId() const {
        return mOwnerEntity;
    }

    bool LuaBehaviorFramework::BehaviorContext::HasComponent(const std::string& ComponentName) const {
        if (mOwnerFramework == nullptr || mWorld == nullptr) {
            return false;
        }

        const std::unordered_map<std::string, ComponentGetter>& Getters{ mOwnerFramework->mComponentGetters };
        auto GetterIt{ Getters.find(ComponentName) };

        if (GetterIt == Getters.end()) {
            return false;
        }

        sol::object Value{ GetterIt->second(mOwnerFramework->mLuaState.lua_state(), *mWorld, mOwnerEntity) };
        return Value.get_type() != sol::type::nil;
    }

    sol::object LuaBehaviorFramework::BehaviorContext::GetComponent(const std::string& ComponentName) {
        if (mOwnerFramework == nullptr || mWorld == nullptr) {
            return sol::object{};
        }

        std::unordered_map<std::string, ComponentGetter>& Getters{ mOwnerFramework->mComponentGetters };
        auto GetterIt{ Getters.find(ComponentName) };

        if (GetterIt == Getters.end()) {
            return sol::make_object(mOwnerFramework->mLuaState.lua_state(), sol::lua_nil);
        }

        return GetterIt->second(mOwnerFramework->mLuaState.lua_state(), *mWorld, mOwnerEntity);
    }

    LuaBehaviorFramework::LuaBehaviorFramework()
        : mWorld{},
          mLuaState{},
          mComponentGetters{},
          mBehaviorFilePaths{},
          mRuntimeInstances{},
          mLastIssuedBehaviorInstanceId{},
          mFixedDeltaSeconds{ 1.0f },
          mFixedUpdateThread{},
          mIsFixedUpdateThreadRunning{ false },
          mFixedUpdateCondition{},
          mRuntimeMutex{} {
    }

    LuaBehaviorFramework::~LuaBehaviorFramework() {
        StopFixedUpdateThread();
    }

    void LuaBehaviorFramework::Initialize(Arche::World* TargetWorld) {
        std::lock_guard<std::mutex> RuntimeGuard{ mRuntimeMutex };
        mWorld = TargetWorld;
        mLuaState.new_usertype<BehaviorContext>("BehaviorContext", "GetEntityId", &BehaviorContext::GetEntityId, "GetComponent", static_cast<sol::object(BehaviorContext::*)(const std::string&)>(&BehaviorContext::GetComponent), "HasComponent", &BehaviorContext::HasComponent);
        StartFixedUpdateThread();
    }

    void LuaBehaviorFramework::OpenDefaultLibraries() {
        std::lock_guard<std::mutex> RuntimeGuard{ mRuntimeMutex };
        mLuaState.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::package);
    }

    void LuaBehaviorFramework::SetFixedUpdateInterval(float FixedDeltaSeconds) {
        std::lock_guard<std::mutex> RuntimeGuard{ mRuntimeMutex };

        if (FixedDeltaSeconds <= 0.0f) {
            return;
        }

        mFixedDeltaSeconds = FixedDeltaSeconds;
        mFixedUpdateCondition.notify_all();
    }

    bool LuaBehaviorFramework::AttachBehavior(Arche::EntityID TargetEntity, const std::string& BehaviorSource, std::uint32_t BehaviorAssetId) {
        std::lock_guard<std::mutex> RuntimeGuard{ mRuntimeMutex };

        if (mWorld == nullptr) {
            return false;
        }

        if (mWorld->GetComponent<BehaviorInstanceComponent>(TargetEntity) != nullptr) {
            return false;
        }

        std::uint32_t BehaviorInstanceId{ GenerateBehaviorInstanceId() };
        BehaviorInstanceComponent NewComponent{};
        NewComponent.mOwnerEntity = TargetEntity;
        NewComponent.mBehaviorInstanceId = BehaviorInstanceId;
        NewComponent.mBehaviorAssetId = BehaviorAssetId;
        mWorld->AddComponent<BehaviorInstanceComponent>(TargetEntity, NewComponent);

        RuntimeBehaviorInstance NewRuntimeInstance{};
        NewRuntimeInstance.mContext.Bind(mWorld, TargetEntity, this);
        NewRuntimeInstance.mEnvironment = sol::environment(mLuaState, sol::create, mLuaState.globals());
        BindContextToEnvironment(NewRuntimeInstance);

        sol::protected_function_result LoadResult{ mLuaState.safe_script(BehaviorSource, NewRuntimeInstance.mEnvironment, sol::script_pass_on_error) };

        if (!LoadResult.valid()) {
            return false;
        }

        ResolveBehaviorEntries(NewRuntimeInstance);
        RunEntryWithArguments(NewRuntimeInstance.mEntries.mAwake, &NewRuntimeInstance.mContext);
        RunEntryWithArguments(NewRuntimeInstance.mEntries.mOnEnable, &NewRuntimeInstance.mContext);
        RunEntryWithArguments(NewRuntimeInstance.mEntries.mStart, &NewRuntimeInstance.mContext);

        mRuntimeInstances.emplace(BehaviorInstanceId, std::move(NewRuntimeInstance));
        return true;
    }

    bool LuaBehaviorFramework::AttachBehaviorFromFile(Arche::EntityID TargetEntity, const std::string& BehaviorFilePath, std::uint32_t BehaviorAssetId) {
        std::string BehaviorSource{};

        if (!ReadBehaviorSourceFromFilePath(BehaviorFilePath, BehaviorSource)) {
            return false;
        }

        if (!AttachBehavior(TargetEntity, BehaviorSource, BehaviorAssetId)) {
            return false;
        }

        std::lock_guard<std::mutex> RuntimeGuard{ mRuntimeMutex };
        RuntimeBehaviorInstance* RuntimeInstance{ FindRuntimeInstance(TargetEntity) };

        if (RuntimeInstance == nullptr) {
            return false;
        }

        std::filesystem::path FullPath{ BehaviorFilePath };
        std::string BehaviorFileName{ FullPath.filename().string() };
        RuntimeInstance->mBehaviorFileName = BehaviorFileName;
        mBehaviorFilePaths[BehaviorFileName] = BehaviorFilePath;
        return true;
    }

    bool LuaBehaviorFramework::HotReloadBehavior(const std::string& BehaviorFileName) {
        std::lock_guard<std::mutex> RuntimeGuard{ mRuntimeMutex };
        auto PathIt{ mBehaviorFilePaths.find(BehaviorFileName) };

        if (PathIt == mBehaviorFilePaths.end()) {
            std::string FallbackPath{ "Script/Lua/" + BehaviorFileName };
            mBehaviorFilePaths[BehaviorFileName] = FallbackPath;
            PathIt = mBehaviorFilePaths.find(BehaviorFileName);
        }

        std::string BehaviorSource{};

        if (!ReadBehaviorSourceFromFilePath(PathIt->second, BehaviorSource)) {
            return false;
        }

        bool IsEveryReloadSucceeded{ true };
        std::size_t ReloadedCount{};

        for (auto& RuntimePair : mRuntimeInstances) {
            RuntimeBehaviorInstance& RuntimeInstance{ RuntimePair.second };

            if (RuntimeInstance.mBehaviorFileName != BehaviorFileName) {
                continue;
            }

            RunEntryWithArguments(RuntimeInstance.mEntries.mOnDisable, &RuntimeInstance.mContext);
            RuntimeInstance.mEnvironment = sol::environment(mLuaState, sol::create, mLuaState.globals());
            BindContextToEnvironment(RuntimeInstance);

            sol::protected_function_result LoadResult{ mLuaState.safe_script(BehaviorSource, RuntimeInstance.mEnvironment, sol::script_pass_on_error) };

            if (!LoadResult.valid()) {
                IsEveryReloadSucceeded = false;
                continue;
            }

            ResolveBehaviorEntries(RuntimeInstance);
            RunEntryWithArguments(RuntimeInstance.mEntries.mAwake, &RuntimeInstance.mContext);
            RunEntryWithArguments(RuntimeInstance.mEntries.mOnEnable, &RuntimeInstance.mContext);
            RunEntryWithArguments(RuntimeInstance.mEntries.mStart, &RuntimeInstance.mContext);
            RuntimeInstance.mIsEnabled = true;
            ReloadedCount = ReloadedCount + 1;
        }

        return ReloadedCount > 0 && IsEveryReloadSucceeded;
    }


    bool LuaBehaviorFramework::DisableBehavior(Arche::EntityID TargetEntity) {
        std::lock_guard<std::mutex> RuntimeGuard{ mRuntimeMutex };
        RuntimeBehaviorInstance* RuntimeInstance{ FindRuntimeInstance(TargetEntity) };

        if (RuntimeInstance == nullptr) {
            return false;
        }

        if (!RuntimeInstance->mIsEnabled) {
            return false;
        }

        RunEntryWithArguments(RuntimeInstance->mEntries.mOnDisable, &RuntimeInstance->mContext);
        RuntimeInstance->mIsEnabled = false;
        return true;
    }

    bool LuaBehaviorFramework::DestroyBehavior(Arche::EntityID TargetEntity) {
        std::lock_guard<std::mutex> RuntimeGuard{ mRuntimeMutex };
        RuntimeBehaviorInstance* RuntimeInstance{ FindRuntimeInstance(TargetEntity) };

        if (RuntimeInstance == nullptr) {
            return false;
        }

        if (RuntimeInstance->mIsEnabled) {
            RunEntryWithArguments(RuntimeInstance->mEntries.mOnDisable, &RuntimeInstance->mContext);
            RuntimeInstance->mIsEnabled = false;
        }

        RunEntryWithArguments(RuntimeInstance->mEntries.mOnDestroy, &RuntimeInstance->mContext);

        BehaviorInstanceComponent* InstanceComponent{ mWorld->GetComponent<BehaviorInstanceComponent>(TargetEntity) };

        if (InstanceComponent == nullptr) {
            return false;
        }

        auto RuntimeIt{ mRuntimeInstances.find(InstanceComponent->mBehaviorInstanceId) };

        if (RuntimeIt == mRuntimeInstances.end()) {
            return false;
        }

        mRuntimeInstances.erase(RuntimeIt);
        return true;
    }

    void LuaBehaviorFramework::DetachBehavior(Arche::EntityID TargetEntity) {
        static_cast<void>(DestroyBehavior(TargetEntity));
    }

    void LuaBehaviorFramework::Update(float DeltaSeconds) {
        std::lock_guard<std::mutex> RuntimeGuard{ mRuntimeMutex };

        for (auto& RuntimePair : mRuntimeInstances) {
            RuntimeBehaviorInstance& RuntimeInstance{ RuntimePair.second };

            if (!RuntimeInstance.mIsEnabled) {
                continue;
            }

            RunEntryWithArguments(RuntimeInstance.mEntries.mUpdate, &RuntimeInstance.mContext, DeltaSeconds);
        }
    }

    void LuaBehaviorFramework::FixedUpdate(float FixedDeltaSeconds) {
        std::lock_guard<std::mutex> RuntimeGuard{ mRuntimeMutex };

        for (auto& RuntimePair : mRuntimeInstances) {
            RuntimeBehaviorInstance& RuntimeInstance{ RuntimePair.second };

            if (!RuntimeInstance.mIsEnabled) {
                continue;
            }

            RuntimeInstance.mFixedTick = RuntimeInstance.mFixedTick + 1;
            RunEntryWithArguments(RuntimeInstance.mEntries.mFixedUpdate, &RuntimeInstance.mContext, FixedDeltaSeconds, RuntimeInstance.mFixedTick);
        }
    }

    void LuaBehaviorFramework::LateUpdate(float DeltaSeconds) {
        std::lock_guard<std::mutex> RuntimeGuard{ mRuntimeMutex };

        for (auto& RuntimePair : mRuntimeInstances) {
            RuntimeBehaviorInstance& RuntimeInstance{ RuntimePair.second };

            if (!RuntimeInstance.mIsEnabled) {
                continue;
            }

            RunEntryWithArguments(RuntimeInstance.mEntries.mLateUpdate, &RuntimeInstance.mContext, DeltaSeconds);
        }
    }

    sol::state& LuaBehaviorFramework::GetState() {
        return mLuaState;
    }

    const sol::state& LuaBehaviorFramework::GetState() const {
        return mLuaState;
    }

    LuaBehaviorFramework::RuntimeBehaviorInstance* LuaBehaviorFramework::FindRuntimeInstance(Arche::EntityID TargetEntity) {
        if (mWorld == nullptr) {
            return nullptr;
        }

        BehaviorInstanceComponent* InstanceComponent{ mWorld->GetComponent<BehaviorInstanceComponent>(TargetEntity) };

        if (InstanceComponent == nullptr) {
            return nullptr;
        }

        auto RuntimeIt{ mRuntimeInstances.find(InstanceComponent->mBehaviorInstanceId) };

        if (RuntimeIt == mRuntimeInstances.end()) {
            return nullptr;
        }

        return &RuntimeIt->second;
    }

    const LuaBehaviorFramework::RuntimeBehaviorInstance* LuaBehaviorFramework::FindRuntimeInstance(Arche::EntityID TargetEntity) const {
        return const_cast<LuaBehaviorFramework*>(this)->FindRuntimeInstance(TargetEntity);
    }

    std::uint32_t LuaBehaviorFramework::GenerateBehaviorInstanceId() {
        ++mLastIssuedBehaviorInstanceId;
        return mLastIssuedBehaviorInstanceId;
    }

    bool LuaBehaviorFramework::ReadBehaviorSourceFromFilePath(const std::string& BehaviorFilePath, std::string& OutBehaviorSource) const {
        std::ifstream InputStream{ BehaviorFilePath, std::ios::in | std::ios::binary };

        if (!InputStream.is_open()) {
            return false;
        }

        std::stringstream Buffer{};
        Buffer << InputStream.rdbuf();
        OutBehaviorSource = Buffer.str();
        return true;
    }

    void LuaBehaviorFramework::StartFixedUpdateThread() {
        if (mIsFixedUpdateThreadRunning.load()) {
            return;
        }

        mIsFixedUpdateThreadRunning.store(true);
        mFixedUpdateThread = std::thread{ &LuaBehaviorFramework::FixedUpdateThreadMain, this };
    }

    void LuaBehaviorFramework::StopFixedUpdateThread() {
        if (!mIsFixedUpdateThreadRunning.load()) {
            return;
        }

        mIsFixedUpdateThreadRunning.store(false);
        mFixedUpdateCondition.notify_all();

        if (mFixedUpdateThread.joinable()) {
            mFixedUpdateThread.join();
        }
    }

    void LuaBehaviorFramework::FixedUpdateThreadMain() {
        while (mIsFixedUpdateThreadRunning.load()) {
            float CurrentFixedDeltaSeconds{};

            {
                std::unique_lock<std::mutex> RuntimeLock{ mRuntimeMutex };
                CurrentFixedDeltaSeconds = mFixedDeltaSeconds;
            }

            FixedUpdate(CurrentFixedDeltaSeconds);

            std::unique_lock<std::mutex> WaitLock{ mRuntimeMutex };
            mFixedUpdateCondition.wait_for(WaitLock, std::chrono::duration<float>{ CurrentFixedDeltaSeconds }, [this]() { return !mIsFixedUpdateThreadRunning.load(); });
        }
    }

    void LuaBehaviorFramework::BindContextToEnvironment(RuntimeBehaviorInstance& TargetInstance) {
        TargetInstance.mEnvironment["This"] = &TargetInstance.mContext;
    }

    void LuaBehaviorFramework::ResolveBehaviorEntries(RuntimeBehaviorInstance& TargetInstance) {
        sol::object CandidateAwake{ TargetInstance.mEnvironment["Awake"] };
        sol::object CandidateOnEnable{ TargetInstance.mEnvironment["OnEnable"] };
        sol::object CandidateStart{ TargetInstance.mEnvironment["Start"] };
        sol::object CandidateUpdate{ TargetInstance.mEnvironment["Update"] };
        sol::object CandidateFixedUpdate{ TargetInstance.mEnvironment["FixedUpdate"] };
        sol::object CandidateLateUpdate{ TargetInstance.mEnvironment["LateUpdate"] };
        sol::object CandidateOnDisable{ TargetInstance.mEnvironment["OnDisable"] };
        sol::object CandidateOnDestroy{ TargetInstance.mEnvironment["OnDestroy"] };

        TargetInstance.mEntries.mAwake = CandidateAwake.get_type() == sol::type::function ? CandidateAwake.as<sol::protected_function>() : sol::protected_function{};
        TargetInstance.mEntries.mOnEnable = CandidateOnEnable.get_type() == sol::type::function ? CandidateOnEnable.as<sol::protected_function>() : sol::protected_function{};
        TargetInstance.mEntries.mStart = CandidateStart.get_type() == sol::type::function ? CandidateStart.as<sol::protected_function>() : sol::protected_function{};
        TargetInstance.mEntries.mUpdate = CandidateUpdate.get_type() == sol::type::function ? CandidateUpdate.as<sol::protected_function>() : sol::protected_function{};
        TargetInstance.mEntries.mFixedUpdate = CandidateFixedUpdate.get_type() == sol::type::function ? CandidateFixedUpdate.as<sol::protected_function>() : sol::protected_function{};
        TargetInstance.mEntries.mLateUpdate = CandidateLateUpdate.get_type() == sol::type::function ? CandidateLateUpdate.as<sol::protected_function>() : sol::protected_function{};
        TargetInstance.mEntries.mOnDisable = CandidateOnDisable.get_type() == sol::type::function ? CandidateOnDisable.as<sol::protected_function>() : sol::protected_function{};
        TargetInstance.mEntries.mOnDestroy = CandidateOnDestroy.get_type() == sol::type::function ? CandidateOnDestroy.as<sol::protected_function>() : sol::protected_function{};
    }
}
