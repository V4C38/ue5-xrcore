#pragma once

#include "CoreMinimal.h"

#include "XRInteractionTypes.generated.h"

// ================================================================================================================================================================
// Interaction System types and interfaces
// ================================================================================================================================================================

namespace XRInteraction
{
	constexpr int32 PrimaryPriority = 1;
	constexpr int32 SecondaryPriority = 2;
	constexpr int32 LowestPriority = 5;
}

UENUM(BlueprintType)
enum class EXRInteractionPriority : uint8
{
	Primary UMETA(DisplayName = "Primary"),
	Secondary UMETA(DisplayName = "Secondary"),
	Custom UMETA(DisplayName = "Custom"),
};

UENUM(BlueprintType)
enum class EXRInteractionPrioritySelection : uint8
{
	Equal UMETA(DisplayName = "Exactly this priority"),
	HigherEqual UMETA(DisplayName = "This priority, then higher"),
	LowerEqual UMETA(DisplayName = "This priority, then lower"),
};

UENUM(BlueprintType)
enum class EXRLaserBehavior : uint8
{
	Disabled UMETA(DisplayName = "Disabled"),
	Suppress UMETA(DisplayName = "Suppress while Interacting"),
	Snap UMETA(DisplayName = "Snap to Interaction"),
};

UENUM(BlueprintType)
enum class EXRMultiInteractorBehavior : uint8
{
	Enabled UMETA(DisplayName = "Allow multiple Interactors"),
	Disabled UMETA(DisplayName = "Only Single Interactor"),
	TakeOver UMETA(DisplayName = "Take over from current Interactor"),
};


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Interaction Interface: implemented by XRInteractor, XRLaser for starting and stopping interactions from user input
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, BlueprintType)
class UXRInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class IXRInteractionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRInteraction")
	void StartInteraction(UXRInteractorComponent* InInteractor, UXRInteractionComponent* InInteraction);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRInteraction")
	void StartInteractionByPriority(int32 InPriority = 1, EXRInteractionPrioritySelection InPrioritySelectionCondition = EXRInteractionPrioritySelection::LowerEqual);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRInteraction")
	void StopInteraction(UXRInteractorComponent* InInteractor, UXRInteractionComponent* InInteraction);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRInteraction")
	void StopInteractionByPriority(int32 InPriority = 5, EXRInteractionPrioritySelection InPrioritySelectionCondition = EXRInteractionPrioritySelection::HigherEqual);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRInteraction")
	void StopAllInteractions(UXRInteractorComponent* InInteractor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "XRCore|XRInteraction")
	void HoverInteraction(UXRInteractorComponent* InInteractor, UXRInteractionComponent* InInteraction, bool InHoverState);
};