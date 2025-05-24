#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "UObject/Interface.h"

#include "Connections/XRConnectorTypes.h"

#include "XRConnectorHologram.generated.h"

class UXRConnectorComponent;

// ================================================================================================================================================================
// Static mesh (or other visual) to indicate available connection sockets for an XRConnector with matching ID
// ================================================================================================================================================================
UCLASS()
class XR_TOOLKIT_API AXRConnectorHologram : public AActor, public IXRHologramInterface
{
	GENERATED_BODY()
	
public:	
	AXRConnectorHologram();

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// API
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void ShowHologram_Implementation(UXRConnectorComponent* InConnector, UStaticMesh* InHologramMesh, float InHologramMeshScale) override;
    virtual void HideHologram_Implementation(UXRConnectorComponent* InConnector) override;
    virtual void SetHologramEnabled_Implementation(UXRConnectorComponent* InConnector, bool InState) override;


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void RemoveHologram();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Config
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------
	/*
	* Time in seconds after which a hidden Hologram will be destroyed.
	* Note: if this becomes a performance issue in the future, an ObjectPool can be implemented instead.
	*/
	UPROPERTY(Editanywhere, Category = "XRConnector", meta = (ClampMin = "0.0"))
	float DestroyAfterHiddenSeconds = 10.0f;

	/*
	* Override the scale of the StaticMesh in the hologram. This is also set by the connector when the hologram is requested.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "XRConnector", meta = (ClampMin = "0.0"))
	float HologramMeshScale = 1.0f;

private:

	FTimerHandle DestroyActorTimer;

};
