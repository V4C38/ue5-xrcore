#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Core/XRCoreSettings.h"
#include "Core/XRCoreTypes.h"
#include "Interactions/XRInteractionTypes.h"

#include "XRToolsUtilityFunctions.generated.h"

class UXRInteractionComponent;
class UXRInteractorComponent;


// ================================================================================================================================================================
// XRCore utility functions and helüers
// ================================================================================================================================================================

UCLASS()
class XR_TOOLKIT_API UXRToolsUtilityFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	/**
	 Get the XRStandard used by this project. 
     * Specifies whether if the default OpenXR plugin or MetaXR is used for this project.
	 * See XRCore Plugin Settings. 
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Utilities")
	static EXRStandard GetXRStandard();

	/**
	 Return if this Actor has an XRInteractionComponents any any XRInteractionComponents that were found.
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Utilities")
	static bool IsActorInteractive(AActor* InActor, TArray<UXRInteractionComponent*>& OutXRInteractions);

	/**
	 Return if any XRInteractionComponent is present on this actor that is currently interacted with. Only applies to ContinuousInteractions.
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Utilities")
	static bool IsActorInteractedWith(AActor* InActor, TArray<UXRInteractionComponent*>& OutActiveXRInteractions);

	/**
	 * Returns the first XRInteractionComponent with provided Priority Value. 
	 * If specified by InPrioritySelectionCondition, returns either the next highest, or next lowest priority Interaction if no exactly matching priority interaction is found.
	 * @param InXRInteractor Optional, provide to validate the interaction for this XRInteractor specifically
	 * @InPrioritySelectionCondition Determine whether only exactly matching priorities should be returned.
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Utilities")
	static UXRInteractionComponent* GetXRInteractionByPriority(TArray<UXRInteractionComponent*> InInteractions, UXRInteractorComponent* InXRInteractor = nullptr, int32 InPriority = 0, 
		EXRInteractionPrioritySelection InPrioritySelectionCondition = EXRInteractionPrioritySelection::LowerEqual, int32 MaxSecondaryPriority = 5);

	/**
	 * Returns true if this Actor has an XRInteractorComponent
	 * @param InXRInteractor Optional, provide to validate if the interaction are avilable to this XRInteractor specifically
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Utilities")
	static UXRInteractionComponent* GetXRInteractionOnActorByPriority(AActor* InActor, UXRInteractorComponent* InXRInteractor = nullptr, int32 InPriority = 0, 
		EXRInteractionPrioritySelection InPrioritySelectionCondition = EXRInteractionPrioritySelection::LowerEqual);


	/**
	* Attempts to connect a Connector to an Actor (if the Actor has an available Socket).
	* @param InSocketID If no ID is provided, it will attempt to connect to the first Socket found on the Actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "XRCore|Utilities")
	static void TryConnectToActor(UXRConnectorComponent* InConnector, AActor* InActor, const FString& InSocketID);


	/**
	* Maps EControllerHand to ESimplifiedControllerHand which effectively outputs "any" for all types but right and left.
	* @param InControllerHand The enum to be reduced to use any for all fields but Right and left
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Utilities")
	static ESimplifiedControllerHand SimplyfyControllerHandEnum(EControllerHand InControllerHand);
};
