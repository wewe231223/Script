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

SimpleMath::Matrix4x4 SimpleMath::Matrix4x4::CreateIdentity() {
    Matrix4x4 Result{};
    Result.mM11 = 1.0f;
    Result.mM22 = 1.0f;
    Result.mM33 = 1.0f;
    Result.mM44 = 1.0f;
    return Result;
}

float SimpleMath::Matrix4x4::GetTrace() const {
    float Result{ mM11 + mM22 + mM33 + mM44 };
    return Result;
}

void SimpleMath::Matrix4x4::SetDiagonal(float Value) {
    mM11 = Value;
    mM22 = Value;
    mM33 = Value;
    mM44 = Value;
}
