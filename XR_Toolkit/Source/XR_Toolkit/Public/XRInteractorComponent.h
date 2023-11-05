
#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "InputCoreTypes.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "XRInteractorComponent.generated.h"

class UXRInteractionComponent;
class UXRInteractorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStartInteracting, UXRInteractorComponent*, Sender, UXRInteractionComponent*, XRInteractionComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStopInteracting, UXRInteractorComponent*, Sender, UXRInteractionComponent*, XRInteractionComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHoverStateChanged, UXRInteractorComponent*, Sender, UXRInteractionComponent*, HoveredXRInteractionComponent, bool, bHoverState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRequestStartXRInteraction, UXRInteractorComponent*, Sender, UXRInteractionComponent*, InteractionToStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRequestStopXRInteraction, UXRInteractorComponent*, Sender, UXRInteractionComponent*, InteractionToStop);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestStopAllXRInteractions, UXRInteractorComponent*, Sender);

UCLASS( ClassGroup=(XRToolkit), meta=(BlueprintSpawnableComponent) )
class XR_TOOLKIT_API UXRInteractorComponent : public USphereComponent
{
	GENERATED_BODY()

public:	
	UXRInteractorComponent();

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Interaction Events
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Send a request to the XRInteractionSystemComponent this Interactor is assigned to start interacting with the next available XRInteractionComponent. 
	 * Will interact with the most prioritized interaction on the closest found actor with an XRInteractionComponent, or the most prioritized interaction on the actor 
	 * that is already being interacted with (using multiple interactions on one Actor).
	 */
	UFUNCTION(BlueprintCallable, Category = "XR Interaction|XR Interactor")
	void RequestStartXRInteraction();
	UFUNCTION(Server, Reliable, Category = "XR Interaction|XR Interactor")
	void Server_StartInteracting(UXRInteractionComponent* InInteractionComponent);

	UPROPERTY()
	FOnRequestStartXRInteraction OnRequestStartXRInteraction;
	UPROPERTY(BlueprintAssignable, Category = "XR Interaction|XR Interactor|Delegates")
	FOnStartInteracting OnStartInteracting;

	/**
	 * Send a request to the XRInteractionSystemComponent this Interactor is assigned to stop interacting with the next available XRInteractionComponent. 
	 * Will stop interacting with the least prioritized interaction on the closest found actor with an XRInteractionComponent, or the least prioritized interaction on the actor 
	 * that is already being interacted with (using multiple interactions on one Actor). 
	 */
	UFUNCTION(BlueprintCallable, Category = "XR Interaction|XR Interactor")
	void RequestStopXRInteraction();
	/**
	 * Send a request to the XRInteractionSystemComponent this Interactor is assigned to stop all active interactions.
	 */
	UFUNCTION(BlueprintCallable, Category = "XR Interaction|XR Interactor")
	void RequestStopAllXRInteractions();
	UFUNCTION(Server, Reliable, Category = "XR Interaction|XR Interactor")
	void Server_StopInteracting(UXRInteractionComponent* InInteractionComponent);

	UPROPERTY()
	FOnRequestStopXRInteraction OnRequestStopXRInteraction;
	UPROPERTY()
	FOnRequestStopAllXRInteractions OnRequestStopAllXRInteractions;
	UPROPERTY(BlueprintAssignable, Category = "XR Interaction|XR Interactor|Delegates")
	FOnStopInteracting OnStopInteracting;
	
	UPROPERTY(BlueprintAssignable, Category = "XR Interaction|XR Interactor|Delegates")
	FOnHoverStateChanged OnHoverStateChanged;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Utility
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Returns true if a valid InteractionComponent is assigned. 
	 */
	UFUNCTION(BlueprintPure, Category="XR Interaction|XR Interactor")
	bool IsInteracting() const;
	/**
	 * Returns all active Interactions (iE. Continuous Interactions this Interactor is handling)
	 */
	UFUNCTION(BlueprintPure, Category="XR Interaction|XR Interactor")
	TArray<UXRInteractionComponent*> GetActiveInteractions() const; 

	/**
	 * Returns XRInteractionComponents on the closest overlapping Actor (that has XRInteractions) for this XRInteractor.
	 * Distance is calculated based on the Actors root, not the contained XRInteractionComponents location.
	 */
	UFUNCTION(BlueprintPure, Category="XR Interaction|XR Interactor")
	AActor* GetClosestXRInteractionActor() const;
	/**
	 * Returns XRInteractionComponents on the closest overlapping Actor (that has XRInteractions) for this XRInteractor.
	 * Distance is calculated based on the Actors root, not the contained XRInteractionComponents location.
	 */
	UFUNCTION(BlueprintPure, Category = "XR Interaction|XR Interactor")
	UXRInteractionComponent* GetPrioritizedXRInteraction(TArray<UXRInteractionComponent*> InInteractions, bool IgnoreActive=true, bool SortByLowest=true) const;


	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Set the HandType for this Interactor.
	*/
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "XR Interaction|XR Interactor")
	void Server_SetXRControllerHand(EControllerHand InXRControllerHand);
	/**
	 * Get the assigned HandType for this Interactor.
	*/
	UFUNCTION(BlueprintPure, Category = "XR Interaction|XR Interactor")
	EControllerHand GetXRControllerHand() const;

	/**
	 * Returns true if this Interactor is part of an XRLaser. This can be useful to distinguish between Interactions
	 * coming from Hands or Lasers to allow for different behavior. (For example SnapToHand-grab via Laser) 
	*/
	UFUNCTION(BlueprintPure, Category="XR Interaction|XR Interactor")
	bool IsLaserInteractor();

	/**
	 * Manually set the assochiated Pawn - this is useful for Actors that need a XRInteractor but are not an APawn like the XRLaser.
	 * Is done automatically at BeginPlay if this XRInteractor is owned by an APawn.
	*/
	UFUNCTION(BlueprintCallable, Category = "XR Interaction|XR Interactor")
	void SetOwningPawn(APawn* InOwningPawn);
	/**
	 * Return the assochiated Pawn. Useful for intermediaries that use XRInteractors like the XRLaser.
	*/
	UFUNCTION(BlueprintPure, Category = "XR Interaction|XR Interactor")
	APawn* GetOwningPawn() const;
	/**
	 * Is the Owner (Pawn) of this XRInteractor locally controlled?
	*/
	UFUNCTION(BlueprintPure, Category="XR Interaction|XR Interactor")
	bool IsLocallyControlled() const;


	/**
	 * Required Physics-based Interactions. Not spawned automatically as manual assignment gives greater flexibility in configuration of the PhysicsConstraint.
	 * @param InPhysicsConstraintComponent PhysicsConstraintComponent to assign to this XRInteractor.
	*/
	UFUNCTION(BlueprintCallable, Category = "XR Interaction|XR Interactor")
	void SetAssignedPhysicsConstraint(UPhysicsConstraintComponent* InPhysicsConstraintComponent);
	/**
	 * Return the associated PhysicsConstraint.
	 * NOTE: Must be assigned manually first. 
	*/
	UFUNCTION(BlueprintPure, Category="XR Interaction|XR Interactor")
	UPhysicsConstraintComponent* GetAssignedPhysicsConstraint() const;
	

	
protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, EditDefaultsOnly, Category="XR Interaction|XR Interactor")
	EControllerHand XRControllerHand = EControllerHand::AnyHand;
	
	UPROPERTY(EditDefaultsOnly, Category="XR Interaction|XR Interactor")
	bool bIsLaserInteractor = false;
	
	
private:
	UFUNCTION(NetMulticast, Reliable, Category = "XR Interaction|XR Interactor")
	void Multicast_StartedInteracting(UXRInteractionComponent* InteractionComponent);
	UFUNCTION(NetMulticast, Reliable, Category = "XR Interaction|XR Interactor")
	void Multicast_StoppedInteracting(UXRInteractionComponent* InteractionComponent);

	UPROPERTY()
	APawn* OwningPawn = nullptr;
	UPROPERTY()
	UPhysicsConstraintComponent* AssignedPhysicsConstraint;
	UPROPERTY()
	AActor* LocalInteractedActor = nullptr;
	UPROPERTY(Replicated)
	TArray<UXRInteractionComponent*> ActiveInteractionComponents = {};
	UPROPERTY()
	TArray<UXRInteractionComponent*> HoveredInteractionComponents = {};

	void CacheIsLocallyControlled();
	bool bIsLocallyControlled = false;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
