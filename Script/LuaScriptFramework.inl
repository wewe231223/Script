#pragma once

namespace Script {
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

    template <typename T, typename... TArgs>
    void LuaScriptFramework::RegisterComponentUsertype(const std::string& ComponentName, TArgs&&... UsertypeArguments) {
        mLuaState.new_usertype<T>(ComponentName, std::forward<TArgs>(UsertypeArguments)...);
    }
}
