// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/GKMovementComponent.h"

#include "GoKart.generated.h"

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

UCLASS()
class CODINGEXPERIENCE5_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FSyncMove Move);

	UFUNCTION()
	void OnRep_ServerState();

private:

	void ClearAcknowledgedMoves(FSyncMove LastMove);

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USkeletalMeshComponent* KartMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* BoxCollision = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (DisplayName="Movement Component"))
	class UGKMovementComponent* GKMovementComponent = nullptr;

private:

	UPROPERTY(ReplicatedUsing=OnRep_ServerState)
	FSyncState ServerState;

	TArray<FSyncMove> UnacknowledgedMoves;
};
