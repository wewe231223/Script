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

namespace SimpleMath {
    struct Matrix4x4 final {
        float mM11{};
        float mM12{};
        float mM13{};
        float mM14{};
        float mM21{};
        float mM22{};
        float mM23{};
        float mM24{};
        float mM31{};
        float mM32{};
        float mM33{};
        float mM34{};
        float mM41{};
        float mM42{};
        float mM43{};
        float mM44{};

        static Matrix4x4 CreateIdentity();
        float GetTrace() const;
        void SetDiagonal(float Value);
    };
}

LuaTypeDefinitionDeclWithName(
    SimpleMath::Matrix4x4,
    "SimpleMathMatrix4x4",
    ComponentFields(
        ComponentField(float, mM11)
        ComponentField(float, mM12)
        ComponentField(float, mM13)
        ComponentField(float, mM14)
        ComponentField(float, mM21)
        ComponentField(float, mM22)
        ComponentField(float, mM23)
        ComponentField(float, mM24)
        ComponentField(float, mM31)
        ComponentField(float, mM32)
        ComponentField(float, mM33)
        ComponentField(float, mM34)
        ComponentField(float, mM41)
        ComponentField(float, mM42)
        ComponentField(float, mM43)
        ComponentField(float, mM44)
    ),
    ComponentMethods(
        ComponentMethod(float GetTrace() const, GetTrace)
        ComponentMethod(void SetDiagonal(float Value), SetDiagonal)
    )
);

LuaStdArrayTypeDefinitionDeclWithName(float, 3, "Float3Array");
