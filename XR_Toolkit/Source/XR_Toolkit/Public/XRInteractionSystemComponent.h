
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

	/**
	 * Will start an Interaction (Replicated).
	 * If the InteractionComponent is already interacted with, the current interaction will be terminated first.
	 * This is intended for manual interaction handling.
	 * This will also give authority to the PlayerController that initiated the Interaction (iE. Interactor will Own the Interacted Actor)
	 * NOTE: Generally, StartInteracting should be used as it automatically handles selecting the appropriate XRInteractions to start.
	 * @param InInteractionComponent Interaction Component to start interaction on
	 * @param InXRInteractor Optional - provide if available. InteractionComponent can have an associated XRInteractor for specific interactions.
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="XRToolkit|XR Interaction System")
	void Server_StartInteraction(UXRInteractionComponent* InInteractionComponent, UXRInteractorComponent* InXRInteractor);

	UPROPERTY(BlueprintAssignable, Category="XRToolkit|XR Interaction System|Delegates")
	FOnInteractionStart OnInteractionStart;

	/**
	 * The Interactor will be checked for overlapping actors and the XRInteractionSystem will determine which XRInteractionComponent
	 * has the highest InteractionPriority. Also supports TakeOver of Interactions on other XRInteractors.
	 * This will also give authority to the PlayerController that initiated the Interaction (iE. Interactor will Own the Interacted Actor)
	 * NOTE: Generally, this should be the default way of starting interactions. 
	 * @param InXRInteractor XRInteractor used to detect the overlapping XRInteractions. 
	 * @param InExclusivePriority Disabled when 0. Only Interactions with the specified Priority will be started. 
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="XRToolkit|XR Interaction System")
	void Server_StartInteracting(UXRInteractorComponent* InXRInteractor, int32 InExclusivePriority);
	
	
	/**
	 * Will stop an Interaction. (Replicated).
	 * If the InteractionComponent is not interacting, the command is discarded.
	 * This is intended for manual interaction handling. 
	 * NOTE: Generally, StopInteracting should be used as it automatically handles selecting the appropriate XRInteractions to stop.
	 * @param InInteractionComponent Interaction Component to stop any ongoing interaction on
	 * @param InXRInteractor Optional - provide if available. InteractionComponent can have an associated XRInteractor for specific interactions.
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="XRToolkit|XR Interaction System")
	void Server_StopInteraction(UXRInteractionComponent* InInteractionComponent, UXRInteractorComponent* InXRInteractor);

	UPROPERTY(BlueprintAssignable, Category="XRToolkit|XR Interaction System|Delegates")
	FOnInteractionEnd OnInteractionEnd;

	/**
	 * Checks the XRInteractor for Active XRInteractions and terminates all of them if no Priority Exlcusivity is given. 
	 * NOTE: Generally, this should be the default way of terminating interactions. 
	 * @param InXRInteractor Interactor to Terminate all Interactions on. 
	 * @param InExclusivePriority Disabled when 0. Only Interactions with the specified Priority will be stopped. 
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="XRToolkit|XR Interaction System")
	void Server_StopInteracting(UXRInteractorComponent* InXRInteractor, int32 InExclusivePriority);

	/**
	 * For all currently active XRInteractions set to IsContinuousInteraction, the interaction is manually restarted.
	 * Mainly used for In-Progress LateJoining 
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="XRToolkit|XR Interaction System")
	void Server_RestartAllActiveInteractions();
	/**
	 * For all currently active XRInteractions set to IsContinuousInteraction, the interaction is manually stopped.
	 * Can be useful for terminating interactions for disconnecting pawns etc.
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="XRToolkit|XR Interaction System")
	void Server_StopAllActiveInteractions();
	
	/**
	 * Replicated. Return all XRInteractionComponents that this XRInteractionSystem is interacting with. 
	 * Components must be set to bIsContinuousInteraction. 
	 */
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction System")
	TArray<UXRInteractionComponent*> GetActiveInteractions() const;

	/**
	 * Returns the highest priority XRInteraction from an array of XRInteractions. 
	 * Found XRInteractions can be currently active if bAllowTakeOver is enabled - XRInteractor must be provided for this. 
	 * @param InXRInteractions Array of Interactions to find the highest Priority one in.
	 * @param InXRInteractor Optional - provide if available. Required for finding ActiveInteractions. 
	 */
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction System")
	UXRInteractionComponent* GetPrioritizedXRInteraction(TArray<UXRInteractionComponent*> InXRInteractions, UXRInteractorComponent* InXRInteractor) const;
	
private:
	/**
	 * Should be called from Server_StartInteraction only. 
	 */
	UFUNCTION(NetMulticast, Reliable, Category="XRToolkit|XR Interaction System")
	void Multicast_StartInteraction(UXRInteractionComponent* InteractionComponent, UXRInteractorComponent* XRInteractor);
	
	/**
	 * Should be called from Server_StopInteraction only. 
	 */
	UFUNCTION(NetMulticast, Reliable, Category="XRToolkit|XR Interaction System")
	void Multicast_StopInteraction(UXRInteractionComponent* InteractionComponent, UXRInteractorComponent* XRInteractor);


	
	/**
	 * Stores all Continuous XRInteractions. Use GetActiveInteractions to access. 
	 * Is only relevant on Server. (Clients could override it if necessary for some custom system that uses Local Interactions)
	 */
	UPROPERTY(Replicated)
	TArray<UXRInteractionComponent*> ActiveInteractions = {};

	UFUNCTION()
	void OnRequestStopXRInteraction(UXRInteractorComponent* Sender);
};
