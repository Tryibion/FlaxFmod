#pragma once

#include "FmodEvent.h"

API_CLASS() class FLAXFMOD_API FmodSnapshot : public FmodEvent
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE_WITH_CONSTRUCTOR_IMPL(FmodSnapshot, FmodEvent);
};

