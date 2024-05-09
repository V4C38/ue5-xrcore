
#include "XRCoreNetDriver.h"

#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogXRCoreGameInstance, Log, All);
DEFINE_LOG_CATEGORY(LogXRCoreGameInstance);



void UXRCoreNetDriver::ProcessRemoteFunction(AActor* Actor, UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack, UObject* SubObject)
{
    if (Actor && Actor->GetClass()->ImplementsInterface(UXRCoreBypassAuthorityCheck::StaticClass()))
    {
        Actor->ProcessEvent(Function, Parameters);
        UE_LOG(LogXRCoreGameInstance, Log, TEXT("Using CustomNetDriver UXRCoreNetDriver to process RPC without authority."));
        return;
    }
    Super::ProcessRemoteFunction(Actor, Function, Parameters, OutParms, Stack, SubObject);
}