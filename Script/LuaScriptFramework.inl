#pragma once

namespace Script {
    template <typename TUsertype, typename TBindings, std::size_t... TIndices>
    void ApplyComponentBindingsToUsertypeImpl(TUsertype& TargetUsertype, const TBindings& TargetBindings, std::index_sequence<TIndices...>) {
        (static_cast<void>(TargetUsertype[std::get<TIndices>(TargetBindings).first] = std::get<TIndices>(TargetBindings).second), ...);
    }

    template <typename TDefinition, std::size_t... TIndices>
    void RegisterComponentUsertypeByDefinitionImpl(sol::state& LuaState, const TDefinition& Definition, std::index_sequence<TIndices...>) {
        auto Usertype{ LuaState.new_usertype<typename TDefinition::ComponentType>(Definition.mTypeName, sol::constructors<typename TDefinition::ComponentType()>()) };
        ApplyComponentBindingsToUsertypeImpl(Usertype, Definition.mFieldBindings, std::index_sequence<TIndices...>{});
    }

    template <TrivialComponent T>
    T* LuaScriptFramework::ScriptContext::GetComponent() {
        if (mWorld == nullptr) {
            return nullptr;
        }

        return mWorld->GetComponent<T>(mOwnerEntity);
    }

    template <TrivialComponent T>
    const T* LuaScriptFramework::ScriptContext::GetComponent() const {
        if (mWorld == nullptr) {
            return nullptr;
        }

        return mWorld->GetComponent<T>(mOwnerEntity);
    }

    template <TrivialComponent T>
    void LuaScriptFramework::RegisterComponent(const std::string& ComponentName) {
        mComponentGetters[ComponentName] = [](sol::state_view TargetState, Arche::World& TargetWorld, Arche::EntityID TargetEntity) {
            T* TargetComponent{ TargetWorld.GetComponent<T>(TargetEntity) };

            if (TargetComponent == nullptr) {
                return sol::make_object(TargetState, sol::lua_nil);
            }

            return sol::make_object(TargetState, std::ref(*TargetComponent));
        };
    }

    template <TrivialComponent T>
    requires HasLuaComponentDefinition<T>
    void LuaScriptFramework::RegisterComponentByDefinition() {
        constexpr auto Definition{ LuaComponentDefinitionTraits<T>::Create() };
        RegisterComponentUsertypeByDefinitionImpl(mLuaState, Definition, std::make_index_sequence<std::tuple_size_v<typename decltype(Definition)::FieldTuple>>{});
        RegisterComponent<T>(Definition.mTypeName);
    }

    template <typename T, typename... TArgs>
    void LuaScriptFramework::RegisterComponentUsertype(const std::string& ComponentName, TArgs&&... UsertypeArguments) {
        mLuaState.new_usertype<T>(ComponentName, std::forward<TArgs>(UsertypeArguments)...);
    }
}
