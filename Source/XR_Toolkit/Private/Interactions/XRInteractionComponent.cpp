
#include "Interactions/XRInteractionComponent.h"
#include "Interactions/XRInteractorComponent.h"
#include "Utilities/XRHighlightComponent.h"
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

	// Assign default hologram class from settings if not already set.
	const UXRCoreSettings* DefaultSettings = GetDefault<UXRCoreSettings>();
	if (DefaultSettings)
	{
		if (!InteractionStartSound)
		{
			InteractionStartSound = DefaultSettings->DefaultInteractionStartSound.Get();
		}
		if (!InteractionEndSound)
		{
			InteractionEndSound = DefaultSettings->DefaultInteractionEndSound.Get();
		}
		if (!HighlightFadeCurve)
		{
			HighlightFadeCurve = DefaultSettings->DefaultHighlightFadeCurve.Get();
		}
	}
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
	HoveringInteractors.Remove(TWeakObjectPtr<UXRInteractorComponent>(InInteractor));
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
		return;
	}
	if (bInHoverState)
	{	
		TArray<UXRInteractorComponent*> CurrentHoveringInteractors = {};
		if (!IsHovered(CurrentHoveringInteractors))
		{
			OnInteractionHover(true, InInteractor);
			OnInteractionHovered.Broadcast(this, InInteractor, true);
			if (XRHighlightComponent)
			{
				XRHighlightComponent->FadeXRHighlight(true);
			}
		}
		HoveringInteractors.AddUnique(TWeakObjectPtr<UXRInteractorComponent>(InInteractor));
	}
	if (!bInHoverState)
	{
		HoveringInteractors.Remove(TWeakObjectPtr<UXRInteractorComponent>(InInteractor));
		TArray<UXRInteractorComponent*> CurrentHoveringInteractors = {};
		if (!IsHovered(CurrentHoveringInteractors))
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


bool UXRInteractionComponent::IsHovered(TArray<UXRInteractorComponent*>& OutHoveringInteractors)
{
	for (auto HoveringInteractor : HoveringInteractors)
	{
		UXRInteractorComponent* ValidInteractor = HoveringInteractor.Get();
		if (ValidInteractor)
		{
			OutHoveringInteractors.Add(ValidInteractor);
		}
	}
	return OutHoveringInteractors.Num() > 0;
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
		XRHighlightComponent->SetHighlightIncludeOnlyTags(HighlightIncludeOnlyTags);
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


bool UXRInteractionComponent::IsInteractedWith() const
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
	if (LaserBehavior == EXRLaserBehavior::Supress && IsInteractedWith())
	{
		return false;
	}
	return true;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
