// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GKMovementComponent.h"
#include "GKMovementRepComponent.generated.h"

USTRUCT(BlueprintType)
struct FSyncState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FSyncMove LastMove;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CODINGEXPERIENCE5_API UGKMovementRepComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UGKMovementRepComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FSyncMove Move);

	UFUNCTION()
	void OnRep_ServerState();

private:

	void ClearAcknowledgedMoves(FSyncMove LastMove);

private:

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FSyncState ServerState;

	TArray<FSyncMove> UnacknowledgedMoves;

	class UGKMovementComponent* GKMovementComponent = nullptr;

};
