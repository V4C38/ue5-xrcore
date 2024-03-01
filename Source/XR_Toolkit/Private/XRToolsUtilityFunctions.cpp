#include "XRToolsUtilityFunctions.h"
#include "XRInteractionComponent.h"
#include "XRInteractorComponent.h"




bool UXRToolsUtilityFunctions::IsActorInteractive(AActor* InActor, TArray<UXRInteractionComponent*>& OutActiveXRInteractions)
{
    if (!InActor)
    {
        return false;
    }

    OutActiveXRInteractions.Empty();
    TArray<UActorComponent*> Components;
    InActor->GetComponents(Components);

    for (UActorComponent* Component : Components)
    {
        UXRInteractionComponent* InteractionComponent = Cast<UXRInteractionComponent>(Component);
        OutActiveXRInteractions.Add(InteractionComponent);
    }

    return OutActiveXRInteractions.Num() > 0;
}


UFUNCTION(BlueprintPure, Category = "XRTools|Utilities")
bool UXRToolsUtilityFunctions::IsActorInteractedWith(AActor* InActor, TArray<UXRInteractionComponent*>& OutActiveXRInteractions)
{
    if (!InActor)
    {
        return false;
    }

    OutActiveXRInteractions.Empty();
    TArray<UActorComponent*> Components;
    InActor->GetComponents(Components);

    for (UActorComponent* Component : Components)
    {
        UXRInteractionComponent* InteractionComponent = Cast<UXRInteractionComponent>(Component);
        if (InteractionComponent && InteractionComponent->IsInteractedWith())
        {
            OutActiveXRInteractions.Add(InteractionComponent);
        }
    }

    return OutActiveXRInteractions.Num() > 0;
}


UXRInteractionComponent* UXRToolsUtilityFunctions::GetXRInteractionByPriority(TArray<UXRInteractionComponent*> InInteractions, UXRInteractorComponent* InXRInteractor, int32 InPriority)
{
    for (UXRInteractionComponent* XRInteraction : InInteractions)
    {
        if (!XRInteraction->IsActive())
        {
            continue;
        }
        if (XRInteraction->IsInteractedWith())
        {
            if (InXRInteractor)
            {
                if (XRInteraction->GetActiveInteractors().Contains(InXRInteractor))
                {
                    continue;
                }
            }
            switch (XRInteraction->GetMultiInteractorBehavior())
            {
                default:
                    break;
                case EXRMultiInteractorBehavior::Disabled:
                    continue;
            }
        }
        int32 CurrentPriority = XRInteraction->GetInteractionPriority();
        if (CurrentPriority == InPriority)
        {
            return XRInteraction;
        }
    }
    return nullptr;
}



// Determines the interaction with the lowest value InteractionPriority (iE. highest priority)
UXRInteractionComponent* UXRToolsUtilityFunctions::GetHighestPrioriyXRInteraction(TArray<UXRInteractionComponent*> InInteractions, UXRInteractorComponent* InXRInteractor)
{
    int32 Priority = -1;
    UXRInteractionComponent* OutXRInteraction = nullptr;
    TArray<UXRInteractionComponent*>  ValidInteractions = {};
    for (UXRInteractionComponent* XRInteraction : InInteractions)
    {
        if (!XRInteraction->IsActive())
        {
            continue;
        }
        if (XRInteraction->IsInteractedWith())
        {
            if (InXRInteractor)
            {
                if (XRInteraction->GetActiveInteractors().Contains(InXRInteractor))
                {
                    continue;
                }
            }
            if (XRInteraction->GetMultiInteractorBehavior() != EXRMultiInteractorBehavior::Enabled)
            {
                continue;
            }
        }
        int32 CurrentPriority = XRInteraction->GetInteractionPriority();
        if (CurrentPriority >= 0 && CurrentPriority < Priority)
        {
            Priority = CurrentPriority;
            OutXRInteraction = XRInteraction;
        }

    }
    return OutXRInteraction;
}

UXRInteractionComponent* UXRToolsUtilityFunctions::GetXRInteractionOnActorByPriority(AActor* InActor, UXRInteractorComponent* InXRInteractor, int32 InPriority)
{
    if (!InActor)
    {
        return nullptr;
    }

    TArray<UActorComponent*> Components;
    InActor->GetComponents(Components);
    TArray<UXRInteractionComponent*> InteractionComponents;

    for (UActorComponent* Component : Components)
    {
        UXRInteractionComponent* InteractionComponent = Cast<UXRInteractionComponent>(Component);
        if (InteractionComponent)
        {
            InteractionComponents.Add(InteractionComponent);
        }
    }
    return GetXRInteractionByPriority(InteractionComponents, InXRInteractor, InPriority);
}
