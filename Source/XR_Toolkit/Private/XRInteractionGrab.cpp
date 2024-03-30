#include "XRInteractionGrab.h"
#include "XRReplicatedPhysicsComponent.h"
#include "XRInteractorComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Net/UnrealNetwork.h"


UXRInteractionGrab::UXRInteractionGrab()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
	SetIsReplicated(true);
	LaserBehavior = EXRLaserBehavior::Enabled;
	MultiInteractorBehavior = EXRMultiInteractorBehavior::Enabled;
}

void UXRInteractionGrab::BeginPlay()
{
	Super::BeginPlay();
	if (bEnablePhysics)
	{
		InitializePhysics();
	}
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Interaction Events
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionGrab::StartInteraction(UXRInteractorComponent* InInteractor)
{
	Super::StartInteraction(InInteractor);
	if (bEnablePhysics)
	{
		PhysicsGrab(InInteractor);
	}
	else
	{
		AttachOwningActorToXRInteractor(InInteractor);
	}
}

void UXRInteractionGrab::EndInteraction(UXRInteractorComponent* InInteractor)
{
	Super::EndInteraction(InInteractor);
	if (bEnablePhysics)
	{
		PhysicsUngrab(InInteractor);
	}
	else
	{
		DetachOwningActorFromXRInteractor();
	}

	// Update GrabActorsLocation for LateJoiners when Physics is disabled
	if (!bEnablePhysics)
	{
		AActor* Owner = GetOwner();
		if (Owner)
		{
			if (Owner->HasAuthority())
			{
				Server_UpdateGrabActorTransform();
			}
		}
	}
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Grab functions
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

void UXRInteractionGrab::AttachOwningActorToXRInteractor(UXRInteractorComponent* InInteractor)
{
	if (InInteractor)
	{
		FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);
		GetOwner()->AttachToComponent(InInteractor, Rules);
	}
}

void UXRInteractionGrab::DetachOwningActorFromXRInteractor()
{
	FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
	GetOwner()->DetachFromActor(Rules);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Physics
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

bool UXRInteractionGrab::HasPhysicsEnabled() const
{
	return bEnablePhysics;
}

void UXRInteractionGrab::PhysicsGrab(UXRInteractorComponent* InInteractor)
{
	if (InInteractor)
	{
		if (GetOwner()->HasAuthority())
		{
			XRReplicatedPhysicsComponent->SetInteractedWith(true);
		}

		TArray<UMeshComponent*> MeshComponents = XRReplicatedPhysicsComponent->GetRegisteredMeshComponents();
		if (MeshComponents.Num() > 0)
		{
			UMeshComponent* PhysicsEnabledMesh = MeshComponents[0];
			UPhysicsConstraintComponent* ActivePhysicsConstraint = InInteractor->GetPhysicsConstraint();

			if (ActivePhysicsConstraint && PhysicsEnabledMesh)
			{
				ActivePhysicsConstraint->SetConstrainedComponents(PhysicsEnabledMesh, "", InInteractor, "");
			}
		}
	}
}

void UXRInteractionGrab::PhysicsUngrab(UXRInteractorComponent* InInteractor)
{
	if (InInteractor)
	{
		UPhysicsConstraintComponent* ActivePhysicsConstraint = InInteractor->GetPhysicsConstraint();
		if (ActivePhysicsConstraint)
		{
			ActivePhysicsConstraint->BreakConstraint();
		}
	}
	if (GetOwner()->HasAuthority() && !IsInteractedWith())
	{
		XRReplicatedPhysicsComponent->SetInteractedWith(false);
	}
}

void UXRInteractionGrab::InitializePhysics()
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		UXRReplicatedPhysicsComponent* FoundXRPhysicsComponent = Owner->FindComponentByClass<UXRReplicatedPhysicsComponent>();
		if (FoundXRPhysicsComponent)
		{
			XRReplicatedPhysicsComponent = FoundXRPhysicsComponent;
			return;
		}
		else
		{
			XRReplicatedPhysicsComponent = NewObject<UXRReplicatedPhysicsComponent>(this->GetOwner());
			if (XRReplicatedPhysicsComponent)
			{
				XRReplicatedPhysicsComponent->RegisterComponent();
				XRReplicatedPhysicsComponent->Activate();
				XRReplicatedPhysicsComponent->RegisterPhysicsMeshComponents(PhysicsTag);
			}
		}
	}
}

UXRReplicatedPhysicsComponent* UXRInteractionGrab::GetPhysicsReplicationComponent()
{
	return XRReplicatedPhysicsComponent;
}



// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Replication
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionGrab::OnRep_GrabActorTransform()
{
	if (!bEnablePhysics)
	{
		AActor* Owner = GetOwner();
		if (Owner)
		{
			Owner->SetActorTransform(GrabActorTransform);
		}
	}
}

void UXRInteractionGrab::Server_UpdateGrabActorTransform_Implementation()
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		GrabActorTransform = Owner->GetActorTransform();
	}
}

void UXRInteractionGrab::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRInteractionGrab, GrabActorTransform);
}