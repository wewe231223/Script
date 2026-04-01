#pragma once
#include <cstdint>
#include "Utility/ComponentRestraint.h"

Component(Vec3)
    float mX{};
    float mY{};
    float mZ{};
EndComponent(Vec3)

Component(TransformComponent)
    Vec3 mPosition{};
    Vec3 mScale{};
EndComponent(TransformComponent)

Component(VelocityComponent)
    Vec3 mLinear{};
EndComponent(VelocityComponent)

Component(AccelerationComponent)
    Vec3 mLinear{};
EndComponent(AccelerationComponent)

Component(HealthComponent)
    std::int32_t mCurrent{};
    std::int32_t mMax{};
EndComponent(HealthComponent)

Component(EnergyComponent)
    float mCurrent{};
    float mDrainPerSecond{};
    float mRegenPerSecond{};
EndComponent(EnergyComponent)

Component(FactionComponent)
    std::int32_t mTeamId{};
EndComponent(FactionComponent)
