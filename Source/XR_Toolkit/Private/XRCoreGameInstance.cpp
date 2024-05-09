
#include "XRCoreGameInstance.h"

void UXRCoreGameInstance::Init()
{
    const UXRCoreSettings* Settings = GetDefault<UXRCoreSettings>();
    if (Settings && Settings->UseCustomNetDriver)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            /**
            // Set the custom net driver for the world
            UXRCoreNetDriver* CustomNetDriver = NewObject<UXRCoreNetDriver>(World, UXRCoreNetDriver::StaticClass());
            World->SetNetDriver(CustomNetDriver);
            **/
        }
    }

    Super::Init();
}