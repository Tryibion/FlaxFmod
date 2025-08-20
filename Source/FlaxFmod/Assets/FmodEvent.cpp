#include "FmodEvent.h"

#include "fmod_studio.hpp"

FMOD_GUID FmodEvent::GetFmodGuid()
{
    if (Guid == String::Empty)
        return {};
    FMOD_GUID guid;
    FMOD::Studio::parseID(Guid.ToStringAnsi().GetText(), &guid);
    return guid;
}
