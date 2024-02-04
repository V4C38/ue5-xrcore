#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "XRCoreUtilities.generated.h"

class UXRInteractionComponent;
class UXRInteractorComponent;


UCLASS()
class XRCORE_API UXRCoreUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	 Return if this Actor has an XRInteractionComponents any any XRInteractionComponents that were found.
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Utilities")
	static bool IsActorInteractive(AActor* InActor, TArray<UXRInteractionComponent*>& OutActiveXRInteractions);

	/**
	 Return if any XRInteractionComponent is present on this actor that is currently interacted with. Only applies to ContinuousInteractions.
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Utilities")
	static bool IsActorInteractedWith(AActor* InActor, TArray<UXRInteractionComponent*>& OutActiveXRInteractions);


	/**
	 * Returns XRInteractionComponent with the highest Priority from an Array of InteractionComponents.
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Utilities")
	static UXRInteractionComponent* GetPrioritizedXRInteraction(TArray<UXRInteractionComponent*> InInteractions, UXRInteractorComponent* InXRInteractor = nullptr, 
		bool IgnoreActive = true, bool SortByLowest = true);

	/**
	 * Returns XRInteractionComponent with the highest Priority on an AActor*.
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Utilities")
	static UXRInteractionComponent* GetPrioritizedXRInteractionOnActor(AActor* InActor, UXRInteractorComponent* InXRInteractor = nullptr, 
		bool IgnoreActive = true, bool SortByLowest = true);
};
