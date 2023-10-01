
#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "EXRHandType.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "XRInteractorComponent.generated.h"


class UXRInteractionComponent;
class UXRInteractorComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStartInteracting, UXRInteractorComponent*, Sender, UXRInteractionComponent*, XRInteractionComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStopInteracting, UXRInteractorComponent*, Sender, UXRInteractionComponent*, XRInteractionComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestStopXRInteraction, UXRInteractorComponent*, Sender);

UCLASS( ClassGroup=(XRToolkit), meta=(BlueprintSpawnableComponent) )
class XR_TOOLKIT_API UXRInteractorComponent : public USphereComponent
{
	GENERATED_BODY()

public:	
	UXRInteractorComponent();
	
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	EXRHandType GetHandType() const;

	/**
	 * Sets which Hand side this interactor is representing.
	 * @param InHandType Which Hand Side
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	void Server_RequestStopXRInteraction();
	
	UPROPERTY(BlueprintAssignable, Category="XRToolkit|XR Interaction System|XR Interactor Component|Delegates")
	FOnRequestStopXRInteraction OnRequestStopXRInteraction;
	
	/**
	 * Sets which Hand side this interactor is representing.
	 * @param InHandType Which Hand Side
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	void Server_SetHandType(EXRHandType InHandType);
	
	/**
	 * Returns true if a valid InteractionComponent is assigned. 
	 */
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	bool IsInteractionActive() const;

	/**
	 * Returns all active Interactions (iE. Continuous Interactions this Interactor is handling)
	 */
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	TArray<UXRInteractionComponent*> GetActiveInteractionComponents() const; 

	/**
	 * Returns XRInteractionComponents on the closest overlapping Actor (that has XRInteractions) for this XRInteractor.
	 * Distance is calculated based on the Actors root, not the contained XRInteractionComponents location.
	 */
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	TArray<UXRInteractionComponent*> GetGetClosestXRInteractions(int32 InExclusivePriority) const;

	
	/**
	 * Adds an active InteractionComponent (Continuous Interaction)
	*/
	UFUNCTION(Server, Reliable, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	void Server_AddActiveInteractionComponent(UXRInteractionComponent* InInteractionComponent);
	/**
	 * Removes an active InteractionComponent (Continuous Interaction)
	*/
	UFUNCTION(Server, Reliable, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	void Server_RemoveActiveInteractionComponent(UXRInteractionComponent* InInteractionComponent);
	
	UPROPERTY(BlueprintAssignable, Category="XRToolkit|XR Interaction System|XR Interactor Component|Delegates")
	FOnStartInteracting OnStartInteracting;

	UPROPERTY(BlueprintAssignable, Category="XRToolkit|XR Interaction System|XR Interactor Component|Delegates")
	FOnStopInteracting OnStopInteracting;

	
	/**
	 * Returns true if this Interactor is part of an XRLaser. This can be useful to distinguish between Interactions
	 * coming from Hands or Lasers to allow for different behavior. (For example SnapToHand-grab via Laser) 
	*/
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	bool IsLaserInteractor();

	
	/**
	 * Is the Owner (Pawn) of this XRInteractor locally controlled?
	*/
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	bool IsLocallyControlled() const;

	/**
	 * Return the assochiated Pawn. Useful for intermediaries that use XRInteractors like the XRLaser. 
	*/
	UFUNCTION(BlueprintPure, Category = "XRToolkit|XR Interaction System|XR Interactor Component")
	APawn* GetOwningPawn() const;

	/**
	 *  Set the assochiated Pawn. 
	*/
	UFUNCTION(BlueprintCallable, Category = "XRToolkit|XR Interaction System|XR Interactor Component")
	void SetOwningPawn(APawn* InOwningPawn);

	/**
	 * Return the associated PhysicsConstraint.
	 * NOTE: Must be assigned manually first. 
	*/
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	UPhysicsConstraintComponent* GetAssignedPhysicsConstraint() const;
	
	/**
	 * Will assign a PhysicsConstraint to be used for Physics-based Interactions.
	 * @param InPhysicsConstraintComponent PhysicsConstraintComponent to assign to this XRInteractor.
	*/
	UFUNCTION(BlueprintCallable, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	void SetAssignedPhysicsConstraint(UPhysicsConstraintComponent* InPhysicsConstraintComponent);

	
protected:
	
	UPROPERTY(Replicated, EditDefaultsOnly, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	EXRHandType HandType = EXRHandType::None;
	
	UPROPERTY(EditDefaultsOnly, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	bool bIsLaserInteractor = false;
	
	virtual void InitializeComponent() override;
	
private:

	UPROPERTY()
	UPhysicsConstraintComponent* AssignedPhysicsConstraint;
	
	UPROPERTY(Replicated)
	TArray<UXRInteractionComponent*> ActiveInteractionComponents = {};

	UPROPERTY()
	APawn* OwningPawn = nullptr;

	void CacheIsLocallyControlled();
	bool bIsLocallyControlled = false;

	
	
	UFUNCTION(NetMulticast, Reliable, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	void Multicast_StartedInteracting(UXRInteractionComponent* InteractionComponent);
	UFUNCTION(NetMulticast, Reliable, Category="XRToolkit|XR Interaction System|XR Interactor Component")
	void Multicast_StoppedInteracting(UXRInteractionComponent* InteractionComponent);
	
};
