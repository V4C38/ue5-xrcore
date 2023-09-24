
#pragma once

#include "CoreMinimal.h"
#include "XRHighlightComponent.h"
#include "XRInteractionComponent.h"
#include "XRInteractionHighlightComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(XRToolkit), meta=(BlueprintSpawnableComponent))
class XR_TOOLKIT_API UXRInteractionHighlightComponent : public UXRHighlightComponent
{
	GENERATED_BODY()

public:
	
	/**
	 * Assigns a XRInteraction to the Highlight. If the Interaction is started, the highlight will terminate
	 * even if the Interacting Pawn is colliding with the object. Normal behavior is resumed when the Assigned
	 * Interaction is ended.
	 * @param InAssignedInteractionComponent Component to assign. You can only assign one component per highlight. Consider adding a Second XRHighlight if needed.
	 */
	UFUNCTION(BlueprintCallable, Category="XRToolkit|XR Highlight Component")
	void AssignXRInteraction(UXRInteractionComponent* InAssignedInteractionComponent);
	
	/**
	 * Will unassign the linked XRInteractionComponent. 
	 */
	UFUNCTION(BlueprintCallable, Category="XRToolkit|XR Highlight Component")
	void UnassignXRInteraction();

	/**
	 * Can return a nullptr. 
	 */
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Highlight Component")
	UXRInteractionComponent* GetAssignedXRInteraction() const;

protected:
	void BindToInteractionHover();
	void UnbindFromInteractionHover();
	void BindToInteraction();
	void UnbindFromInteraction();

	virtual void SetActive(bool bNewActive, bool bReset) override;
	
private:
	UFUNCTION()
	void OnInteractionHovered(UXRInteractionComponent* Sender, UXRInteractorComponent* XRInteractor, bool bHovered);
	UFUNCTION()
	void OnInteractionStarted(UXRInteractionComponent* Sender, UXRInteractorComponent* XRInteractorComponent);
	UFUNCTION()
	void OnInteractionEnded(UXRInteractionComponent* Sender, UXRInteractorComponent* XRInteractorComponent);
	UFUNCTION()
	void OnInteractionComponentActivated(UActorComponent* Component, bool Reset);
	UFUNCTION()
	void OnInteractionComponentDeactivated(UActorComponent* Component);
	
	UPROPERTY()
	UXRInteractionComponent* AssignedInteractionComponent = nullptr;
};
