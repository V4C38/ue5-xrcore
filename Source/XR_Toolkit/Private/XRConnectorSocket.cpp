// Fill out your copyright notice in the Description page of Project Settings.


#include "XRConnectorSocket.h"
#include "XRConnectorComponent.h"

UXRConnectorSocket::UXRConnectorSocket()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void UXRConnectorSocket::BeginPlay()
{
	Super::BeginPlay();
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// API
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorSocket::RegisterConnection(UXRConnectorComponent* InConnectorComponent)
{
    if (!InConnectorComponent)
    {
        return;
    }
    AttachedXRConnectors.AddUnique(InConnectorComponent);
    OnSocketConnected.Broadcast(this, InConnectorComponent);
}

void UXRConnectorSocket::DeregisterConnection(UXRConnectorComponent* InConnectorComponent)
{
    if (!InConnectorComponent)
    {
        return;
    }
    AttachedXRConnectors.Remove(InConnectorComponent);
    OnSocketDisconnected.Broadcast(this, InConnectorComponent);
}

TArray<UXRConnectorComponent*> UXRConnectorSocket::GetAttachedXRConnectors() const
{
    TArray<UXRConnectorComponent*> OutComponents;
    for (auto AttachedXRConnector : AttachedXRConnectors)
    {
        if (AttachedXRConnector.IsValid())
        {
            OutComponents.AddUnique(AttachedXRConnector.Get());
        }
    }
    return OutComponents;
}

TArray<FName> UXRConnectorSocket::GetCompatibleIDs() const
{
    return CompatibleConnectorIDs;
}

bool UXRConnectorSocket::IsConnectionAllowed(UXRConnectorComponent* InXRConnectorComponent) const
{
    if(!InXRConnectorComponent)
    {
        return false;
    }
    // Disallow attaching Connectors to Sockets on the same Actor
    if (GetOwner() == InXRConnectorComponent->GetOwner())
    {
        return false;
    }
    if (GetAttachedXRConnectors().Num() >= AllowedConnections)
    {
        return false;
    }
    return CompatibleConnectorIDs.Contains(InXRConnectorComponent->GetConnectorID());
}