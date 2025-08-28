#pragma once

#include "FmodAsset.h"

API_CLASS() class FLAXFMOD_API FmodBus : public FmodAsset
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE_WITH_CONSTRUCTOR_IMPL(FmodBus, FmodAsset);
public:

    /// <summary>
    /// Gets or sets the bus volume.
    /// </summary>
    API_PROPERTY() float GetVolume() const;

    /// <summary>
    /// Gets or sets the bus volume.
    /// </summary>
    API_PROPERTY() void SetVolume(float volume) const;

    /// <summary>
    /// Gets or sets if the bus is muted.
    /// </summary>
    API_PROPERTY() bool GetMuted() const;

    /// <summary>
    /// Gets or sets if the bus is muted.
    /// </summary>
    API_PROPERTY() void SetMuted(bool muted) const;

    /// <summary>
    /// Gets or sets if the bus is paused
    /// </summary>
    API_PROPERTY() bool GetPaused() const;

    /// <summary>
    /// Gets or sets if the bus is paused
    /// </summary>
    API_PROPERTY() void SetPaused(bool paused) const;
};
