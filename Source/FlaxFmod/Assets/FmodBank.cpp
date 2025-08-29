#include "FmodBank.h"

#include "FlaxFmod/FmodAudio.h"

void FmodBank::Load()
{
    FmodAudio::LoadBank(GetPathName());
}

void FmodBank::Unload()
{
    FmodAudio::UnloadBank(GetPathName());
}

bool FmodBank::IsLoaded()
{
    return FmodAudio::IsBankLoaded(GetPathName());
}

String FmodBank::GetPathName()
{
    auto name = Path;
    name.Replace(TEXT("bank:/"), TEXT(""), StringSearchCase::IgnoreCase);
    return name;
}
