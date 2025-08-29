#pragma once

#include "FmodAsset.h"

API_CLASS() class FLAXFMOD_API FmodBank : public FmodAsset
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE_WITH_CONSTRUCTOR_IMPL(FmodBank, FmodAsset);
public:

    /// <summary>
    /// Loads this bank.
    /// </summary>
    API_FUNCTION() void Load();

    /// <summary>
    /// Unloads this bank.
    /// </summary>
    API_FUNCTION() void Unload();

    /// <summary>
    /// Is this bank loaded.
    /// </summary>
    API_FUNCTION() bool IsLoaded();

    String GetBankName();
};
