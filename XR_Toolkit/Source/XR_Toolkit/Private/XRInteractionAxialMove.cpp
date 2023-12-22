#include "XRInteractionAxialMove.h"
#include "XRInteractorComponent.h"
#include "Net/UnrealNetwork.h"


UXRInteractionAxialMove::UXRInteractionAxialMove()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


void UXRInteractionAxialMove::BeginPlay()
{
	Super::BeginPlay();
}

void UXRInteractionAxialMove::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!GetOwner() || !GetActiveInteractor())
	{
		return;
	}

	AActor* Owner = GetOwner();
	FVector CurrentWorldLocation = FVector();
	FTransform CurrentWorldTransform = FTransform();
	FVector TargetWorldLocation = GetActiveInteractor()->GetComponentLocation();
	FVector TargetLocalLocation = FVector();  

	switch (ObjectToMove)
	{
		case EAxialMoveTarget::OwningActor:
			CurrentWorldLocation = Owner->GetActorLocation();
			CurrentWorldTransform = Owner->GetActorTransform();
			TargetLocalLocation = CurrentWorldTransform.InverseTransformPosition(TargetWorldLocation);
			// Apply local axis constraints
			TargetLocalLocation.X = bConstrainX ? 0 : TargetLocalLocation.X;
			TargetLocalLocation.Y = bConstrainY ? 0 : TargetLocalLocation.Y;
			TargetLocalLocation.Z = bConstrainZ ? 0 : TargetLocalLocation.Z;
			TargetWorldLocation = CurrentWorldTransform.TransformPosition(TargetLocalLocation);
			Owner->SetActorLocation(TargetWorldLocation);
			if (MovementSpeed)
			{
				Owner->SetActorLocation(FMath::VInterpTo(CurrentWorldLocation, TargetWorldLocation, DeltaTime, MovementSpeed));
			}
			else
			{
				Owner->SetActorLocation(TargetWorldLocation);
			}
			break;
		case EAxialMoveTarget::ThisComponent:
			CurrentWorldLocation = GetComponentLocation();
			CurrentWorldTransform = GetComponentTransform();
			TargetLocalLocation = CurrentWorldTransform.InverseTransformPosition(TargetWorldLocation);
			// Apply Axis constraints in local space for this component
			TargetLocalLocation.X = bConstrainX ? 0 : TargetLocalLocation.X;
			TargetLocalLocation.Y = bConstrainY ? 0 : TargetLocalLocation.Y;
			TargetLocalLocation.Z = bConstrainZ ? 0 : TargetLocalLocation.Z;
			TargetWorldLocation = CurrentWorldTransform.TransformPosition(TargetLocalLocation);
			if (MovementSpeed)
			{
				SetWorldLocation(FMath::VInterpTo(CurrentWorldLocation, TargetWorldLocation, DeltaTime, MovementSpeed));
			}
			else
			{
				SetWorldLocation(TargetWorldLocation);
			}
			break;
	}
}

void UXRInteractionAxialMove::StartInteraction(UXRInteractorComponent* InInteractor)
{
	Super::StartInteraction(InInteractor);
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
			SetWorldTransform(AxialMoveResult);
			break;
	}
}

void UXRInteractionAxialMove::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXRInteractionAxialMove, AxialMoveResult);
	DOREPLIFETIME(UXRInteractionAxialMove, Origin);
}