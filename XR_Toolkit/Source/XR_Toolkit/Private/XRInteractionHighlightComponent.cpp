// Fill out your copyright notice in the Description page of Project Settings.


#include "XRInteractionHighlightComponent.h"

#include "XRInteractorComponent.h"


void UXRInteractionHighlightComponent::OnInteractionHovered(UXRInteractionComponent* Sender, UXRInteractorComponent* XRInteractor, bool bHovered)
{
	if (GetAssignedXRInteraction() && XRInteractor)
	{
		if (XRInteractor != GetAssignedXRInteraction()->GetActiveInteractor())
		{
			if (XRInteractor->IsLaserInteractor())
			{
				if (GetAssignedXRInteraction()->IsLaserInteractionEnabled())
				{
					FadeXRHighlight(bHovered);
				}
			}
			else
			{
				FadeXRHighlight(bHovered);
			}
		}
		else
		{
			FadeTimeline.Stop();
			SetHighlighted(false);
		}
	}
}

void UXRInteractionHighlightComponent::OnInteractionStarted(UXRInteractionComponent* Sender,
	UXRInteractorComponent* XRInteractorComponent)
{
	if (GetAssignedXRInteraction())
	{
		if(GetAssignedXRInteraction()->GetIsContinuousInteraction())
		{
			SetActive(false, false);
		}
	}
}

void UXRInteractionHighlightComponent::OnInteractionEnded(UXRInteractionComponent* Sender,
	UXRInteractorComponent* XRInteractorComponent)
{
	if (GetAssignedXRInteraction())
	{
		SetActive(true, false);
	}
}

void UXRInteractionHighlightComponent::OnInteractionComponentActivated(UActorComponent* Component, bool Reset)
{
	SetActive(true, false);
}

void UXRInteractionHighlightComponent::OnInteractionComponentDeactivated(UActorComponent* Component)
{
	SetActive(false, false);
}


void UXRInteractionHighlightComponent::AssignXRInteraction(
	UXRInteractionComponent* InAssignedInteractionComponent)
{
	AssignedInteractionComponent = InAssignedInteractionComponent;
	BindToInteractionHover();
	BindToInteraction();
}

void UXRInteractionHighlightComponent::UnassignXRInteraction()
{
	UnbindFromInteractionHover();
	UnbindFromInteraction();
	AssignedInteractionComponent = nullptr;
}


UXRInteractionComponent* UXRInteractionHighlightComponent::GetAssignedXRInteraction() const
{
	return AssignedInteractionComponent;
}

void UXRInteractionHighlightComponent::BindToInteractionHover()
{
	if (GetAssignedXRInteraction())
	{
		AssignedInteractionComponent->OnInteractionHovered.AddDynamic(this, &UXRInteractionHighlightComponent::OnInteractionHovered);
	}
}

void UXRInteractionHighlightComponent::UnbindFromInteractionHover()
{
	if (GetAssignedXRInteraction())
	{
		AssignedInteractionComponent->OnInteractionHovered.RemoveDynamic(this, &UXRInteractionHighlightComponent::OnInteractionHovered);
	}
}

void UXRInteractionHighlightComponent::BindToInteraction()
{
	if (GetAssignedXRInteraction())
	{
		AssignedInteractionComponent->OnInteractionStarted.AddDynamic(this, &UXRInteractionHighlightComponent::OnInteractionStarted);
		AssignedInteractionComponent->OnInteractionEnded.AddDynamic(this, &UXRInteractionHighlightComponent::OnInteractionEnded);
		AssignedInteractionComponent->OnComponentActivated.AddDynamic(this, &UXRInteractionHighlightComponent::OnInteractionComponentActivated);
		AssignedInteractionComponent->OnComponentDeactivated.AddDynamic(this, &UXRInteractionHighlightComponent::OnInteractionComponentDeactivated);
	}
}

void UXRInteractionHighlightComponent::UnbindFromInteraction()
{
	if (GetAssignedXRInteraction())
	{
		AssignedInteractionComponent->OnInteractionStarted.RemoveDynamic(this, &UXRInteractionHighlightComponent::OnInteractionStarted);
		AssignedInteractionComponent->OnInteractionEnded.RemoveDynamic(this, &UXRInteractionHighlightComponent::OnInteractionEnded);
		AssignedInteractionComponent->OnComponentActivated.RemoveDynamic(this, &UXRInteractionHighlightComponent::OnInteractionComponentActivated);
		AssignedInteractionComponent->OnComponentDeactivated.RemoveDynamic(this, &UXRInteractionHighlightComponent::OnInteractionComponentDeactivated);
	}
}

void UXRInteractionHighlightComponent::SetActive(bool bNewActive, bool bReset)
{
	Super::SetActive(bNewActive, bReset);
	if(!bNewActive)
	{
		SetHighlighted(false);
	}
	else
	{
		UXRInteractionComponent* AssignedInteractionComp = GetAssignedXRInteraction();
		if (AssignedInteractionComp->GetHoveringInteractor())
		{
			FadeXRHighlight(true);
		}
	}
}






