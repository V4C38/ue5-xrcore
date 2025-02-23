// Fill out your copyright notice in the Description page of Project Settings.


#include "XRConnectorSocket.h"
#include "XRConnectorComponent.h"

UXRConnectorSocket::UXRConnectorSocket()
{
    PrimaryComponentTick.bCanEverTick = false;
    bAutoActivate = true;
    SetIsReplicatedByDefault(true);
}

void UXRConnectorSocket::BeginPlay()
{
    Super::BeginPlay();
    SocketState = DefaultSocketState;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// API
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorSocket::SetSocketState(EXRConnectorSocketState InSocketState)
{
    SocketState = InSocketState;
    if (SocketState != EXRConnectorSocketState::Occupied)
    {
        DefaultSocketState = InSocketState;
    }
}

EXRConnectorSocketState UXRConnectorSocket::GetSocketState() const
{
    return SocketState;
}

FString UXRConnectorSocket::GetSocketID() const
{
    return ID;
}


void UXRConnectorSocket::RegisterConnection(UXRConnectorComponent* InConnectorComponent)
{
    if (!InConnectorComponent)
    {
        return;
    }
    AttachedXRConnectors.AddUnique(InConnectorComponent);
    SocketState = EXRConnectorSocketState::Occupied;
    OnSocketConnected.Broadcast(this, InConnectorComponent);
}

void UXRConnectorSocket::DeregisterConnection(UXRConnectorComponent* InConnectorComponent)
{
    if (!InConnectorComponent)
    {
        return;
    }
    AttachedXRConnectors.Remove(InConnectorComponent);
    SocketState = DefaultSocketState;
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
    if (SocketState == EXRConnectorSocketState::Disabled || SocketState == EXRConnectorSocketState::Occupied)
    {
        return false;
    }
    if (!CompatibleConnectorIDs.IsEmpty())
    {
        if (!CompatibleConnectorIDs.Contains(InXRConnectorComponent->GetConnectorID()))
        {
            return false;
        }
    }
    return true;
}

bool UXRConnectorSocket::IsHologramAllowed() const
{
    if (SocketState == EXRConnectorSocketState::Disabled || bSupressHologram)
    {
        return false;
    }
    return true;
}