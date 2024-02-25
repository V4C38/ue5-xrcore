
#include "XRInteractionComponent.h"
#include "XRInteractorComponent.h"
#include "XRHighlightComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/GameSession.h"
#include "Kismet/GameplayStatics.h"

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
UXRInteractionComponent::UXRInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);
}

void UXRInteractionComponent::InitializeComponent()
{
	Super::InitializeComponent();
	GetOwner()->SetReplicates(true);
	UpdateAbsolouteInteractionPriority();
}

void UXRInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	if (bEnableHighlighting)
	{
		SpawnAndConfigureXRHighlight();
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Interaction Events
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionComponent::StartInteraction(UXRInteractorComponent* InInteractor)
{
	ActiveInteractors.AddUnique(TWeakObjectPtr<UXRInteractorComponent>(InInteractor));
	OnInteractionStart(InInteractor);
	OnInteractionStarted.Broadcast(this, InInteractor);
	RequestAudioPlay(InteractionStartSound);
	if (XRHighlightComponent)
	{
		XRHighlightComponent->SetHighlighted(0.0f);
	}
}

void UXRInteractionComponent::EndInteraction(UXRInteractorComponent* InInteractor)
{
	ActiveInteractors.Remove(TWeakObjectPtr<UXRInteractorComponent>(InInteractor));
	OnInteractionEnded.Broadcast(this, InInteractor);
	RequestAudioPlay(InteractionEndSound);
}

void UXRInteractionComponent::HoverInteraction(UXRInteractorComponent* InInteractor, bool bInHoverState)
{
	if (!InInteractor)
	{
		OnInteractionHovered.Broadcast(this, InInteractor, bInHoverState);
		OnInteractionHover(bInHoverState, InInteractor);
		return;
	}

	if (bInHoverState)
	{
		HoveringInteractors.AddUnique(TWeakObjectPtr<UXRInteractorComponent>(InInteractor));
		if (HoveringInteractors.Num() == 1 && GetActiveInteractors().Num() == 0)
		{
			OnInteractionHover(true, InInteractor);
			OnInteractionHovered.Broadcast(this, InInteractor, true);
			if (XRHighlightComponent)
			{
				XRHighlightComponent->FadeXRHighlight(true);
			}
		}
	}
	else
	{
		HoveringInteractors.Remove(TWeakObjectPtr<UXRInteractorComponent>(InInteractor));
		if (HoveringInteractors.Num() == 0 && GetActiveInteractors().Num() == 0)
		{
			OnInteractionHover(false, InInteractor);
			OnInteractionHovered.Broadcast(this, InInteractor, false);
			if (XRHighlightComponent)
			{
				XRHighlightComponent->FadeXRHighlight(false);
			}
		}
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Highlighting & Audio
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionComponent::SpawnAndConfigureXRHighlight()
{
	if (XRHighlightComponent)
	{
		return;
	}
	XRHighlightComponent = NewObject<UXRHighlightComponent>(this->GetOwner());
	if (XRHighlightComponent)
	{
		XRHighlightComponent->RegisterComponent();
		XRHighlightComponent->SetHighlightFadeCurve(HighlightFadeCurve);
		XRHighlightComponent->SetHighlightIgnoreMeshTag(HighlightIgnoreMeshTag);
		XRHighlightComponent->Activate();
	}
}

UXRHighlightComponent* UXRInteractionComponent::GetXRHighlightComponent()
{
	return XRHighlightComponent;
}


void UXRInteractionComponent::RequestAudioPlay(USoundBase* InSound)
{
	if (CurrentAudioComponent && CurrentAudioComponent->IsPlaying())
	{
		CurrentAudioComponent->Stop();
	}

	if (InSound)
	{
		CurrentAudioComponent = UGameplayStatics::SpawnSoundAtLocation(
			GetWorld(),
			InSound,
			this->GetComponentLocation()
		);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Utility
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 UXRInteractionComponent::GetInteractionPriority()
{
	return AbsolouteInteractionPriority;
}
void UXRInteractionComponent::UpdateAbsolouteInteractionPriority()
{
	int32 OutInteractionPriority = 0;
	switch (InteractionPriority)
	{
		case EXRInteractionPriority::Primary:
			OutInteractionPriority = 1;
			break;
		case EXRInteractionPriority::Secondary:
			OutInteractionPriority = 2;
			break;
		case EXRInteractionPriority::Custom:
			OutInteractionPriority = AbsolouteInteractionPriority;
			break;
	}
	AbsolouteInteractionPriority = OutInteractionPriority;
}

void UXRInteractionComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UXRInteractionComponent, InteractionPriority))
	{
		bUsingCustomPriorityValue = InteractionPriority == EXRInteractionPriority::Custom;
		UpdateAbsolouteInteractionPriority();
	}
}

TArray<UXRInteractorComponent*> UXRInteractionComponent::GetActiveInteractors() const
{
	TArray<UXRInteractorComponent*> OutInteractors = {};
	for (auto Interactor : ActiveInteractors)
	{
		if (Interactor.IsValid())
		{
			OutInteractors.AddUnique(Interactor.Get());
		}
	}
	return OutInteractors;
}


EXRMultiInteractorBehavior UXRInteractionComponent::GetMultiInteractorBehavior() const
{
	return MultiInteractorBehavior;
}


bool UXRInteractionComponent::IsInteractionActive() const
{
	return GetActiveInteractors().Num() > 0;
}

EXRLaserBehavior UXRInteractionComponent::GetLaserBehavior() const
{
	return LaserBehavior;
}


void UXRInteractionComponent::SetLaserBehavior(EXRLaserBehavior InLaserBehavior)
{
	LaserBehavior = InLaserBehavior;
}

bool UXRInteractionComponent::IsLaserInteractionEnabled() const
{
	if (LaserBehavior == EXRLaserBehavior::Disabled)
	{
		return false;
	}
	if (LaserBehavior == EXRLaserBehavior::Supress && IsInteractionActive())
	{
		return false;
	}
	return true;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
