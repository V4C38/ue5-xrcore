
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
	SetHighlightTag(HighlightMeshTag);
	
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

void UXRHighlightComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FadeTimeline.TickTimeline(DeltaTime);

	// NOTE: for some reason the Component will not stop ticking even though it is called on timeline finished
	// GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("Highlight Tick"));
}


void UXRHighlightComponent::SetHighlighted(float InHighlightState)
{
	HighlightState = InHighlightState;

	for (auto* HighlightMeshComponent : HighlightableMeshComponents)
	{
		if (HighlightMeshComponent)
		{
			HighlightMeshComponent->SetScalarParameterValueOnMaterials("HighlightState", InHighlightState);
		}
	}
}


float UXRHighlightComponent::GetHighlightState()
{
	return HighlightState;
}


void UXRHighlightComponent::FadeXRHighlight(bool bFadeIn)
{
	if (IsActive())
	{
		if (HighlightFadeCurve)
		{
			SetComponentTickEnabled(true);
			if(bFadeIn)
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
	else
	{
		SetHighlighted(0.0f);
	}

}

void UXRHighlightComponent::TimelineUpdate(float InTimelineProgress)
{
	SetHighlighted(InTimelineProgress);
}

void UXRHighlightComponent::TimelineFinished()
{
	SetComponentTickEnabled(false);
	// GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("Highlight Tick"));
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

void UXRHighlightComponent::SetActive(bool bNewActive, bool bReset)
{
	Super::SetActive(bNewActive, bReset);
	if(!bNewActive)
	{
		SetHighlighted(false);
	}
}





