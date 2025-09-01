#include "FmodBank.h"

#include "FlaxFmod/FmodAudio.h"

void FmodBank::Load(bool loadSampleData)
{
    FmodAudio::LoadBank(GetBankName(), loadSampleData);
}

void FmodBank::Unload()
{
    FmodAudio::UnloadBank(GetBankName());
}

bool FmodBank::IsLoaded()
{
    return FmodAudio::IsBankLoaded(GetBankName());
}

String FmodBank::GetBankName()
{
    auto name = Path;
    name.Replace(TEXT("bank:/"), TEXT(""), StringSearchCase::IgnoreCase);
    return name;
}
