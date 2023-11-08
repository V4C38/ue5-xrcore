#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "XRToolsUtilityFunctions.generated.h"

class UXRInteractionComponent;

UCLASS()
class XR_TOOLKIT_API UXRToolsUtilityFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	 Return if this Actor has an XRInteractionComponents any any XRInteractionComponents that were found.
	 */
	UFUNCTION(BlueprintPure, Category="XRTools|Utilities")
	static bool IsActorInteractive(AActor* InActor, TArray<UXRInteractionComponent*>& OutActiveXRInteractions);

	/**
	 Return if any XRInteractionComponent is present on this actor that is currently interacted with. Only applies to ContinuousInteractions.
	 */
	UFUNCTION(BlueprintPure, Category = "XRTools|Utilities")
	static bool IsActorInteractedWith(AActor* InActor, TArray<UXRInteractionComponent*>& OutActiveXRInteractions);


	/**
	 * Returns XRInteractionComponents on the closest overlapping Actor (that has XRInteractions) for this XRInteractor.
	 * Distance is calculated based on the Actors root, not the contained XRInteractionComponents location.
	 */
	UFUNCTION(BlueprintPure, Category = "XRTools|Utilities")
	static UXRInteractionComponent* GetPrioritizedXRInteraction(TArray<UXRInteractionComponent*> InInteractions, bool IgnoreActive = true, bool SortByLowest = true);
};
