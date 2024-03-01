#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "XRToolsUtilityFunctions.generated.h"

class UXRInteractionComponent;
class UXRInteractorComponent;


UCLASS()
class XR_TOOLKIT_API UXRToolsUtilityFunctions : public UBlueprintFunctionLibrary
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
	 * @param InXRInteractor Optional, provide to validate the interaction for this XRInteractor specifically
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Utilities")
	static UXRInteractionComponent* GetHighestPrioriyXRInteraction(TArray<UXRInteractionComponent*> InInteractions, UXRInteractorComponent* InXRInteractor = nullptr);

	/**
	 * Returns the first XRInteractionComponent with provided Priority Value.
	 * @param InXRInteractor Optional, provide to validate the interaction for this XRInteractor specifically
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Utilities")
	static UXRInteractionComponent* GetXRInteractionByPriority(TArray<UXRInteractionComponent*> InInteractions, UXRInteractorComponent* InXRInteractor = nullptr, int32 InPriority = 0);

	/**
	 * Returns true if this Actor has an XRInteractorComponent
	 * @param InXRInteractor Optional, provide to validate if the interaction are avilable to this XRInteractor specifically
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Utilities")
	static UXRInteractionComponent* GetXRInteractionOnActorByPriority(AActor* InActor, UXRInteractorComponent* InXRInteractor = nullptr, int32 InPriority = 0);
};
