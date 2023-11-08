#include "XRToolsUtilityFunctions.h"
#include "XRInteractionComponent.h"




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
UXRInteractionComponent* UXRToolsUtilityFunctions::GetPrioritizedXRInteraction(TArray<UXRInteractionComponent*> InInteractions, bool IgnoreActive, bool SortByLowest) 
{
    int32 Priority = SortByLowest ? INT_MAX : INT_MIN;
    UXRInteractionComponent* OutXRInteraction = nullptr;
    for (UXRInteractionComponent* XRInteraction : InInteractions)
    {
        if (IgnoreActive && XRInteraction->IsInteractionActive())
        {
            continue;
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