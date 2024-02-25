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
UXRInteractionComponent* UXRToolsUtilityFunctions::GetPrioritizedXRInteraction(TArray<UXRInteractionComponent*> InInteractions, UXRInteractorComponent* InXRInteractor, 
bool IgnoreActive, bool SortByLowest)
{
    int32 Priority = SortByLowest ? INT_MAX : INT_MIN;
    UXRInteractionComponent* OutXRInteraction = nullptr;
    for (UXRInteractionComponent* XRInteraction : InInteractions)
    {
        if (IgnoreActive && XRInteraction->IsInteractionActive())
        {
            if (InXRInteractor == XRInteraction->GetActiveInteractor())
            {
                continue;
            }
            else
            {
                if (!XRInteraction->GetAllowTakeOver())
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


UXRInteractionComponent* UXRToolsUtilityFunctions::GetPrioritizedXRInteractionOnActor(AActor* InActor, UXRInteractorComponent* InXRInteractor, bool IgnoreActive, bool SortByLowest)
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
    return GetPrioritizedXRInteraction(InteractionComponents, InXRInteractor, IgnoreActive, SortByLowest);
}
