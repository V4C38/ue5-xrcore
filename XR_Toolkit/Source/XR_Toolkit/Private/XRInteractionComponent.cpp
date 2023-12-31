
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
	SetIsReplicated(true);
}

void UXRInteractionComponent::InitializeComponent()
{
	Super::InitializeComponent();
	GetOwner()->SetReplicates(true);
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
	SetActiveInteractor(InInteractor);
	OnInteractionStart(InInteractor);
	OnInteractionStarted.Broadcast(this, InInteractor);
	RequestAudioPlay(InteractionStartSound);
	bIsInteractionActive = true;
	if (XRHighlightComponent)
	{
		XRHighlightComponent->SetHighlighted(0.0f);
	}
}

void UXRInteractionComponent::EndInteraction(UXRInteractorComponent* InInteractor)
{
	OnInteractionEnd(InInteractor);
	OnInteractionEnded.Broadcast(this, InInteractor);
	SetActiveInteractor(nullptr);
	RequestAudioPlay(InteractionEndSound);
	bIsInteractionActive = false;
}

void UXRInteractionComponent::HoverInteraction(UXRInteractorComponent* InInteractor, bool bInHoverState)
{
	OnInteractionHovered.Broadcast(this, InInteractor, bInHoverState);
	OnInteractionHover(bInHoverState, InInteractor);
	if (XRHighlightComponent && !bIsInteractionActive)
	{
		XRHighlightComponent->FadeXRHighlight(bInHoverState);
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
void UXRInteractionComponent::SetActiveInteractor(UXRInteractorComponent* InInteractor)
{
	if (InInteractor)
	{
		ActiveInteractor = InInteractor;
	}
	else
	{
		ActiveInteractor.Reset();
	}

}

int32 UXRInteractionComponent::GetInteractionPriority() const
{
	return InteractionPriority;
}

UXRInteractorComponent* UXRInteractionComponent::GetActiveInteractor()
{
	return ActiveInteractor.IsValid() ? ActiveInteractor.Get() : nullptr;
}

bool UXRInteractionComponent::IsInteractionActive() const
{
	return bIsInteractionActive;
}

bool UXRInteractionComponent::GetAllowTakeOver() const
{
	return bAllowTakeOver;
}

void UXRInteractionComponent::SetAllowTakeOver(bool bInAllowTakeOver)
{
	bAllowTakeOver = bInAllowTakeOver;
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
