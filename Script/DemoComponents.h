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

Component(HealthComponent)
    std::int32_t mCurrent{};
    std::int32_t mMax{};
EndComponent(HealthComponent)
