#pragma once

#include "fmod_common.h"
#include "Engine/Core/Config.h"
#include "Engine/Core/ISerializable.h"
#include "Engine/Scripting/ScriptingObject.h"

API_CLASS() class FLAXFMOD_API FmodEvent : public ScriptingObject, public ISerializable
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE_WITH_CONSTRUCTOR_IMPL(FmodEvent, ScriptingObject);
public:

    API_FIELD() String Path;
    API_FIELD() String Guid;


    FMOD_GUID GetFmodGuid();
};
