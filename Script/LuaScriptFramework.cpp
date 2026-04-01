#include <fstream>
#include <sstream>
#include "LuaScriptFramework.h"

using namespace Script;

LuaScriptFramework::ScriptContext::ScriptContext()
    : mWorld{},
    mOwnerEntity{},
    mOwnerFramework{} {
}

LuaScriptFramework::ScriptContext::~ScriptContext() {
}

LuaScriptFramework::ScriptContext::ScriptContext(ScriptContext&& Other) noexcept
    : mWorld{ Other.mWorld },
    mOwnerEntity{ Other.mOwnerEntity },
    mOwnerFramework{ Other.mOwnerFramework } {
}

LuaScriptFramework::ScriptContext& LuaScriptFramework::ScriptContext::operator=(ScriptContext&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mWorld = Other.mWorld;
    mOwnerEntity = Other.mOwnerEntity;
    mOwnerFramework = Other.mOwnerFramework;
    return *this;
}

void LuaScriptFramework::ScriptContext::Bind(Arche::World* TargetWorld, Arche::EntityID OwnerEntity, LuaScriptFramework* OwnerFramework) {
    mWorld = TargetWorld;
    mOwnerEntity = OwnerEntity;
    mOwnerFramework = OwnerFramework;
}

Arche::EntityID LuaScriptFramework::ScriptContext::GetEntityId() const {
    return mOwnerEntity;
}

bool LuaScriptFramework::ScriptContext::HasComponent(const std::string& ComponentName) const {
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

sol::object LuaScriptFramework::ScriptContext::GetComponent(const std::string& ComponentName) {
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

LuaScriptFramework::LuaScriptFramework()
    : mWorld{},
    mLuaState{},
    mComponentGetters{},
    mRuntimeInstances{},
    mLastIssuedScriptInstanceId{} {
}

LuaScriptFramework::~LuaScriptFramework() {
}

void LuaScriptFramework::Initialize(Arche::World* TargetWorld) {
    mWorld = TargetWorld;

    mLuaState.new_usertype<ScriptContext>("ScriptContext", "GetEntityId", &ScriptContext::GetEntityId, "GetComponent", static_cast<sol::object(ScriptContext::*)(const std::string&)>(&ScriptContext::GetComponent), "HasComponent", &ScriptContext::HasComponent);
}

void LuaScriptFramework::OpenDefaultLibraries() {
    mLuaState.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::package);
}

bool LuaScriptFramework::AttachScript(Arche::EntityID TargetEntity, const std::string& ScriptSource, std::uint32_t ScriptAssetId) {
    if (mWorld == nullptr) {
        return false;
    }

    if (mWorld->GetComponent<ScriptInstanceComponent>(TargetEntity) != nullptr) {
        return false;
    }

    std::uint32_t ScriptInstanceId{ GenerateScriptInstanceId() };
    ScriptInstanceComponent NewComponent{};
    NewComponent.mOwnerEntity = TargetEntity;
    NewComponent.mScriptInstanceId = ScriptInstanceId;
    NewComponent.mScriptAssetId = ScriptAssetId;
    mWorld->AddComponent<ScriptInstanceComponent>(TargetEntity, NewComponent);

    RuntimeScriptInstance NewRuntimeInstance{};
    NewRuntimeInstance.mContext.Bind(mWorld, TargetEntity, this);
    NewRuntimeInstance.mEnvironment = sol::environment(mLuaState, sol::create, mLuaState.globals());
    BindContextToEnvironment(NewRuntimeInstance);

    sol::protected_function_result LoadResult{ mLuaState.safe_script(ScriptSource, NewRuntimeInstance.mEnvironment, sol::script_pass_on_error) };

    if (!LoadResult.valid()) {
        return false;
    }

    sol::object Candidate{ NewRuntimeInstance.mEnvironment["Update"] };

    if (Candidate.get_type() == sol::type::function) {
        NewRuntimeInstance.mOnUpdate = Candidate.as<sol::protected_function>();
    }

    mRuntimeInstances.emplace(ScriptInstanceId, std::move(NewRuntimeInstance));
    return true;
}


bool LuaScriptFramework::AttachScriptFromFile(Arche::EntityID TargetEntity, const std::string& ScriptFilePath, std::uint32_t ScriptAssetId) {
    std::ifstream InputStream{ ScriptFilePath, std::ios::in | std::ios::binary };

    if (!InputStream.is_open()) {
        return false;
    }

    std::stringstream Buffer{};
    Buffer << InputStream.rdbuf();
    std::string ScriptSource{ Buffer.str() };

    return AttachScript(TargetEntity, ScriptSource, ScriptAssetId);
}

void LuaScriptFramework::DetachScript(Arche::EntityID TargetEntity) {
    if (mWorld == nullptr) {
        return;
    }

    ScriptInstanceComponent* InstanceComponent{ mWorld->GetComponent<ScriptInstanceComponent>(TargetEntity) };

    if (InstanceComponent == nullptr) {
        return;
    }

    mRuntimeInstances.erase(InstanceComponent->mScriptInstanceId);
}

void LuaScriptFramework::Update(float DeltaSeconds) {
    for (auto& RuntimePair : mRuntimeInstances) {
        RuntimeScriptInstance& RuntimeInstance{ RuntimePair.second };

        if (!RuntimeInstance.mOnUpdate.valid()) {
            continue;
        }

        sol::protected_function_result Result{ RuntimeInstance.mOnUpdate(RuntimeInstance.mContext, DeltaSeconds) };

        if (!Result.valid()) {
            continue;
        }
    }
}

sol::state& LuaScriptFramework::GetState() {
    return mLuaState;
}

const sol::state& LuaScriptFramework::GetState() const {
    return mLuaState;
}

LuaScriptFramework::RuntimeScriptInstance* LuaScriptFramework::FindRuntimeInstance(Arche::EntityID TargetEntity) {
    if (mWorld == nullptr) {
        return nullptr;
    }

    ScriptInstanceComponent* InstanceComponent{ mWorld->GetComponent<ScriptInstanceComponent>(TargetEntity) };

    if (InstanceComponent == nullptr) {
        return nullptr;
    }

    auto RuntimeIt{ mRuntimeInstances.find(InstanceComponent->mScriptInstanceId) };

    if (RuntimeIt == mRuntimeInstances.end()) {
        return nullptr;
    }

    return &RuntimeIt->second;
}

const LuaScriptFramework::RuntimeScriptInstance* LuaScriptFramework::FindRuntimeInstance(Arche::EntityID TargetEntity) const {
    return const_cast<LuaScriptFramework*>(this)->FindRuntimeInstance(TargetEntity);
}

std::uint32_t LuaScriptFramework::GenerateScriptInstanceId() {
    ++mLastIssuedScriptInstanceId;
    return mLastIssuedScriptInstanceId;
}

void LuaScriptFramework::BindContextToEnvironment(RuntimeScriptInstance& TargetInstance) {
    TargetInstance.mEnvironment["This"] = &TargetInstance.mContext;
}
