#pragma once

#include "FmodAsset.h"

API_CLASS() class FLAXFMOD_API FmodEvent : public FmodAsset
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE_WITH_CONSTRUCTOR_IMPL(FmodEvent, FmodAsset);
};
