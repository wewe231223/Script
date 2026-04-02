#include <cmath>
#include <iostream>
#include "Arche/World.h"
#include "Script/LuaScriptFramework.h"
#include "Script/DemoComponents.h"

#pragma comment(lib, "lua54.lib")

bool AlmostEqual(float Left, float Right, float Epsilon) {
    return std::fabs(Left - Right) <= Epsilon;
}

int main(void) {
    Arche::World MainWorld{};

    TransformComponent TransformA{};
    TransformA.mPosition = Vec3{ 0.0f, 0.0f, 0.0f };
    TransformA.mScale = Vec3{ 1.0f, 1.0f, 1.0f };

    VelocityComponent VelocityA{};
    VelocityA.mLinear = Vec3{ 1.0f, 0.0f, 0.0f };

    AccelerationComponent AccelerationA{};
    AccelerationA.mLinear = Vec3{ 1.0f, 0.0f, 0.0f };

    HealthComponent HealthA{};
    HealthA.mCurrent = 20;
    HealthA.mMax = 20;

    EnergyComponent EnergyA{};
    EnergyA.mCurrent = 30.0f;
    EnergyA.mDrainPerSecond = 4.0f;
    EnergyA.mRegenPerSecond = 1.0f;

    FactionComponent FactionA{};
    FactionA.mTeamId = 1;

    ValueOnlyComponent ValueOnlyA{};
    ValueOnlyA.mValue = 11;

    Arche::EntityID EntityA{ MainWorld.CreateEntity<TransformComponent, VelocityComponent, AccelerationComponent, HealthComponent, EnergyComponent, FactionComponent, ValueOnlyComponent>(TransformA, VelocityA, AccelerationA, HealthA, EnergyA, FactionA, ValueOnlyA) };

    TransformComponent TransformB{};
    TransformB.mPosition = Vec3{ 10.0f, 0.0f, 0.0f };
    TransformB.mScale = Vec3{ 2.0f, 2.0f, 2.0f };

    VelocityComponent VelocityB{};
    VelocityB.mLinear = Vec3{ -2.0f, 1.0f, 0.0f };

    AccelerationComponent AccelerationB{};
    AccelerationB.mLinear = Vec3{ 0.0f, 1.0f, 0.0f };

    HealthComponent HealthB{};
    HealthB.mCurrent = 10;
    HealthB.mMax = 12;

    EnergyComponent EnergyB{};
    EnergyB.mCurrent = 40.0f;
    EnergyB.mDrainPerSecond = 3.0f;
    EnergyB.mRegenPerSecond = 6.0f;

    FactionComponent FactionB{};
    FactionB.mTeamId = 2;

    ValueOnlyComponent ValueOnlyB{};
    ValueOnlyB.mValue = 22;

    Arche::EntityID EntityB{ MainWorld.CreateEntity<TransformComponent, VelocityComponent, AccelerationComponent, HealthComponent, EnergyComponent, FactionComponent, ValueOnlyComponent>(TransformB, VelocityB, AccelerationB, HealthB, EnergyB, FactionB, ValueOnlyB) };

    TransformComponent TransformC{};
    TransformC.mPosition = Vec3{ 1.0f, 1.0f, 1.0f };
    TransformC.mScale = Vec3{ 1.0f, 1.0f, 1.0f };

    HealthComponent HealthC{};
    HealthC.mCurrent = 3;
    HealthC.mMax = 5;

    FactionComponent FactionC{};
    FactionC.mTeamId = 0;

    ValueOnlyComponent ValueOnlyC{};
    ValueOnlyC.mValue = 33;

    Arche::EntityID EntityC{ MainWorld.CreateEntity<TransformComponent, HealthComponent, FactionComponent, ValueOnlyComponent>(TransformC, HealthC, FactionC, ValueOnlyC) };

    Script::LuaScriptFramework Framework{};
    Framework.Initialize(&MainWorld);
    Framework.OpenDefaultLibraries();

    Framework.RegisterComponentByDefinition<Vec3>();
    Framework.RegisterComponentByDefinition<TransformComponent>();
    Framework.RegisterComponentByDefinition<VelocityComponent>();
    Framework.RegisterComponentByDefinition<AccelerationComponent>();
    Framework.RegisterComponentByDefinition<HealthComponent>();
    Framework.RegisterComponentByDefinition<EnergyComponent>();
    Framework.RegisterComponentByDefinition<FactionComponent>();
    Framework.RegisterComponentByDefinition<ValueOnlyComponent>();

    bool IsAttachedA{ Framework.AttachScriptFromFile(EntityA, "Script/Lua/DemoUpdate.lua", 1u) };
    bool IsAttachedB{ Framework.AttachScriptFromFile(EntityB, "Script/Lua/DemoUpdate.lua", 2u) };
    bool IsAttachedC{ Framework.AttachScriptFromFile(EntityC, "Script/Lua/DemoSupport.lua", 3u) };

    if (IsAttachedA == false || IsAttachedB == false || IsAttachedC == false) {
        std::cout << "스크립트 부착 실패" << std::endl;
        return 1;
    }

    Framework.Update(0.5f);
    Framework.Update(0.5f);
    Framework.Update(0.5f);

    TransformComponent* FinalTransformA{ MainWorld.GetComponent<TransformComponent>(EntityA) };
    VelocityComponent* FinalVelocityA{ MainWorld.GetComponent<VelocityComponent>(EntityA) };
    HealthComponent* FinalHealthA{ MainWorld.GetComponent<HealthComponent>(EntityA) };
    EnergyComponent* FinalEnergyA{ MainWorld.GetComponent<EnergyComponent>(EntityA) };
    ValueOnlyComponent* FinalValueOnlyA{ MainWorld.GetComponent<ValueOnlyComponent>(EntityA) };

    TransformComponent* FinalTransformB{ MainWorld.GetComponent<TransformComponent>(EntityB) };
    VelocityComponent* FinalVelocityB{ MainWorld.GetComponent<VelocityComponent>(EntityB) };
    HealthComponent* FinalHealthB{ MainWorld.GetComponent<HealthComponent>(EntityB) };
    EnergyComponent* FinalEnergyB{ MainWorld.GetComponent<EnergyComponent>(EntityB) };
    ValueOnlyComponent* FinalValueOnlyB{ MainWorld.GetComponent<ValueOnlyComponent>(EntityB) };

    TransformComponent* FinalTransformC{ MainWorld.GetComponent<TransformComponent>(EntityC) };
    HealthComponent* FinalHealthC{ MainWorld.GetComponent<HealthComponent>(EntityC) };
    ValueOnlyComponent* FinalValueOnlyC{ MainWorld.GetComponent<ValueOnlyComponent>(EntityC) };

    if (FinalTransformA == nullptr || FinalVelocityA == nullptr || FinalHealthA == nullptr || FinalEnergyA == nullptr || FinalValueOnlyA == nullptr || FinalTransformB == nullptr || FinalVelocityB == nullptr || FinalHealthB == nullptr || FinalEnergyB == nullptr || FinalValueOnlyB == nullptr || FinalTransformC == nullptr || FinalHealthC == nullptr || FinalValueOnlyC == nullptr) {
        std::cout << "컴포넌트 조회 실패" << std::endl;
        return 1;
    }

    std::cout << "EntityA PositionX: " << FinalTransformA->mPosition.mX << " VelocityX: " << FinalVelocityA->mLinear.mX << " Health: " << FinalHealthA->mCurrent << " Energy: " << FinalEnergyA->mCurrent << std::endl;
    std::cout << "EntityB PositionY: " << FinalTransformB->mPosition.mY << " VelocityY: " << FinalVelocityB->mLinear.mY << " Health: " << FinalHealthB->mCurrent << " Energy: " << FinalEnergyB->mCurrent << std::endl;
    std::cout << "EntityC PositionX: " << FinalTransformC->mPosition.mX << " Health: " << FinalHealthC->mCurrent << std::endl;

    bool IsEntityAValid{ AlmostEqual(FinalTransformA->mPosition.mX, 2.4f, 0.001f) && AlmostEqual(FinalVelocityA->mLinear.mX, 2.5f, 0.001f) && FinalHealthA->mCurrent == 14 && FinalHealthA->IsAlive() && AlmostEqual(FinalEnergyA->mCurrent, 24.0f, 0.001f) && FinalValueOnlyA->mValue == 11 };
    bool IsEntityBValid{ AlmostEqual(FinalTransformB->mPosition.mY, 3.6f, 0.001f) && AlmostEqual(FinalVelocityB->mLinear.mY, 2.5f, 0.001f) && FinalHealthB->mCurrent == 10 && FinalHealthB->IsAlive() && AlmostEqual(FinalEnergyB->mCurrent, 49.0f, 0.001f) && FinalValueOnlyB->mValue == 22 };
    bool IsEntityCValid{ AlmostEqual(FinalTransformC->mPosition.mX, 1.0f, 0.001f) && FinalHealthC->mCurrent == 5 && FinalHealthC->IsAlive() && FinalValueOnlyC->mValue == 33 };

    if (IsEntityAValid == false || IsEntityBValid == false || IsEntityCValid == false) {
        std::cout << "검증 실패" << std::endl;
        return 1;
    }

    std::cout << "다중 엔티티 Lua 동작 검증 성공" << std::endl;
    return 0;
}
