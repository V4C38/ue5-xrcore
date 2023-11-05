
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XRInteractionComponent.h"
#include "XRInteractorComponent.h"
#include "XRInteractionSystemComponent.generated.h"

class UXRInteractionSystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionStart, UXRInteractionSystemComponent*, Sender, UXRInteractionComponent*, XRInteractionComponent, UXRInteractorComponent*, XRInteractorComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionEnd, UXRInteractionSystemComponent*, Sender, UXRInteractionComponent*, XRInteractionComponent, UXRInteractorComponent*, XRInteractorComponent);



UCLASS( ClassGroup=(XRToolkit), meta=(BlueprintSpawnableComponent) )
class XR_TOOLKIT_API UXRInteractionSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UXRInteractionSystemComponent();
	virtual void BeginPlay() override;


	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Interaction Events
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Will start an Interaction (Replicated).
	 * If the InteractionComponent is already interacted with, the current interaction will be terminated first.
	 * This is intended for manual interaction handling.
	 * Will also give authority to the PlayerController that initiated the Interaction (iE. Interactor will Own the Interacted Actor)
	 * NOTE: Generally, RequestStartXRInteraction on the XRInteractorComponent is used to start interactions, but you can call this directly too for any kind of custom system.
	 * @param InInteractionComponent Interaction Component to start interaction on
	 * @param InXRInteractor Optional - provide if available. InteractionComponent can have an associated XRInteractor for specific interactions.
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="XR Interaction|System")
	void Server_StartInteraction(UXRInteractionComponent* InInteractionComponent, UXRInteractorComponent* InXRInteractor);

	UPROPERTY(BlueprintAssignable, Category="XR Interaction|System|Delegates")
	FOnInteractionStart OnInteractionStart; 
	/**
	 * Will stop an Interaction. (Replicated).
	 * If the InteractionComponent is not interacting, the command is discarded.
	 * This is intended for manual interaction handling. 
	 * NOTE: Generally, RequestStopXRInteraction on the XRInteractorComponent is used to stop interactions, but you can call this directly too for any kind of custom system.
	 * @param InInteractionComponent Interaction Component to stop any ongoing interaction on
	 * @param InXRInteractor Optional - provide if available. InteractionComponent can have an associated XRInteractor for specific interactions.
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="XR Interaction|System")
	void Server_StopInteraction(UXRInteractionComponent* InInteractionComponent, UXRInteractorComponent* InXRInteractor);

	UPROPERTY(BlueprintAssignable, Category="XR Interaction|System|Delegates")
	FOnInteractionEnd OnInteractionEnd;

	/**
	 * Manually assign an XRInteractors to this XRInteractionSystemComponent. 
	 * XRInteractors are assigned automatically by the InteractionSystem if both are owned by the same Actor (iE. Pawn).
	 */
	UFUNCTION(BlueprintCallable, Category = "XR Interaction|System")
	void RegisterXRInteractor(UXRInteractorComponent* InXRInteractor);
	/**
	 * Return all XRInteractors that are assigned to this XRInteractionSystemComponent.
	 */
	UFUNCTION(BlueprintPure, Category = "XR Interaction|System")
	TArray <UXRInteractorComponent*> GetRegisteredXRInteractors() const;
	/**
	 * Return all XRInteractionComponents that any assigned XRInteractor is currently interacting with (iE. active Continuous Interactions)
	 */
	UFUNCTION(BlueprintPure, Category = "XR Interaction|System")
	TArray <UXRInteractionComponent*> GetActiveXRInteractions() const;

private:
	UFUNCTION(NetMulticast, Reliable, Category="XR Interaction|System")
	void Multicast_StartInteraction(UXRInteractionComponent* InteractionComponent, UXRInteractorComponent* XRInteractor);
	UFUNCTION(NetMulticast, Reliable, Category="XR Interaction|System")
	void Multicast_StopInteraction(UXRInteractionComponent* InteractionComponent, UXRInteractorComponent* XRInteractor);

	UFUNCTION()
	void OnInteractorRequestStartXRInteraction(UXRInteractorComponent* Sender, UXRInteractionComponent* InteractionToStart);
	UFUNCTION()
	void OnInteractorRequestStopXRInteraction(UXRInteractorComponent* Sender, UXRInteractionComponent* InteractionToStop);
	UFUNCTION()
	void OnInteractorRequestStopAllXRInteractions(UXRInteractorComponent* Sender);

	
	/**
	 * Stores all Continuous XRInteractions. Use GetActiveInteractions to access. 
	 * Is only relevant on Server. (Clients could override it if necessary for some custom system that uses Local Interactions)
	 */
	UPROPERTY(Replicated)
	TArray<UXRInteractionComponent*> ActiveInteractions = {};

	UFUNCTION()
	void InitiallyRegisterXRInteractors();

	UPROPERTY()
	TArray < UXRInteractorComponent*> RegisteredXRInteractors = {};

};
