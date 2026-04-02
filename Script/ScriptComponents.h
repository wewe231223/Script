#pragma once
#include <cstdint>
#include "Arche/Common.h"
#include "Utility/ComponentRestraint.h"

ComponentDecl(ScriptInstanceComponent, ((Arche::EntityID, mOwnerEntity))((std::uint32_t, mScriptInstanceId))((std::uint32_t, mScriptAssetId)), BOOST_PP_SEQ_NIL);
