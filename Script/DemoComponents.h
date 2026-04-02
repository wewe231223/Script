#pragma once
#include <cstdint>
#include "Utility/ComponentRestraint.h"

ComponentDecl(Vec3, ((float, mX))((float, mY))((float, mZ)), ((float GetLengthSquared() const, GetLengthSquared))((void NormalizeX(), NormalizeX)));

ComponentDecl(TransformComponent, ((Vec3, mPosition))((Vec3, mScale)), ((void ResetScale(), ResetScale)));

ComponentDecl(VelocityComponent, ((Vec3, mLinear)), ((float GetSpeedSquared() const, GetSpeedSquared)));

ComponentDecl(AccelerationComponent, ((Vec3, mLinear)), ((float GetAccelerationSquared() const, GetAccelerationSquared)));

ComponentDecl(HealthComponent, ((std::int32_t, mCurrent))((std::int32_t, mMax)), ((bool IsAlive() const, IsAlive)));

ComponentDecl(EnergyComponent, ((float, mCurrent))((float, mDrainPerSecond))((float, mRegenPerSecond)), ((bool HasEnergy() const, HasEnergy)));

ComponentDecl(FactionComponent, ((std::int32_t, mTeamId)), BOOST_PP_SEQ_NIL);

ComponentDecl(ValueOnlyComponent, ((std::int32_t, mValue)), BOOST_PP_SEQ_NIL);
