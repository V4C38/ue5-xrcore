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

	for (auto Collider : OwnerCollisions)
	{
		if (Collider)
		{
			Collider->OnComponentBeginOverlap.RemoveAll(this);
			Collider->OnComponentEndOverlap.RemoveAll(this);
		}
	}
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

	if (EstablishConnectionTime <= 0.0f)
	{
		AttachToSocket();
	}
	else
	{
		DeferredAttachToSocket();
	}
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
	// Physics
	UXRReplicatedPhysicsComponent* XRPhysicsComponent = GetOwner()->FindComponentByClass<UXRReplicatedPhysicsComponent>();
	if (XRPhysicsComponent)
	{
		XRPhysicsComponent->SetSimulatePhysicsOnOwner(true);
	}

	ConnectedSocket = nullptr;
	DetachFromSocket();
}


UXRConnectorSocket* UXRConnectorComponent::GetClosestOverlappedSocket()
{
	float MinSqDistance = UE_BIG_NUMBER;
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
// Connection Logic
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// This is it`s own Method so it can be called after EstablishConnectionTimer finishes
void UXRConnectorComponent::AttachToSocket() 
{
	if (!ConnectedSocket || !GetOwner())
	{
		return;
	}

	SetComponentTickEnabled(false);

	// Clear previous connection if exists
	if (PreviouslyConnectedSocket.IsValid())
	{
		PreviouslyConnectedSocket.Get()->DeregisterConnection(this);
		OnDisconnected.Broadcast(this, PreviouslyConnectedSocket.Get());
	}

	// Disable Physics
	UXRReplicatedPhysicsComponent* XRPhysicsComponent = GetOwner()->FindComponentByClass<UXRReplicatedPhysicsComponent>();
	if (XRPhysicsComponent)
	{
		XRPhysicsComponent->SetSimulatePhysicsOnOwner(false);
	}

	GetOwner()->AttachToComponent(ConnectedSocket, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));

	// Update Physics State after snapping
	if (XRPhysicsComponent)
	{
		XRPhysicsComponent->Server_ForceUpdate();
	}

	PreviouslyConnectedSocket = ConnectedSocket;
	ConnectedSocket->RegisterConnection(this);

	HideAllHolograms();

	OnConnected.Broadcast(this, ConnectedSocket);
}

void UXRConnectorComponent::DeferredAttachToSocket()
{	
	// Physics
	UXRReplicatedPhysicsComponent* XRPhysicsComponent = GetOwner()->FindComponentByClass<UXRReplicatedPhysicsComponent>();
	if (XRPhysicsComponent)
	{
		XRPhysicsComponent->SetSimulatePhysicsOnOwner(false);
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(EstablishConnectionTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(EstablishConnectionTimer);
	}
	GetWorld()->GetTimerManager().SetTimer(EstablishConnectionTimer, this, &UXRConnectorComponent::AttachToSocket, EstablishConnectionTime, false);
	SetComponentTickEnabled(true);
}


void UXRConnectorComponent::DetachFromSocket()
{	
	if (!PreviouslyConnectedSocket.Get() || !GetOwner())
	{
		return;
	}

	SetComponentTickEnabled(false);

	PreviouslyConnectedSocket.Get()->DeregisterConnection(this);

	GetOwner()->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	OnDisconnected.Broadcast(this, PreviouslyConnectedSocket.Get());
	PreviouslyConnectedSocket = nullptr;

	// Physics
	UXRReplicatedPhysicsComponent* XRPhysicsComponent = GetOwner()->FindComponentByClass<UXRReplicatedPhysicsComponent>();
	if (XRPhysicsComponent)
	{
		XRPhysicsComponent->SetSimulatePhysicsOnOwner(true);
		XRPhysicsComponent->Server_ForceUpdate();
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Replication
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorComponent::OnRep_ConnectedSocket()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		return;
	}

	if (ConnectedSocket)
	{
		PreviouslyConnectedSocket = ConnectedSocket;
		if (EstablishConnectionTime <= 0.0f)
		{
			AttachToSocket();
		}
		else
		{
			DeferredAttachToSocket();
		}
	}
	else
	{
		DetachFromSocket();
	}
}

void UXRConnectorComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRConnectorComponent, ConnectedSocket);
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

		// Ensure closest socket is updated when setting the holograms visibility
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

		SetHologramState(OverlappedSocket, EXRHologramState::Hidden);
		OverlappedSockets.Remove(OverlappedSocket);
		ShowAllAvailableHolograms();
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Hologram
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRConnectorComponent::SetHologramState(UXRConnectorSocket* InSocket, EXRHologramState InState)
{
	if (!InSocket)
	{
		return;
	}

	// Don't show hologram for actively grabbed socket
	if (InState != EXRHologramState::Hidden)
	{
		TArray<UXRInteractionComponent*> ActiveInteractions;
		if (UXRToolsUtilityFunctions::IsActorInteractedWith(InSocket->GetOwner(), ActiveInteractions))
		{
			return;
		}
		if (!InSocket->IsHologramAllowed())
		{
			return;
		}
	}

	// Hologram already exists -> set State directly
	auto FoundHologram = AssignedHolograms.Find(InSocket);
	if (FoundHologram && FoundHologram->IsValid())
	{
		AActor* HologramActor = FoundHologram->Get();
		if (HologramActor && HologramActor->Implements<UXRHologramInterface>())
		{
			IXRHologramInterface::Execute_SetHologramState(HologramActor, InState);

			if (InState == EXRHologramState::Hidden)
			{
				AssignedHolograms.Remove(InSocket);
			}
		}
		return;
	}

	// Hologram needs to be spawned (Client only)
	ENetMode Mode = GetNetMode();
	if (Mode == NM_Client || Mode == NM_Standalone)
	{
		if (!HologramMesh)
		{
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
			IXRHologramInterface::Execute_InitHologram(SpawnedHologram, this, HologramMesh, HologramScale);
			IXRHologramInterface::Execute_SetHologramState(SpawnedHologram, InState);
		}
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

	// Set Hologram state based on distance to the closest overlapped Socket (iE. Highlighted vs. just visible)
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
			SetHologramState(OverlappedSocket.Get(), EXRHologramState::Hidden);
			continue;
		}

		if (OverlappedSocket == ClosestSocket)
		{
			SetHologramState(OverlappedSocket.Get(), EXRHologramState::Highlighted);
		}
		else
		{
			SetHologramState(OverlappedSocket.Get(), EXRHologramState::Visible);
		}
	}

	for (auto InvalidSocket : InvalidSockets)
	{
		OverlappedSockets.Remove(InvalidSocket);
	}
}

void UXRConnectorComponent::HideAllHolograms()
{
	for (const auto& Pair : AssignedHolograms)
	{
		const TWeakObjectPtr<AActor>& Hologram = Pair.Value;
		if (Hologram.IsValid() && Hologram->Implements<UXRHologramInterface>())
		{
			IXRHologramInterface::Execute_SetHologramState(Hologram.Get(), EXRHologramState::Hidden);
		}
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
