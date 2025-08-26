#include "FmodAsset.h"

#include "fmod_studio.hpp"

FMOD_GUID FmodAsset::GetFmodGuid() const
{
    if (Guid == String::Empty)
        return {};
    FMOD_GUID guid;
    FMOD::Studio::parseID(Guid.ToStringAnsi().GetText(), &guid);
    return guid;
}
