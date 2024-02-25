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
        if (InteractionComponent && InteractionComponent->IsInteractionActive())
        {
            OutActiveXRInteractions.Add(InteractionComponent);
        }
    }

    return OutActiveXRInteractions.Num() > 0;
}

// Determines the interaction with the highest or lowest priority, depending on SortByLowest
UXRInteractionComponent* UXRToolsUtilityFunctions::GetPrioritizedXRInteraction(TArray<UXRInteractionComponent*> InInteractions, UXRInteractorComponent* InXRInteractor, bool SortByLowest)
{
    int32 Priority = SortByLowest ? INT_MAX : INT_MIN;
    UXRInteractionComponent* OutXRInteraction = nullptr;
    for (UXRInteractionComponent* XRInteraction : InInteractions)
    {
        if (!XRInteraction->IsActive())
        {
            continue;
        }
        if (XRInteraction->IsInteractionActive())
        {
            // If Currently active Interaction, skip this Interaction if MultiInteractor is Disabled
            auto ActiveInteractors = XRInteraction->GetActiveInteractors();
            if (ActiveInteractors.Num() > 0 && !ActiveInteractors.Contains(InXRInteractor))
            {
                if (XRInteraction->GetMultiInteractorBehavior() == EXRMultiInteractorBehavior::Disabled)
                {
                    continue;
                }
            }
        }
        int32 CurrentPriority = XRInteraction->GetInteractionPriority();
        bool ShouldUpdate = SortByLowest ? CurrentPriority < Priority : CurrentPriority > Priority;
        if (ShouldUpdate)
        {
            Priority = CurrentPriority;
            OutXRInteraction = XRInteraction;
        }
    }
    return OutXRInteraction;
}


UXRInteractionComponent* UXRToolsUtilityFunctions::GetPrioritizedXRInteractionOnActor(AActor* InActor, UXRInteractorComponent* InXRInteractor, bool SortByLowest)
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
    return GetPrioritizedXRInteraction(InteractionComponents, InXRInteractor, SortByLowest);
}
