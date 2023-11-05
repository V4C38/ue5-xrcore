
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
	if (IsContinuousInteraction())
	{
		bIsInteractionActive = true;
		if (XRHighlightComponent)
		{
			XRHighlightComponent->SetHighlighted(0.0f);
		}
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
	if (IsContinuousInteraction())
	{
		ActiveInteractor = InInteractor;
	}
}

int32 UXRInteractionComponent::GetInteractionPriority() const
{
	return InteractionPriority;
}

UXRInteractorComponent* UXRInteractionComponent::GetActiveInteractor()
{
	return ActiveInteractor;
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

bool UXRInteractionComponent::IsContinuousInteraction() const
{
	return bIsContinuousInteraction;
}

bool UXRInteractionComponent::IsLaserInteractionEnabled() const
{
	return bEnableLaserInteraction;
}

bool UXRInteractionComponent::GetSupressLaserWhenInteracting() const
{
	return bSupressLaserWhenInteracting;
}

void UXRInteractionComponent::SetSupressLaserWhenInteracting(bool InSupressLaser)
{
	bSupressLaserWhenInteracting = InSupressLaser;
}

bool UXRInteractionComponent::GetSnapXRLaserToActor() const
{
	return bSnapXRLaserToActor;
}

void UXRInteractionComponent::SetSnapXRLaserToActor(bool InSnapXRLaserToActor)
{
	bSnapXRLaserToActor = InSnapXRLaserToActor;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
