#pragma once
#include <cstdint>
#include "Utility/ComponentRestraint.h"

ComponentDecl(
    Vec3,
    ComponentFields(
        ComponentField(float, mX)
        ComponentField(float, mY)
        ComponentField(float, mZ)
    ),
    ComponentMethods(
        ComponentMethod(float GetLengthSquared() const, GetLengthSquared)
        ComponentMethod(void NormalizeX(), NormalizeX)
    )
);

ComponentDecl(
    TransformComponent,
    ComponentFields(
        ComponentField(Vec3, mPosition)
        ComponentField(Vec3, mScale)
    ),
    ComponentMethods(
        ComponentMethod(void ResetScale(), ResetScale)
    )
);

ComponentDecl(
    VelocityComponent,
    ComponentFields(
        ComponentField(Vec3, mLinear)
    ),
    ComponentMethods(
        ComponentMethod(float GetSpeedSquared() const, GetSpeedSquared)
    )
);

ComponentDecl(
    AccelerationComponent,
    ComponentFields(
        ComponentField(Vec3, mLinear)
    ),
    ComponentMethods(
        ComponentMethod(float GetAccelerationSquared() const, GetAccelerationSquared)
    )
);

ComponentDecl(
    HealthComponent,
    ComponentFields(
        ComponentField(std::int32_t, mCurrent)
        ComponentField(std::int32_t, mMax)
    ),
    ComponentMethods(
        ComponentMethod(bool IsAlive() const, IsAlive)
    )
);

ComponentDecl(
    EnergyComponent,
    ComponentFields(
        ComponentField(float, mCurrent)
        ComponentField(float, mDrainPerSecond)
        ComponentField(float, mRegenPerSecond)
    ),
    ComponentMethods(
        ComponentMethod(bool HasEnergy() const, HasEnergy)
    )
);

ComponentDecl(
    FactionComponent,
    ComponentFields(
        ComponentField(std::int32_t, mTeamId)
    ),
    BOOST_PP_SEQ_NIL
);

ComponentDecl(
    ValueOnlyComponent,
    ComponentFields(
        ComponentField(std::int32_t, mValue)
    ),
    BOOST_PP_SEQ_NIL
);
