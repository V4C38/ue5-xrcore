#include "XRInteractionAxialMove.h"
#include "XRInteractorComponent.h"
#include "Net/UnrealNetwork.h"


UXRInteractionAxialMove::UXRInteractionAxialMove()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	LaserBehavior = EXRLaserBehavior::SnapMove;
}


void UXRInteractionAxialMove::BeginPlay()
{
	Super::BeginPlay();
	if (!GetOwner())
	{
		return;
	}
	switch (ObjectToMove)
	{
		case EAxialMoveTarget::OwningActor:
			RootTransform = GetOwner()->GetActorTransform();
			break;
		case EAxialMoveTarget::ThisComponent:
			RootTransform = GetComponentTransform();
			break;
	}
}

void UXRInteractionAxialMove::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!GetOwner() || GetActiveInteractors().Num() < 1)
	{
		return;
	} 

	FVector TargetWorldLocation = GetActiveInteractors()[0]->GetComponentLocation();
	FVector MoveDirection = {};

	switch (ObjectToMove)
	{
		case EAxialMoveTarget::OwningActor:
		{
			AActor* Owner = GetOwner();
			MoveDirection = GetActiveInteractors()[0]->GetComponentLocation() - Owner->GetActorLocation();
			Owner->SetActorLocation(RootTransform.GetLocation() + MoveDirection);
			break;
		}

		case EAxialMoveTarget::ThisComponent:
		{
			MoveDirection = GetActiveInteractors()[0]->GetComponentLocation() - this->GetComponentLocation();
			this->SetWorldLocation(RootTransform.GetLocation() + MoveDirection);
			break;
		}
	}






	/*


	FVector CurrentWorldLocation = TargetActor ? TargetActor->GetActorLocation() : TargetComponent->GetComponentLocation();
	FTransform CurrentWorldTransform = TargetActor ? TargetActor->GetActorTransform() : TargetComponent->GetComponentTransform();
	FVector TargetWorldLocation = GetActiveInteractor()->GetComponentLocation();

	// Apply Axis constraints in local space
	FVector TargetLocalLocation = CurrentWorldTransform.InverseTransformPosition(TargetWorldLocation);
	TargetLocalLocation.X = bConstrainX ? 0 : TargetLocalLocation.X;
	TargetLocalLocation.Y = bConstrainY ? 0 : TargetLocalLocation.Y;
	TargetLocalLocation.Z = bConstrainZ ? 0 : TargetLocalLocation.Z;
	TargetWorldLocation = CurrentWorldTransform.TransformPosition(TargetLocalLocation);

	// Distance Check
	switch (ObjectToMove)
	{
		case EAxialMoveTarget::OwningActor:
			if (FVector::Dist(TargetWorldLocation, RootTransform.GetLocation()) > DistanceLimit)
			{
				OnMovementLimitReached.Broadcast(this);
				return;
			}
			break;
		case EAxialMoveTarget::ThisComponent:
			FVector WorldTarget = GetOwner()->GetActorTransform().InverseTransformPosition(TargetWorldLocation);
			if (FVector::Dist(WorldTarget, RootTransform.GetLocation()) > DistanceLimit)
			{
				OnMovementLimitReached.Broadcast(this);
				return;
			}
			break;
	}

	// Move the target
	FVector NewLocation = MovementSpeed ? FMath::VInterpTo(CurrentWorldLocation, TargetWorldLocation, DeltaTime, MovementSpeed) : TargetWorldLocation;
	if (TargetActor)
	{
		TargetActor->SetActorLocation(NewLocation);
	}
	else if (TargetComponent)
	{
		TargetComponent->SetWorldLocation(NewLocation);
	}
	OnMovementUpdate.Broadcast(this, GetMovementProgress());

	*/
}

void UXRInteractionAxialMove::StartInteraction(UXRInteractorComponent* InInteractor)
{
	if (!InInteractor)
	{
		return;
	}
	Super::StartInteraction(InInteractor);
	InteractorOrigin = InInteractor->GetComponentTransform();
	SetComponentTickEnabled(true);
}

void UXRInteractionAxialMove::EndInteraction(UXRInteractorComponent* InInteractor)
{
	Super::EndInteraction(InInteractor);
	SetComponentTickEnabled(false);

	if (GetOwner())
	{
		return;
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		switch (ObjectToMove)
		{
			case EAxialMoveTarget::OwningActor:
				AxialMoveResult = GetOwner()->GetActorTransform();
				break;
			case EAxialMoveTarget::ThisComponent:
				AxialMoveResult = GetComponentTransform();
				break;
		}
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Utility
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
float UXRInteractionAxialMove::GetMovementProgress() const
{
	if (!GetOwner())
	{
		return -1.0f;
	}
	float OutProgress = 0.0f;
	FVector CurrentLocation = FVector::ZeroVector;
	switch (ObjectToMove)
	{
	case EAxialMoveTarget::OwningActor:
		CurrentLocation = GetOwner()->GetActorLocation();
		break;
	case EAxialMoveTarget::ThisComponent:
		CurrentLocation = GetRelativeLocation();
		break;
	}
	OutProgress = FVector::Dist(CurrentLocation, RootTransform.GetLocation()) / DistanceLimit;
	return FMath::Clamp(OutProgress, 0.0f, 1.0f);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// Replication
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
void UXRInteractionAxialMove::OnRep_AxialMoveResult()
{
	switch (ObjectToMove)
	{
		case EAxialMoveTarget::OwningActor:
			if (GetOwner())
			{
				GetOwner()->SetActorTransform(AxialMoveResult);
			}
			break;
		case EAxialMoveTarget::ThisComponent:
			SetRelativeTransform(AxialMoveResult);
			break;
	}
}

void UXRInteractionAxialMove::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRInteractionAxialMove, AxialMoveResult);
	DOREPLIFETIME(UXRInteractionAxialMove, RootTransform);
}