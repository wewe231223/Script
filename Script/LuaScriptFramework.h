#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <functional>
#include <type_traits>
#include <filesystem>
#include "sol/sol.hpp"
#include "Arche/World.h"
#include "ScriptComponents.h"
#include "Utility/ComponentRestraint.h"

namespace Script {
    class LuaScriptFramework final {
    public:
        using ComponentGetter = std::function<sol::object(sol::state_view, Arche::World&, Arche::EntityID)>;

    public:
        class ScriptContext final {
        public:
            ScriptContext();
            ~ScriptContext();

            ScriptContext(const ScriptContext& Other) = delete;
            ScriptContext& operator=(const ScriptContext& Other) = delete;

            ScriptContext(ScriptContext&& Other) noexcept;
            ScriptContext& operator=(ScriptContext&& Other) noexcept;

        public:
            void Bind(Arche::World* TargetWorld, Arche::EntityID OwnerEntity, LuaScriptFramework* OwnerFramework);

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
            LuaScriptFramework* mOwnerFramework{};
        };

    private:
        struct RuntimeScriptInstance final {
            ScriptContext mContext{};
            sol::environment mEnvironment{};
            sol::protected_function mOnUpdate{};
            std::string mScriptFileName{};
        };

    public:
        LuaScriptFramework();
        ~LuaScriptFramework();

        LuaScriptFramework(const LuaScriptFramework& Other) = delete;
        LuaScriptFramework& operator=(const LuaScriptFramework& Other) = delete;

        LuaScriptFramework(LuaScriptFramework&& Other) noexcept = delete;
        LuaScriptFramework& operator=(LuaScriptFramework&& Other) noexcept = delete;

    public:
        void Initialize(Arche::World* TargetWorld);
        void OpenDefaultLibraries();

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

        bool AttachScript(Arche::EntityID TargetEntity, const std::string& ScriptSource, std::uint32_t ScriptAssetId);
        bool AttachScriptFromFile(Arche::EntityID TargetEntity, const std::string& ScriptFilePath, std::uint32_t ScriptAssetId);
        bool HotReloadScript(const std::string& ScriptFileName);
        void DetachScript(Arche::EntityID TargetEntity);
        void Update(float DeltaSeconds);

        sol::state& GetState();
        const sol::state& GetState() const;

    private:
        RuntimeScriptInstance* FindRuntimeInstance(Arche::EntityID TargetEntity);
        const RuntimeScriptInstance* FindRuntimeInstance(Arche::EntityID TargetEntity) const;

        std::uint32_t GenerateScriptInstanceId();
        bool ReadScriptSourceFromFilePath(const std::string& ScriptFilePath, std::string& OutScriptSource) const;
        void BindContextToEnvironment(RuntimeScriptInstance& TargetInstance);

    private:
        Arche::World* mWorld{};
        sol::state mLuaState{};
        std::unordered_map<std::string, ComponentGetter> mComponentGetters{};
        std::unordered_map<std::string, std::string> mScriptFilePaths{};
        std::unordered_map<std::uint32_t, RuntimeScriptInstance> mRuntimeInstances{};
        std::uint32_t mLastIssuedScriptInstanceId{};
    };
}

#include "LuaScriptFramework.inl"
