#pragma once
#include <cstdint>
#include "Arche/Common.h"
#include "Utility/ComponentRestraint.h"

Component(ScriptInstanceComponent)
    Arche::EntityID mOwnerEntity{};
    std::uint32_t mScriptInstanceId{};
    std::uint32_t mScriptAssetId{};
EndComponent(ScriptInstanceComponent)
