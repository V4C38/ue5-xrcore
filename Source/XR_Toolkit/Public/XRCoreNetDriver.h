
#pragma once

#include "CoreMinimal.h"
#include "Engine/NetDriver.h"
#include "UObject/Interface.h"
#include "XRCoreNetDriver.generated.h"


UINTERFACE(MinimalAPI, BlueprintType)
class UXRCoreBypassAuthorityCheck : public UInterface
{
	GENERATED_BODY()
};

class IXRCoreBypassAuthorityCheck
{
	GENERATED_BODY()
};

/**
 * Allows to effectively call RPCs from any Client without authority.
 * Ignores the authority check for RPCs when an actor has the IXRBypassAuthorityCheck Interface applied. 
 */
UCLASS()
class XR_TOOLKIT_API UXRCoreNetDriver : public UNetDriver
{
	GENERATED_BODY()

public:
	virtual void ProcessRemoteFunction(AActor* Actor, UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack, UObject* SubObject) override;

};
