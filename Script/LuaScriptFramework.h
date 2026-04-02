#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include "sol/sol.hpp"
#include "Arche/World.h"
#include "ScriptComponents.h"
#include "Utility/ComponentRestraint.h"

namespace Script {
    class LuaBehaviorFramework final {
    public:
        using ComponentGetter = std::function<sol::object(sol::state_view, Arche::World&, Arche::EntityID)>;

    public:
        class BehaviorContext final {
        public:
            BehaviorContext();
            ~BehaviorContext();
            BehaviorContext(const BehaviorContext& Other) = delete;
            BehaviorContext& operator=(const BehaviorContext& Other) = delete;
            BehaviorContext(BehaviorContext&& Other) noexcept;
            BehaviorContext& operator=(BehaviorContext&& Other) noexcept;

        public:
            void Bind(Arche::World* TargetWorld, Arche::EntityID OwnerEntity, LuaBehaviorFramework* OwnerFramework);
            Arche::EntityID GetEntityId() const;

            template <TrivialComponent T>
            T* GetComponent();

            template <TrivialComponent T>
            const T* GetComponent() const;

            bool HasComponent(const std::string& ComponentName) const;
            sol::object GetComponent(const std::string& ComponentName);

        private:
            Arche::World* mWorld{};
            Arche::EntityID mOwnerEntity{};
            LuaBehaviorFramework* mOwnerFramework{};
        };

    private:
        struct BehaviorEntries final {
            sol::protected_function mAwake{};
            sol::protected_function mOnEnable{};
            sol::protected_function mStart{};
            sol::protected_function mUpdate{};
            sol::protected_function mFixedUpdate{};
            sol::protected_function mLateUpdate{};
            sol::protected_function mOnDisable{};
            sol::protected_function mOnDestroy{};
        };

        struct RuntimeBehaviorInstance final {
            BehaviorContext mContext{};
            sol::environment mEnvironment{};
            BehaviorEntries mEntries{};
            std::string mBehaviorFileName{};
            std::uint32_t mFixedTick{};
            bool mIsEnabled{ true };
        };

    public:
        LuaBehaviorFramework();
        ~LuaBehaviorFramework();
        LuaBehaviorFramework(const LuaBehaviorFramework& Other) = delete;
        LuaBehaviorFramework& operator=(const LuaBehaviorFramework& Other) = delete;
        LuaBehaviorFramework(LuaBehaviorFramework&& Other) noexcept = delete;
        LuaBehaviorFramework& operator=(LuaBehaviorFramework&& Other) noexcept = delete;

    public:
        void Initialize(Arche::World* TargetWorld);
        void OpenDefaultLibraries();
        void SetFixedUpdateInterval(float FixedDeltaSeconds);

        template <TrivialComponent T>
        void RegisterComponent(const std::string& ComponentName);

        template <TrivialComponent T>
        requires HasLuaComponentDefinition<T>
        void RegisterComponentByDefinition();

        template <typename T>
        requires HasLuaTypeDefinition<T>
        void RegisterTypeByDefinition();

        template <typename T, typename... TArgs>
        void RegisterComponentUsertype(const std::string& ComponentName, TArgs&&... UsertypeArguments);

        template <typename T, typename... TArgs>
        void RegisterTypeUsertype(const std::string& TypeName, TArgs&&... UsertypeArguments);

        bool AttachBehavior(Arche::EntityID TargetEntity, const std::string& BehaviorSource, std::uint32_t BehaviorAssetId);
        bool AttachBehaviorFromFile(Arche::EntityID TargetEntity, const std::string& BehaviorFilePath, std::uint32_t BehaviorAssetId);
        bool HotReloadBehavior(const std::string& BehaviorFileName);
        bool DisableBehavior(Arche::EntityID TargetEntity);
        bool DestroyBehavior(Arche::EntityID TargetEntity);

        void DetachBehavior(Arche::EntityID TargetEntity);
        void Update(float DeltaSeconds);
        void FixedUpdate(float FixedDeltaSeconds);
        void LateUpdate(float DeltaSeconds);

        sol::state& GetState();
        const sol::state& GetState() const;

    private:
        RuntimeBehaviorInstance* FindRuntimeInstance(Arche::EntityID TargetEntity);
        const RuntimeBehaviorInstance* FindRuntimeInstance(Arche::EntityID TargetEntity) const;
        std::uint32_t GenerateBehaviorInstanceId();
        bool ReadBehaviorSourceFromFilePath(const std::string& BehaviorFilePath, std::string& OutBehaviorSource) const;

        void StartFixedUpdateThread();
        void StopFixedUpdateThread();
        void FixedUpdateThreadMain();
        void BindContextToEnvironment(RuntimeBehaviorInstance& TargetInstance);
        void ResolveBehaviorEntries(RuntimeBehaviorInstance& TargetInstance);

        template <typename... TArgs>
        bool RunEntryWithArguments(sol::protected_function& EntryFunction, TArgs&&... Arguments);

    private:
        Arche::World* mWorld{};
        sol::state mLuaState{};
        std::unordered_map<std::string, ComponentGetter> mComponentGetters{};
        std::unordered_map<std::string, std::string> mBehaviorFilePaths{};
        std::unordered_map<std::uint32_t, RuntimeBehaviorInstance> mRuntimeInstances{};
        std::uint32_t mLastIssuedBehaviorInstanceId{};
        float mFixedDeltaSeconds{ 1.0f };
        std::thread mFixedUpdateThread{};
        std::atomic<bool> mIsFixedUpdateThreadRunning{ false };
        std::condition_variable mFixedUpdateCondition{};
        std::mutex mRuntimeMutex{};
    };
}

#include "LuaScriptFramework.inl"
