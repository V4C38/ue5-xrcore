
#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Components/ActorComponent.h"
#include "Components/MeshComponent.h"

#include "XRHighlightComponent.generated.h"

// ================================================================================================================================================================
// Material based highlighting on assigned UMeshComponents
// ================================================================================================================================================================

UCLASS(Blueprintable, ClassGroup=(XRToolkit), meta=(BlueprintSpawnableComponent) )
class XRCORE_API UXRHighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UXRHighlightComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------

	/**
	 * Sets all cached HighlightMeshComponents to the desired state.
	 * NOTE: Material based approach, Material must have "HighlightState" MaterialParameter.
	 * @param InHighlightState <= 0 off, >= 0 on, > 1 hovered (special state based on Material Setup)
	 */
	UFUNCTION(BlueprintCallable, Category = "XRCore|Highlight")
	void SetHighlighted(float InHighlightState);

	/**
	 * 0 = off, 1 = on; if curve drives state, range is [0,1]
	 */
	UFUNCTION(BlueprintPure, Category = "XRCore|Highlight")
	float GetHighlightState() const;

	/**
	 * Using SetHighlightState() to Fade In or Out the Highlight via a Timeline controlled by the HighlightFadeCurve.
	 * HighlightState will be applied instantly if no HighlightFadeCurve is provided.
	 * @param bFadeIn Switch between FadeIn / FadeOut.
	 */
	UFUNCTION(BlueprintCallable, Category = "XRCore|Highlight")
	void FadeXRHighlight(bool bFadeIn);

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------

	/**
	 * Tag used to determine which MeshComponents to cache on the Owning Actor.
	 * Specifying no Tag will cache all UMesh components. 
	 */
	UPROPERTY(EditAnywhere, Category = "XRCore|Highlight")
	TArray<FName> HighlightIncludeOnlyTags = {};

	/**
	* Caches all UMeshComponents with the given Tag on the Owning Actor.
	* Specifying no Tag will cache all UMesh components. 
	* @param InHighlightMeshTag All UMeshComponents with this tag will be cached for highlighting. 
	*/
	UFUNCTION(BlueprintCallable, Category="XRCore|Highlight")
	void SetHighlightIncludeOnlyTags(TArray<FName> InHighlightIncludeOnlyTags);

	/**
	* Returns the currently assigned HighlightTags.
	* If None are returned, all UMeshComponents are cached.
	*/
	UFUNCTION(BlueprintPure, Category="XRCore|Highlight")
	TArray<FName> GetHighlightIncludeOnlyTags() const;

	/**
	* Returns the currently cached UMeshComponents that are considered for Highlighting. 
	*/
	UFUNCTION(BlueprintPure, Category="XRCore|Highlight")
	TArray<UMeshComponent*> GetHighlightMeshes() const;

	/**
	 * Required to drive the Fade behavior. If none is set, HighlightState will be set instantly, even if FadeXRHighlight() is called.
	 */
	UPROPERTY(EditAnywhere, Category="XRCore|Highlight")
	UCurveFloat* HighlightFadeCurve = nullptr;
	/**
	 * Set the Curve to be used for Highlight fading. Will initialize a timeline and configre the fading functionality.
	 */
	UFUNCTION(BlueprintCallable, Category = "XRCore|Highlight")
	void SetHighlightFadeCurve(UCurveFloat* InHighlightFadeCurve);

protected:
	UPROPERTY()
	FName HighlightMaterialParameter = "XRHighlight_State";

	UPROPERTY()
	FTimeline FadeTimeline;

	UFUNCTION()
	void TimelineUpdate(float InTimelineProgress);
	UFUNCTION()
	void TimelineFinished();

	UPROPERTY()
	float HighlightState = 0.0f;
	
	virtual void SetActive(bool bNewActive, bool bReset) override;
	UFUNCTION()
	void InitializeFadeTimeline();

	
private:
	UPROPERTY()
	TArray<UMeshComponent*> HighlightableMeshComponents;
	UFUNCTION()
	void CacheHighlightableMeshComponents();
	bool bIsTimelineInitialized = false;
};
