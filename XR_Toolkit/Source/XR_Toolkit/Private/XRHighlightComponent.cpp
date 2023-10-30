
#include "XRHighlightComponent.h"

UXRHighlightComponent::UXRHighlightComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
}

void UXRHighlightComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeFadeTimeline();
	SetHighlightTag(HighlightMeshTag);
	SetHighlightFadeCurve(HighlightFadeCurve);
}

void UXRHighlightComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FadeTimeline.TickTimeline(DeltaTime);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------

void UXRHighlightComponent::SetHighlighted(float InHighlightState)
{
	HighlightState = InHighlightState;

	for (auto* HighlightMeshComponent : HighlightableMeshComponents)
	{
		if (HighlightMeshComponent)
		{
			HighlightMeshComponent->SetScalarParameterValueOnMaterials(HighlightMaterialParameter, InHighlightState);
		}
	}
}

void UXRHighlightComponent::FadeXRHighlight(bool bFadeIn)
{
	if (!IsActive())
	{
		SetHighlighted(0.0f);
		return;
	}
	if (HighlightFadeCurve)
	{
		if (!bIsTimelineInitialized)
		{
			InitializeFadeTimeline();
		}
		SetComponentTickEnabled(true);
		if (bFadeIn)
			{
			FadeTimeline.Play();
		}
		else
		{
			FadeTimeline.Reverse();
		}
	}
	else
	{
		if (bFadeIn)
		{
			SetHighlighted(1.0f);
		}
		else
		{
			SetHighlighted(0.0f);
		}
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------

void UXRHighlightComponent::TimelineUpdate(float InTimelineProgress)
{
	SetHighlighted(InTimelineProgress);
}

void UXRHighlightComponent::TimelineFinished()
{
	SetComponentTickEnabled(false);
}

void UXRHighlightComponent::SetHighlightFadeCurve(UCurveFloat* InHighlightFadeCurve)
{
	HighlightFadeCurve = InHighlightFadeCurve;
	if (HighlightFadeCurve)
	{
		FOnTimelineFloat TimelineUpdate;
		TimelineUpdate.BindUFunction(this, FName("TimelineUpdate"));

		FOnTimelineEventStatic TimelineFinished;
		FadeTimeline.SetTimelineFinishedFunc(TimelineFinished);
		FadeTimeline.SetLooping(false);
		FadeTimeline.AddInterpFloat(HighlightFadeCurve, TimelineUpdate);
	}
}

void UXRHighlightComponent::InitializeFadeTimeline()
{
	if (HighlightFadeCurve && !bIsTimelineInitialized)
	{
		FOnTimelineFloat TimelineUpdate;
		TimelineUpdate.BindUFunction(this, FName("TimelineUpdate"));

		FOnTimelineEventStatic TimelineFinished;
		TimelineFinished.BindUFunction(this, FName("TimelineFinished"));

		FadeTimeline.SetTimelineFinishedFunc(TimelineFinished);
		FadeTimeline.AddInterpFloat(HighlightFadeCurve, TimelineUpdate);

		bIsTimelineInitialized = true;
	}
}

void UXRHighlightComponent::CacheHighlightableMeshComponents()
{
	const AActor* ParentActor = GetOwner();
	TInlineComponentArray<UMeshComponent*> MeshComponents;
	ParentActor->GetComponents(MeshComponents);

	TArray<UMeshComponent*> ValidComponents;
	for (auto* MeshComponent : MeshComponents)
	{
		if (MeshComponent)
		{
			if (MeshComponent->ComponentHasTag(HighlightMeshTag))
			{
				ValidComponents.Add(MeshComponent);
			}
		}
	}
	
	HighlightableMeshComponents = ValidComponents;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------

float UXRHighlightComponent::GetHighlightState()
{
	return HighlightState;
}

void UXRHighlightComponent::SetHighlightTag(FName InHighlightMeshTag)
{
	CacheHighlightableMeshComponents();
}

FName UXRHighlightComponent::GetHighlightTag() const
{
	return HighlightMeshTag;
}

TArray<UMeshComponent*> UXRHighlightComponent::GetHighlightMeshes() const
{
	return HighlightableMeshComponents;
}


void UXRHighlightComponent::SetActive(bool bNewActive, bool bReset)
{
	Super::SetActive(bNewActive, bReset);
	if(!bNewActive)
	{
		SetHighlighted(false);
	}
}


