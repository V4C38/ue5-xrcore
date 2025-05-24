// Fill out your copyright notice in the Description page of Project Settings.


#include "Connections/XRConnectorComponent.h"
#include "Connections/XRConnectorSocket.h"
#include "Connections/XRConnectorHologram.h"
#include "Interactions/XRInteractionGrab.h"
#include "Utilities/XRToolsUtilityFunctions.h"
#include "Utilities/XRReplicatedPhysicsComponent.h"
#include "Net/UnrealNetwork.h"

UXRConnectorComponent::UXRConnectorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);

	// Assign default hologram class from settings if not already set.
	const UXRCoreSettings* DefaultSettings = GetDefault<UXRCoreSettings>();
	if (DefaultSettings && !HologramClass)
	{
		HologramClass = DefaultSettings->DefaultHologramClass.Get();
	}
	else
	{
		HologramClass = AXRConnectorHologram::StaticClass();
	}
}

void UXRConnectorComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeOverlapBindings();
	InitializeInteractionBindings();
	MinDistanceToConnectSquared = FMath::Square(MinDistanceToConnect);
}

void UXRConnectorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (ConnectedSocket)
	{
		if (PreviouslyConnectedSocket.IsValid())
		{
			PreviouslyConnectedSocket.Get()->DeregisterConnection(this);
		}
	}
	HideAllHolograms();
}

void UXRConnectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();

	// Interpolate to Connected Socket when establishing connection
	if (EstablishConnectionTimer.IsValid() && ConnectedSocket)
	{
		float TimeLeft = GetWorld()->GetTimerManager().GetTimerRemaining(EstablishConnectionTimer);
		float Alpha = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, EstablishConnectionTime), FVector2D(1.0f, 0.0f), TimeLeft);

		FVector InterpolatedLocation = FMath::Lerp(Owner->GetActorLocation(), ConnectedSocket->GetComponentLocation(), Alpha);
		FQuat InterpolatedRotation = FMath::Lerp(Owner->GetActorQuat(), ConnectedSocket->GetComponentQuat(), Alpha);
		Owner->SetActorLocation(InterpolatedLocation);
		Owner->SetActorRotation(InterpolatedRotation);
	}

	if (bAutoBindToGrabInteraction && BoundGrabComponent)
	{
		if (BoundGrabComponent->IsInteractedWith())
		{
			ShowAllAvailableHolograms();
		}
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Connection API
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorComponent::Server_ConnectToSocket_Implementation(UXRConnectorSocket* InSocket)
{
	if (!InSocket)
	{
		return;
	}
	if (!InSocket->IsConnectionAllowed(this))
	{
		return;
	}

	ConnectedSocket = InSocket;
	OnRep_ConnectedSocket();
}

void UXRConnectorComponent::Server_ConnectToClosestOverlappedSocket_Implementation()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UXRConnectorSocket* ClosestSocket = GetClosestOverlappedSocket();
	if (ClosestSocket)
	{
		Server_ConnectToSocket(ClosestSocket);
	}
}

void UXRConnectorComponent::Server_DisconnectFromSocket_Implementation()
{
	ConnectedSocket = nullptr;
	OnRep_ConnectedSocket();
}


UXRConnectorSocket* UXRConnectorComponent::GetClosestOverlappedSocket()
{
	float MinSqDistance = FLT_MAX;
	UXRConnectorSocket* ClosestSocket = nullptr;

	for (auto OverlappedSocket : OverlappedSockets)
	{
		if (!OverlappedSocket.IsValid())
		{
			continue;
		}
		if (!OverlappedSocket.Get()->IsConnectionAllowed(this))
		{
			continue;
		}

		// Check distances between valid overlapped sockets and get the closetst one
		float DistanceSquared = FVector::DistSquared(GetOwner()->GetActorLocation(), OverlappedSocket.Get()->GetComponentLocation());
		if (DistanceSquared < MinSqDistance && DistanceSquared <= MinDistanceToConnectSquared)
		{
			MinSqDistance = DistanceSquared;
			ClosestSocket = OverlappedSocket.Get();
		}
	}
	return ClosestSocket;
}

bool UXRConnectorComponent::IsConnected(UXRConnectorSocket*& OutConnectedSocket) const
{
	OutConnectedSocket = nullptr;
	if (ConnectedSocket)
	{
		OutConnectedSocket = ConnectedSocket;
		return true;
	}
	return false;
}

FName UXRConnectorComponent::GetConnectorID() const
{
	return ConnectorID;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Replication
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorComponent::OnRep_ConnectedSocket()
{
	// Attach
	if (ConnectedSocket)
	{
		// Physics
		UXRReplicatedPhysicsComponent* XRPhysicsComponent = GetOwner()->FindComponentByClass<UXRReplicatedPhysicsComponent>();
		if (XRPhysicsComponent)
		{
			// Disable Physics
			if (GetOwnerRole() == ROLE_Authority)
			{
				XRPhysicsComponent->SetSimulatePhysicsOnOwner(false);
			}
		}

		if (EstablishConnectionTime <= 0.0f)
		{
			InternalAttachToSocket();
		}
		else
		{
			if (GetWorld()->GetTimerManager().IsTimerActive(EstablishConnectionTimer))
			{
				GetWorld()->GetTimerManager().ClearTimer(EstablishConnectionTimer);
			}
			GetWorld()->GetTimerManager().SetTimer(EstablishConnectionTimer, this, &UXRConnectorComponent::InternalAttachToSocket, EstablishConnectionTime, false);
			SetComponentTickEnabled(true);
		}
		OnConnected.Broadcast(this, ConnectedSocket);
	}

	// Detach
	else
	{
		if (PreviouslyConnectedSocket.Get())
		{
			PreviouslyConnectedSocket.Get()->DeregisterConnection(this);
		}
		GetOwner()->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));

		// Physics
		UXRReplicatedPhysicsComponent* XRPhysicsComponent = GetOwner()->FindComponentByClass<UXRReplicatedPhysicsComponent>();
		if (XRPhysicsComponent)
		{
			// Re-Enable Physics on Server
			if (GetOwnerRole() == ROLE_Authority)
			{
				XRPhysicsComponent->SetSimulatePhysicsOnOwner(true);
			}
		}

		OnDisconnected.Broadcast(this, PreviouslyConnectedSocket.Get());
		PreviouslyConnectedSocket = nullptr;
	}
}

void UXRConnectorComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRConnectorComponent, ConnectedSocket);
}



// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Connection Logic
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

// This is it`s own Method so it can be called after EstablishConnectionTimer finishes
void UXRConnectorComponent::InternalAttachToSocket() 
{
	UE_LOG(LogTemp, Warning, TEXT("InternalAttachToSocket"));

	if (!ConnectedSocket)
	{
		return;
	}

	if (PreviouslyConnectedSocket.IsValid())
	{
		PreviouslyConnectedSocket.Get()->DeregisterConnection(this);
	}

	SetComponentTickEnabled(false);
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	GetOwner()->AttachToComponent(ConnectedSocket, FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false));
	GetOwner()->SetActorRelativeLocation(FVector::ZeroVector);
	GetOwner()->SetActorRelativeRotation(FQuat::Identity);

	PreviouslyConnectedSocket = ConnectedSocket;
	ConnectedSocket->RegisterConnection(this);
	HideAllHolograms();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Overlap Logic
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorComponent::InitializeOverlapBindings()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	Owner->GetComponents<UPrimitiveComponent>(OwnerCollisions);

	for (auto Collider : OwnerCollisions)
	{
		Collider->OnComponentBeginOverlap.AddDynamic(this, &UXRConnectorComponent::OnOverlapBegin);
		Collider->OnComponentEndOverlap.AddDynamic(this, &UXRConnectorComponent::OnOverlapEnd);
	}
}

void UXRConnectorComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (UXRConnectorSocket* OverlappedSocket = Cast<UXRConnectorSocket>(OtherComp))
	{
		if (!OverlappedSocket->IsConnectionAllowed(this))
		{
			return;
		}
		if (OverlappedSockets.Contains(OverlappedSocket))
		{
			return;
		}
		OverlappedSockets.Add(OverlappedSocket);
		ShowAllAvailableHolograms();
	}
}

void UXRConnectorComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (UXRConnectorSocket* OverlappedSocket = Cast<UXRConnectorSocket>(OtherComp))
	{
		// Ensure an overlapped Socket is only removed when no Collider on the OwningActor is overlapping it anymore, not just the one that stopped overlapping
		for (auto Collider : OwnerCollisions)
		{
			if (Collider != OverlappedComponent)
			{
				TArray<UPrimitiveComponent*> OverlappedComponents = {};
				Collider->GetOverlappingComponents(OverlappedComponents);
				if (OverlappedComponents.Contains(OtherComp))
				{
					return;
				}
			}
		}
		OverlappedSockets.Remove(OverlappedSocket);
		ShowAllAvailableHolograms();
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Hologram
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorComponent::ShowHologram(UXRConnectorSocket* InSocket)
{
	if (!InSocket)
	{
		return;
	}
	if (!bShowConnectorHologram || !HologramMesh)
	{
		return;
	}
	if (!InSocket->IsHologramAllowed())
	{
		return;
	}

	// Only show the Hologram in Bound to Grab mode when the GrabInteraction is currently active
	if (bAutoBindToGrabInteraction)
	{
		if (!BoundGrabComponent)
		{
			return;
		}
		if (!BoundGrabComponent->IsInteractedWith())
		{
			return;
		}
	}

	auto FoundHologram = AssignedHolograms.Find(InSocket);
	if (FoundHologram && FoundHologram->IsValid())
	{
		if (FoundHologram->Get()->Implements<UXRHologramInterface>())
		{
			IXRHologramInterface::Execute_ShowHologram(FoundHologram->Get(), this, HologramMesh, HologramScale);
		}
		return;
	}

	FVector Location = InSocket->GetComponentLocation();
	FRotator Rotation = InSocket->GetComponentRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* SpawnedHologram = GetWorld()->SpawnActor<AXRConnectorHologram>(HologramClass, Location, Rotation, SpawnParams);
	if (!SpawnedHologram)
	{
		return;
	}
	AssignedHolograms.Add(InSocket, SpawnedHologram);
	SpawnedHologram->AttachToComponent(InSocket, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
	if (SpawnedHologram->Implements<UXRHologramInterface>())
	{
		IXRHologramInterface::Execute_ShowHologram(SpawnedHologram, this, HologramMesh, HologramScale);
	}
}

void UXRConnectorComponent::ShowAllAvailableHolograms()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (bAutoBindToGrabInteraction && BoundGrabComponent)
	{
		if (!BoundGrabComponent->IsInteractedWith() || ConnectedSocket)
		{
			HideAllHolograms();
			return;
		}
	}

	UXRConnectorSocket* ClosestSocket = GetClosestOverlappedSocket();
	TArray<TWeakObjectPtr<UXRConnectorSocket>> InvalidSockets = {};
	for (auto OverlappedSocket : OverlappedSockets)
	{
		if (!OverlappedSocket.Get())
		{
			InvalidSockets.Add(OverlappedSocket);
			continue;
		}
		if (!OverlappedSocket.Get()->IsConnectionAllowed(this))
		{
			HideHologram(OverlappedSocket.Get());
			continue;
		}
		ShowHologram(OverlappedSocket.Get());
		SetHologramState(OverlappedSocket.Get(), OverlappedSocket.Get() == ClosestSocket);
	}

	for (auto InvalidSocket : InvalidSockets)
	{
		OverlappedSockets.Remove(InvalidSocket);
	}

}

void UXRConnectorComponent::HideHologram(UXRConnectorSocket* InSocket)
{
	if (!InSocket)
	{
		return;
	}
	auto FoundHologram = AssignedHolograms.Find(InSocket);
	if (FoundHologram && FoundHologram->IsValid())
	{
		if (FoundHologram->Get()->Implements<UXRHologramInterface>())
		{
			IXRHologramInterface::Execute_HideHologram(FoundHologram->Get(), this);
		}
	}
	else
	{
		AssignedHolograms.Remove(InSocket);
	}
}

void UXRConnectorComponent::HideAllHolograms()
{
	for (auto Socket : OverlappedSockets)
	{
		if (Socket.IsValid())
		{
			HideHologram(Socket.Get());
		}
	}
}

void UXRConnectorComponent::SetHologramState(UXRConnectorSocket* InSocket, bool InState)
{
	if (!InSocket)
	{
		return;
	}
	auto FoundHologram = AssignedHolograms.Find(InSocket);
	if (FoundHologram && FoundHologram->IsValid())
	{
		auto Hologram = FoundHologram->Get();
		if (Hologram->Implements<UXRHologramInterface>())
		{
			IXRHologramInterface::Execute_SetHologramEnabled(Hologram, this, InState);
		}
	}
	else
	{
		AssignedHolograms.Remove(InSocket);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Interaction Bindings
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorComponent::InitializeInteractionBindings()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	TArray<UXRInteractionGrab*> FoundGrabInteractions;
	Owner->GetComponents<UXRInteractionGrab>(FoundGrabInteractions);
	TArray<UXRInteractionComponent*> InteractionComponents;
	for (UXRInteractionGrab* GrabInteraction : FoundGrabInteractions)
	{
		// Because UXRInteractionGrab inherits from UXRInteractionComponent, we can safely cast
		UXRInteractionComponent* InteractionComponent = Cast<UXRInteractionComponent>(GrabInteraction);
		if (InteractionComponent)
		{
			InteractionComponents.Add(InteractionComponent);
		}
	}
	auto FoundInteractionComp = UXRToolsUtilityFunctions::GetXRInteractionByPriority(InteractionComponents, nullptr, 0, EXRInteractionPrioritySelection::LowerEqual,5);
	if (FoundInteractionComp)
	{
		FoundInteractionComp->OnInteractionStarted.AddDynamic(this, &UXRConnectorComponent::OnInteractionStarted);
		FoundInteractionComp->OnInteractionEnded.AddDynamic(this, &UXRConnectorComponent::OnInteractionEnded);
		BoundGrabComponent = Cast<UXRInteractionGrab>(FoundInteractionComp);
	}
}

void UXRConnectorComponent::OnInteractionStarted(UXRInteractionComponent* Sender, UXRInteractorComponent* XRInteractorComponent)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		Server_DisconnectFromSocket();
	}
	ShowAllAvailableHolograms();
}

void UXRConnectorComponent::OnInteractionEnded(UXRInteractionComponent* Sender, UXRInteractorComponent* XRInteractorComponent)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		Server_ConnectToClosestOverlappedSocket();
	}
	HideAllHolograms();
}
