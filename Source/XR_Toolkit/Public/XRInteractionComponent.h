
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundBase.h"
#include "XRInteractionComponent.generated.h"


class UXRInteractorComponent;
class UXRInteractionComponent;
class UXRInteractionHighlightComponent;

UENUM(BlueprintType)
enum class EXRInteractionPriority : uint8
{
	Primary UMETA(DisplayName = "Primary"),
	Secondary UMETA(DisplayName = "Secondary"),
	Custom UMETA(DisplayName = "Custom"),
};

UENUM(BlueprintType)
enum class EXRLaserBehavior : uint8
{
	Disabled UMETA(DisplayName = "Disabled"),
	Supress UMETA(DisplayName = "Supress while Interacting"),
	Enabled UMETA(DisplayName = "Enabled"),
	Snap UMETA(DisplayName = "Snap to Interaction Start"),
	SnapMove UMETA(DisplayName = "Snap to Interaction Start - allow movement"),
};

UENUM(BlueprintType)
enum class EXRMultiInteractorBehavior : uint8
{
	Enabled UMETA(DisplayName = "Allow multiple Interactors"),
	Disabled UMETA(DisplayName = "Only Single Interactor"),
	TakeOver UMETA(DisplayName = "Take over from current Interactor"),
};

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
	 * Returns true if this interaction is currently interacted with by one or more XRInteractors. 
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Interaction")
	bool IsInteractedWith() const;

	/**
	 * Returns true if this interaction is currently interacted with by one or more XRInteractors.
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Interaction")
	bool IsHovered(TArray<UXRInteractorComponent*>& OutHoveringInteractors);

	/**
	 * Return associated XRInteractorComponent. Can be nullptr. 
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Interaction")
	TArray<UXRInteractorComponent*> GetActiveInteractors() const;

	/**
	* Return what happens when this interaction is active and a second XRInteractor starts interacting.
	* Allow: Allow multiple Interactors
	* Single: Ignore secondary Interactors
	* TakeOver: Take over from current Interactor
	*/
	UFUNCTION(BlueprintPure, Category = "XRCore|Interaction")
	EXRMultiInteractorBehavior GetMultiInteractorBehavior() const;

	/**
	 * Return the Priority value for this XRInteractionComponent.
	 */
	UFUNCTION(BlueprintPure, Category="XRCore|Interaction")
	int32 GetInteractionPriority();

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
	 * Determines how this interaction is addressed by the XRInteractor. 
	 * Generally, Interactions are started/stopped by priority values that correspond to Input mappings like Primary or Secondary Input.
	 * Custom allows to specify this as an integer value for custom implementations of start/stop.
	 */
	UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|General")
	EXRInteractionPriority InteractionPriority = EXRInteractionPriority::Primary;

	/**
	 * Lower Value gives higher Priority. Prioritized Interactions will be started first.
	 * This allows stacking multiple Interactions on one Actor and starting / stopping them independently from each other.
	 */
	UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|General", meta = (ClampMin = "0"))
	int32 AbsolouteInteractionPriority = 1;
	UFUNCTION()
	void UpdateAbsolouteInteractionPriority();

	/**
	* Determine what happens when this interaction is active and a second XRInteractor tries to start interacting. 
	* Allow: Allow multiple Interactors
	* Single: Ignore secondary Interactors
	* TakeOver: Take over from current Interactor
	*/
	UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|General")
	EXRMultiInteractorBehavior MultiInteractorBehavior = EXRMultiInteractorBehavior::TakeOver;

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
	 * Enable Material based Highlighting for this Interaction.
	 * Spawn an XRInteractionHighlight on BeginPlay and set this interaction as it`s assigned Interaction if true at BeginPlay.
	 */
	UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|Highlighting")
	bool bEnableHighlighting = true;

	/**
	 * Tag used to determine which MeshComponents to ignore for Highlighting - 
	 * By default all UMeshComponents will be highlighted if a compatible material is assigned.
	 */
	UPROPERTY(EditAnywhere, Category = "XRCore|Interaction|Highlighting")
	FName HighlightIgnoreMeshTag = "XRHighlight_Ignore";

	/**
	 * Fade the highlight based on this curve. If no curve is provided, HighlightState will be applied immediately.
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
	TArray<TWeakObjectPtr<UXRInteractorComponent>> ActiveInteractors = {};

	UPROPERTY()
	TArray<TWeakObjectPtr<UXRInteractorComponent>> HoveringInteractors = {};
};
