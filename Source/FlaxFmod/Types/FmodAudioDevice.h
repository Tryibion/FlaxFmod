#pragma once
#include "Engine/Core/Config.h"
#include "Engine/Scripting/ScriptingObject.h"

API_CLASS(NoSpawn) class FLAXFMOD_API FmodAudioDevice : public ScriptingObject
{
    DECLARE_SCRIPTING_TYPE_NO_SPAWN(FmodAdioDevice);

    explicit FmodAudioDevice()
        : ScriptingObject(SpawnParams(Guid::New(), TypeInitializer))
    {
    }

    FmodAudioDevice(const FmodAudioDevice& other)
        : ScriptingObject(SpawnParams(Guid::New(), TypeInitializer))
    {
        Name = other.Name;
        InternalName = other.InternalName;
    }

    FmodAudioDevice& operator=(const FmodAudioDevice& other)
    {
        Name = other.Name;
        InternalName = other.InternalName;
        return *this;
    }
 
public:
    /// <summary>
    /// The device name.
    /// </summary>
    API_FIELD(ReadOnly) String Name;

    /// <summary>
    /// The internal device name used by the audio backend.
    /// </summary>
    StringAnsi InternalName;

    String ToString() const override
    {
        return Name;
    }
};
