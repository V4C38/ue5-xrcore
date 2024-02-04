
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundBase.h"
#include "XRInteractionComponent.generated.h"


class UXRInteractorComponent;
class UXRInteractionComponent;
class UXRInteractionHighlightComponent;

UENUM(BlueprintType)
enum class EXRLaserBehavior : uint8
{
	Disabled UMETA(DisplayName = "Disabled"),
	Supress UMETA(DisplayName = "Supress while Interacting"),
	Enabled UMETA(DisplayName = "Enabled"),
	Snap UMETA(DisplayName = "Snap to Interaction Start"),
	SnapMove UMETA(DisplayName = "Snap to Interaction Start - allow movement"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionStarted, UXRInteractionComponent*, Sender, UXRInteractorComponent*, XRInteractorComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionEnded, UXRInteractionComponent*, Sender, UXRInteractorComponent*, XRInteractorComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionHovered, UXRInteractionComponent*, Sender, UXRInteractorComponent*, HoveringXRInteractor, bool, bHovered);

UCLASS(Blueprintable, ClassGroup=(XRCore), meta=(BlueprintSpawnableComponent) )
class XRCORE_API UXRInteractionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UXRInteractionComponent();


	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Interaction Events
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Will manually call the OnInteractionStart Event which should be overriden by the inheriting class with specific interaction behavior.
	 * NOTE: Not intended to be called manually - invoked by XRInteractor.
	 * @param InInteractor Optional. 
	 */
	UFUNCTION(BlueprintCallable, Category="XRCore|Interaction")
	virtual void StartInteraction(UXRInteractorComponent* InInteractor);

	UPROPERTY(BlueprintAssignable, Category="XRCore|Interaction|Delegates")
	FOnInteractionStarted OnInteractionStarted;

	
	/**
	 * Will manually call the OnInteractionStop Event which should be overriden by the inheriting class with specific interaction behavior.
	 * NOTE: Not intended to be called manually - invoked by XRInteractor.
	 *  @param InInteractor Optional. 
	 */
	UFUNCTION(BlueprintCallable, Category="XRCore|Interaction")
	virtual void EndInteraction(UXRInteractorComponent* InInteractor);
	
	UPROPERTY(BlueprintAssignable, Category="XRCore|Interaction|Delegates")
	FOnInteractionEnded OnInteractionEnded;


	/**
	 * Will manually call the OnInteractionHovered Event which should be overriden by the inheriting class with specific interaction behavior.
	 * NOTE: Not intended to be called manually - invoked by XRInteractor.
	 *  @param InInteractor Optional.
	 *  @param bInHoverState Set Hover state to true or false - corresponds to 0.0f or 1.0f in the HighlightState Material Parameter.
	 */
	UFUNCTION(BlueprintCallable, Category="XRCore|Interaction")
	virtual void HoverInteraction(UXRInteractorComponent* InInteractor, bool bInHoverState);

	UPROPERTY(BlueprintAssignable, Category = "XRCore|Interaction|Delegates")
	FOnInteractionHovered OnInteractionHovered;


	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Utility
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Returns true if this interaction is currently ongoing. 
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Interaction")
	bool IsInteractionActive() const;

	/**
	 * Sets the Active Interactor. (Replicated)
	 * @param InInteractor Set to nullptr if you are unassigning an Interactor.
	 */
	UFUNCTION(BlueprintCallable, Category = "XRCore|Interaction")
	void SetActiveInteractor(UXRInteractorComponent* InInteractor);
	/**
	 * Return associated XRInteractorComponent. Can be nullptr. 
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Interaction")
	UXRInteractorComponent* GetActiveInteractor();

	/**
	 * Return the Priority value for this XRInteractionComponent.
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Interaction")
	int32 GetInteractionPriority() const;

	/**
	* Can this Interaction be taken over by another XRInteractor while it is active?
	* Example: GrabInteraction - an XRInteractor starts grabing even though the grab is already active on another.
	 */
	UFUNCTION(BlueprintCallable, Category = "XRCore|Interaction")
	void SetAllowTakeOver(bool bInAllowTakeOver);
	/**
	* Can this Interaction be taken over by another XRInteractor while it is active?
	* Example: GrabInteraction - an XRInteractor starts grabing even though the grab is already active on another. 
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Interaction")
	bool GetAllowTakeOver() const;

	/**
	 * Return the assigned XRInteractionHighlightComponent. Only valid if bEnableHighlighting is true on BeginPlay.
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Interaction|Highlight")
	UXRHighlightComponent* GetXRHighlightComponent();

	/**
	 * Return the assigned XRLaser behavior.
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Interaction|Laser")
	EXRLaserBehavior GetLaserBehavior() const;

	/**
	 * Set the XRLaser behavior.
	 */
	UFUNCTION(BlueprintCallable, Category = "XRCore|Interaction|Laser")
	void SetLaserBehavior(EXRLaserBehavior InLaserBehavior);

	/**
	 * Enables / Disables this Interaction to be triggered via the LaserComponent. Will also disable Highlighting via the Laser. 
	 * The Actor will be ignored during the collision checks in the InteractionSystemComponent. 
	 */
	UFUNCTION(Blueprintpure, Category="XRCore|Interaction|Laser")
	bool IsLaserInteractionEnabled() const;


protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

	UPROPERTY()
	bool bIsInteractionActive = false;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Abstract Interaction Events 
	// Override in derived classes to implement Interaction specific logic.
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Override in Child Blueprint classes to implement the interaction behavior
	 * @param InInteractor Optional. Can be nullptr.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="XRCore|Interaction")
	void OnInteractionStart(UXRInteractorComponent* InInteractor);
	/**
	 * Override in Child Blueprint classes to implement the interaction behavior
	 * @param InInteractor Optional. Can be nullptr.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="XRCore|Interaction")
	void OnInteractionEnd(UXRInteractorComponent* InInteractor);
	
	/**
	 * Override in Child Blueprint classes.
	 * Called on Interaction Hover Start and End. 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="XRCore|Interaction")
	void OnInteractionHover(bool bHovering, UXRInteractorComponent* HoveringXRInteractor);

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config - General
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Lower Prio is better. Prioritized Interactions will be started first.
	 * This allows stacking multiple Interactions on one Actor and starting / stopping them independently from each other.
	 */
	UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|General")
	int32 InteractionPriority = 1;

	/**
	* Can this Interaction be taken over by another XRInteractor while it is active?
	* Example: GrabInteraction - an XRInteractor starts grabing even though the grab is already active on another.
	*/
	UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|General")
	bool bAllowTakeOver = true;

	/**
	* Disabled - No Laser Interaction.
	* Supress while Interacting - Will disable the Laser upon Interaction Start.
	* Enabled - Laser Interaction enabled.
	* Snap to Interaction Start - Will snap the Laser tip to the position on the interacted actor when the interaction starts.
	* Snap to Interaction Start - allow movement - Same as normal, but will also allow the Laser Tip to move slightly.
	*/
	UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|General")
	EXRLaserBehavior LaserBehavior = EXRLaserBehavior::Enabled;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config - Highlighting
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	 * Ebanble Material based Highlighting for this Interaction.
	 * Spawn an XRInteractionHighlight on BeginPlay and set this interaction as it`s assigned Interaction if true at BeginPlay.
	 */
	UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|Highlighting")
	bool bEnableHighlighting = true;

	/**
	 * Tag used to determine which MeshComponents to ignore for Highlighting.
	 */
	UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|Highlighting")
	FName HighlightIgnoreMeshTag = "XRHighlight_Ignore";

	/**
	 * Fade the highlight based on this curve. If no curve is provided, HighlightState will be set instantly.
	 */
	UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|Highlighting")
	UCurveFloat* HighlightFadeCurve = nullptr;

	UPROPERTY()
	UXRHighlightComponent* XRHighlightComponent = nullptr;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config - Audio
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/**
	* Played when the Interaction starts at the location of the XRInteractionComponent.
	 */
	UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|Audio")
	USoundBase* InteractionStartSound= nullptr;
	/**
	* Played when the Interaction ends at the location of the XRInteractionComponent.
	 */
	UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|Audio")
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
	TWeakObjectPtr<UXRInteractorComponent> ActiveInteractor;

	UPROPERTY()
	TArray<TWeakObjectPtr<UXRInteractorComponent>> HoveringInteractors = {};
};
