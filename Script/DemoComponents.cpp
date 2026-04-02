#include <cmath>
#include "DemoComponents.h"

float Vec3::GetLengthSquared() const {
    float LengthSquared{ mX * mX + mY * mY + mZ * mZ };
    return LengthSquared;
}

void Vec3::NormalizeX() {
    float Length{ std::sqrt(GetLengthSquared()) };

    if (Length <= 0.0f) {
        return;
    }

    mX = mX / Length;
}

void TransformComponent::ResetScale() {
    mScale = Vec3{ 1.0f, 1.0f, 1.0f };
}

float VelocityComponent::GetSpeedSquared() const {
    float SpeedSquared{ mLinear.GetLengthSquared() };
    return SpeedSquared;
}

float AccelerationComponent::GetAccelerationSquared() const {
    float AccelerationSquared{ mLinear.GetLengthSquared() };
    return AccelerationSquared;
}

bool HealthComponent::IsAlive() const {
    bool Result{ mCurrent > 0 };
    return Result;
}

bool EnergyComponent::HasEnergy() const {
    bool Result{ mCurrent > 0.0f };
    return Result;
}
