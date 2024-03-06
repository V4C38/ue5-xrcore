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


UXRInteractionComponent* UXRToolsUtilityFunctions::GetXRInteractionByPriority(TArray<UXRInteractionComponent*> InInteractions, UXRInteractorComponent* InXRInteractor, int32 InPriority, 
    EXRInteractionPrioritySelection InPrioritySelectionCondition, int32 MaxSecondaryPriority)
{
    if (InInteractions.Num() == 0)
    {
        return nullptr;
    }
    TArray<UXRInteractionComponent*> ValidXRInteractions = {};
    for (UXRInteractionComponent* XRInteraction : InInteractions)
    {
        if (!XRInteraction)
        {
            continue;
        }
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
        if (XRInteraction->GetInteractionPriority() == InPriority)
        {
            return XRInteraction;
        }
        ValidXRInteractions.Add(XRInteraction);
    }

    int32 NextPriority = InPriority;
    switch (InPrioritySelectionCondition)
    {
        case EXRInteractionPrioritySelection::Equal:
            return nullptr;
        case EXRInteractionPrioritySelection::HigherEqual:
            NextPriority = InPriority - 1;
            break;
        case EXRInteractionPrioritySelection::LowerEqual:
            NextPriority = InPriority + 1;
            break;
    }
    if (NextPriority < 0 || NextPriority >= MaxSecondaryPriority)
    {
        return nullptr;
    }
    return GetXRInteractionByPriority(ValidXRInteractions, InXRInteractor, NextPriority, InPrioritySelectionCondition);
}


UXRInteractionComponent* UXRToolsUtilityFunctions::GetXRInteractionOnActorByPriority(AActor* InActor, UXRInteractorComponent* InXRInteractor, int32 InPriority, EXRInteractionPrioritySelection InPrioritySelectionCondition)
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
    return GetXRInteractionByPriority(InteractionComponents, InXRInteractor, InPriority, InPrioritySelectionCondition);
}
