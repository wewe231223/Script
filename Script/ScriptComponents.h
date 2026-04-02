#pragma once
#include <cstdint>
#include "Arche/Common.h"
#include "Utility/ComponentRestraint.h"

ComponentDecl(
    ScriptInstanceComponent,
    ComponentFields(
        ComponentField(Arche::EntityID, mOwnerEntity)
        ComponentField(std::uint32_t, mScriptInstanceId)
        ComponentField(std::uint32_t, mScriptAssetId)
    ),
    BOOST_PP_SEQ_NIL
);
