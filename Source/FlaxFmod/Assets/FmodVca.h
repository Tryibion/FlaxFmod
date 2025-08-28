#pragma once

#include "FmodAsset.h"

API_CLASS() class FLAXFMOD_API FmodVca : public FmodAsset
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE_WITH_CONSTRUCTOR_IMPL(FmodVca, FmodAsset);

public:

    /// <summary>
    /// Gets or sets the VCA volume.
    /// </summary>
    API_PROPERTY() float GetVolume() const;

    /// <summary>
    /// Gets or sets the VCA volume.
    /// </summary>
    API_PROPERTY() void SetVolume(float volume) const;
};
