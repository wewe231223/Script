#include <iostream>
#include "Arche/World.h"
#include "Script/LuaScriptFramework.h"
#include "Script/DemoComponents.h"

#pragma comment(lib, "lua54.lib")

int main(void) {
    Arche::World MainWorld{};

    TransformComponent Transform{};
    Transform.mPosition = Vec3{ 0.0f, 0.0f, 0.0f };
    Transform.mScale = Vec3{ 1.0f, 1.0f, 1.0f };

    VelocityComponent Velocity{};
    Velocity.mLinear = Vec3{ 1.5f, -0.25f, 0.5f };

    HealthComponent Health{};
    Health.mCurrent = 20;
    Health.mMax = 20;

    Arche::EntityID TargetEntity{ MainWorld.CreateEntity<TransformComponent, VelocityComponent, HealthComponent>(Transform, Velocity, Health) };

    Script::LuaScriptFramework Framework{};
    Framework.Initialize(&MainWorld);
    Framework.OpenDefaultLibraries();

    Framework.RegisterComponentUsertype<Vec3>("Vec3", sol::constructors<Vec3()>(), "mX", &Vec3::mX, "mY", &Vec3::mY, "mZ", &Vec3::mZ);
    Framework.RegisterComponentUsertype<TransformComponent>("TransformComponent", sol::constructors<TransformComponent()>(), "mPosition", &TransformComponent::mPosition, "mScale", &TransformComponent::mScale);
    Framework.RegisterComponentUsertype<VelocityComponent>("VelocityComponent", sol::constructors<VelocityComponent()>(), "mLinear", &VelocityComponent::mLinear);
    Framework.RegisterComponentUsertype<HealthComponent>("HealthComponent", sol::constructors<HealthComponent()>(), "mCurrent", &HealthComponent::mCurrent, "mMax", &HealthComponent::mMax);

    Framework.RegisterComponent<TransformComponent>("TransformComponent");
    Framework.RegisterComponent<VelocityComponent>("VelocityComponent");
    Framework.RegisterComponent<HealthComponent>("HealthComponent");

    bool IsAttached{ Framework.AttachScriptFromFile(TargetEntity, "Script/Lua/DemoUpdate.lua", 1u) };

    if (IsAttached == false) {
        std::cout << "스크립트 부착 실패" << std::endl;
        return 1;
    }

    Framework.Update(0.5f);
    Framework.Update(0.5f);
    Framework.Update(0.5f);

    TransformComponent* FinalTransform{ MainWorld.GetComponent<TransformComponent>(TargetEntity) };
    HealthComponent* FinalHealth{ MainWorld.GetComponent<HealthComponent>(TargetEntity) };

    if (FinalTransform == nullptr || FinalHealth == nullptr) {
        std::cout << "컴포넌트 조회 실패" << std::endl;
        return 1;
    }

    std::cout << "Position: " << FinalTransform->mPosition.mX << ", " << FinalTransform->mPosition.mY << ", " << FinalTransform->mPosition.mZ << std::endl;
    std::cout << "Health: " << FinalHealth->mCurrent << "/" << FinalHealth->mMax << std::endl;

    bool IsPositionValid{ FinalTransform->mPosition.mX > 2.24f && FinalTransform->mPosition.mX < 2.26f };
    bool IsHealthValid{ FinalHealth->mCurrent == 17 };

    if (IsPositionValid == false || IsHealthValid == false) {
        std::cout << "검증 실패" << std::endl;
        return 1;
    }

    std::cout << "Lua 동작 검증 성공" << std::endl;
    return 0;
}
