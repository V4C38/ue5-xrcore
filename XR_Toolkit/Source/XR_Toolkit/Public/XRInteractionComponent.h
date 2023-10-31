
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundBase.h"
#include "XRInteractionComponent.generated.h"


class UXRInteractorComponent;
class UXRInteractionComponent;
class UXRInteractionHighlightComponent;



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionStarted, UXRInteractionComponent*, Sender, UXRInteractorComponent*, XRInteractorComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionEnded, UXRInteractionComponent*, Sender, UXRInteractorComponent*, XRInteractorComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionHovered, UXRInteractionComponent*, Sender, UXRInteractorComponent*, HoveringXRInteractor, bool, bHovered);


UCLASS(Blueprintable, ClassGroup=(XRToolkit), meta=(BlueprintSpawnableComponent) )
class XR_TOOLKIT_API UXRInteractionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UXRInteractionComponent();
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "XRToolkit|XR Interaction|Delegates")
	FOnInteractionHovered OnInteractionHovered;

	/**
	 * Will manually call the OnInteractionStart Event which should be overriden by the inheriting class with the interaction behavior.
	 * Can also be called only on one Client for Local-only interactions as well. (Default is via Multicast from XRInteractionSystemComponent)
	 * NOTE: Intended to be automatically called from XRInteractionSystem. 
	 * @param InInteractor Optional. 
	 */
	UFUNCTION(BlueprintCallable, Category="XRToolkit|XR Interaction")
	virtual void StartInteraction(UXRInteractorComponent* InInteractor);
	
	UPROPERTY(BlueprintAssignable, Category="XRToolkit|XR Interaction|Delegates")
	FOnInteractionStarted OnInteractionStarted;

	
	/**
	 * Will manually call the OnInteractionStop Event which should be overriden by the inheriting class with the interaction behavior.
	 * Can also be called only on one Client for Local-only interactions as well. (Default is via Multicast from XRInteractionSystemComponent)
	 * NOTE: Intended to be automatically called from XRInteractionSystem. 
	 *  @param InInteractor Optional. 
	 */
	UFUNCTION(BlueprintCallable, Category="XRToolkit|XR Interaction")
	virtual void EndInteraction(UXRInteractorComponent* InInteractor);
	
	UPROPERTY(BlueprintAssignable, Category="XRToolkit|XR Interaction|Delegates")
	FOnInteractionEnded OnInteractionEnded;

	
	UFUNCTION(BlueprintCallable, Category="XRToolkit|XR Interaction")
	virtual void HoverInteraction(UXRInteractorComponent* InInteractor, bool bInHoverState);


	// ------------------------------------------------------------------------------------------------------------------------------------------------------------

	/**
	 * Returns true if this is a continuous interaction and it is currently ongoing. 
	 */
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction")
	bool GetIsInteractionActive() const;
	
	/**
	 * Return associated XRInteractorComponent. Can be nullptr. 
	 */
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction")
	UXRInteractorComponent* GetActiveInteractor();

	/**
	 * Sets the Active Interactor if this is a ContinuousInteraction. (Replicated)
	 * @param InInteractor Set to nullptr if you are unassigning an Interactor. 
	 */
	UFUNCTION(BlueprintCallable, Category="XRToolkit|XR Interaction")
	void SetActiveInteractor(UXRInteractorComponent* InInteractor);

	/**
	* Return an XRInteractorComponent that is overlapping with the Interaction Collision. Can be nullptr.
	*/
	UFUNCTION(BlueprintPure, Category = "XRToolkit|XR Interaction")
	UXRInteractorComponent* GetHoveringInteractor();

	/**
	* Caches all UMeshComponents on the owner. Those are considered for Overlaps and call OnInteractionHovered.
	* Initially Called on BeginPlay already - can be used for Override or if Components are added at runtime.
	*/
	UFUNCTION(BlueprintCallable, Category = "XRToolkit|XR Interaction|Config")
	void CacheInteractionCollision();

	/**
	* Return the cached UMeshComponents utiized for Interaction Hovering on the Owning Actor.
	*/
	UFUNCTION(BlueprintPure, Category = "XRToolkit|XR Interaction|Config")
	TArray<UMeshComponent*> GetInteractionCollision() const;

	/**
	 * Return the Priority value for this XRInteractionComponent.
	 */
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction")
	int32 GetInteractionPriority() const;
	
	/**
	* Can this Continuous Interaction be taken over by another XRInteractor while it is active?
	* Example: GrabInteraction - an XRInteractor starts grabing even though the grab is already active on another. 
	 */
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction")
	bool GetAllowTakeOver() const;

	/**
	* Can this Continuous Interaction be taken over by another XRInteractor while it is active?
	* Example: GrabInteraction - an XRInteractor starts grabing even though the grab is already active on another. 
	 */
	UFUNCTION(BlueprintCallable, Category="XRToolkit|XR Interaction")
	void SetAllowTakeOver(bool bInAllowTakeOver);
	
	/**
	 * Is the interaction finished instantly or must it be ended manually.  
	*/
	UFUNCTION(BlueprintPure, Category="XRToolkit|XR Interaction|Config")
	bool GetIsContinuousInteraction() const;
	
	/**
	 * Enables / Disables this Interaction to be triggered via the LaserComponent. Will also disable Highlighting via the Laser. 
	 * The Actor will be ignored during the collision checks in the InteractionSystemComponent. 
	 */
	UFUNCTION(Blueprintpure, Category="XRToolkit|XR Interaction|Laser")
	bool IsLaserInteractionEnabled() const;

	/**
	 * Should this XRInteraction disable the XRLaser while the Interaction is active.
	 */
	UFUNCTION(BlueprintPure, Category = "XRToolkit|XR Interaction|Laser")
	bool GetSupressLaserWhenInteracting() const;
	/**
	 * Set if this XRInteraction disables the XRLaser while the Interaction is active.
	 */
	UFUNCTION(BlueprintCallable, Category = "XRToolkit|XR Interaction|Laser")
	void SetSupressLaserWhenInteracting(bool InSupressLaser);

	/**
	 * Should this XRInteraction force the XRLaser to snap to the owning Actor.
	 */
	UFUNCTION(BlueprintPure, Category = "XRToolkit|XR Interaction|Laser")
	bool GetSnapXRLaserToActor() const;
	/**
	 * Set if this XRInteraction forces the XRLaser to snap to the owning Actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "XRToolkit|XR Interaction|Laser")
	void SetSnapXRLaserToActor(bool InSnapXRLaserToActor);

	/**
	 * Return the assigned XRInteractionHighlightComponent. Only valid if bEnableHighlighting is true on BeginPlay.
	 */
	UFUNCTION(BlueprintPure, Category = "XRToolkit|XR Interaction|Highlight")
	UXRHighlightComponent* GetXRHighlightComponent();

protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

	UPROPERTY()
	bool bIsInteractionActive = false;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------

	/**
	 * Override in Child Blueprint classes to implement the interaction behavior
	 * @param InInteractor Optional. Can be nullptr.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="XRToolkit|XR Interaction")
	void OnInteractionStart(UXRInteractorComponent* InInteractor);
	/**
	 * Override in Child Blueprint classes to implement the interaction behavior
	 * @param InInteractor Optional. Can be nullptr.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="XRToolkit|XR Interaction")
	void OnInteractionEnd(UXRInteractorComponent* InInteractor);
	
	/**
	 * Override in Child Blueprint classes.
	 * Called on Interaction Hover Start and End. 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="XRToolkit|XR Interaction")
	void OnInteractionHover(bool bHovering, UXRInteractorComponent* HoveringXRInteractor);

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------

	/**
	 * Interactions with a higher priority will be started first. Value must be >= 0.
	 * This allows stacking multiple Interactions on one Actor and starting / stopping them independently from each other.
	 * iE. Grab (Prio 2) + Use (Prio 1) will allow grabing the Actor and using it while grabed without having to terminate
	 * the Grab Interaction.
	 */
	UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Config")
	int32 InteractionPriority = 0;

	/**
	 * Determines if the interaction is finished instantly or must be ended manually.
	 * Enables In-Progress Late-Joining.
	 */
	UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Config")
	bool bIsContinuousInteraction = true;

	/**
	* Can this Continuous Interaction be taken over by another XRInteractor while it is active?
	* Example: GrabInteraction - an XRInteractor starts grabing even though the grab is already active on another.
	*/
	UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Config")
	bool bAllowTakeOver = true;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Enables / Disables this Interaction to be triggered via the LaserComponent. Will also disable Highlighting via the Laser.
	 * The Actor will be ignored during the collision checks in the InteractionSystemComponent.
	 */
	UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Laser")
	bool bEnableLaserInteraction = true;

	/**
	 * Should this XRInteraction disable the XRLaser while the Interaction is active.
	 */
	UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Laser")
	bool bSupressLaserWhenInteracting = true;

	/**
	 * Should this XRInteraction force the XRLaser to snap to the OwningActor.
	 */
	UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Laser")
	bool bSnapXRLaserToActor = false;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Ebanble Material based Highlighting for this Interaction.
	 * Spawn an XRInteractionHighlight on BeginPlay and set this interaction as it`s assigned Interaction if true at BeginPlay.
	 */
	UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Highlighting")
	bool bEnableHighlighting = true;

	/**
	 * Tag used to determine which MeshComponents to cache on the Owning Actor for highlighting.
	 */
	UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Highlighting")
	FName HighlightIgnoreMeshTag = "XRHighlight_Ignore";

	/**
	 * Fade the highlight based on this curve. If no curve is provided, HighlightState will be set instantly.
	 */
	UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Highlighting")
	UCurveFloat* HighlightFadeCurve = nullptr;

	UPROPERTY()
	UXRHighlightComponent* XRHighlightComponent = nullptr;
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	* Played when the Interaction starts at the location of the XRInteractionComponent.
	 */
	UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Audio")
	USoundBase* InteractionStartSound= nullptr;
	/**
	* Played when the Interaction ends at the location of the XRInteractionComponent.
	 */
	UPROPERTY(EditAnywhere, Category = "XRToolkit|XR Interaction|Audio")
	USoundBase* InteractionEndSound = nullptr;

	UPROPERTY()
	UAudioComponent* CurrentAudioComponent = nullptr;

	UFUNCTION()
	void RequestAudioPlay(USoundBase* InSound);
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------

private:

	UFUNCTION()
	void SpawnAndConfigureXRHighlight();
	
	UPROPERTY()
	TArray<UMeshComponent*> InteractionCollision = {nullptr};
	
	UPROPERTY()
	UXRInteractorComponent* ActiveInteractor = nullptr;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
