#pragma once
#include "Engine/Core/Config.h"
#include "Engine/Core/ISerializable.h"
#include "Engine/Core/Types/String.h"
#include "Engine/Scripting/ScriptingType.h"

API_STRUCT() struct FLAXFMOD_API FmodParameter: public ISerializable
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE_STRUCTURE(FmodParameter);
public:
    
    API_FIELD() String Name;
    API_FIELD() float Value;
};
