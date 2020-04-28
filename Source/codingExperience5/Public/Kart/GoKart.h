// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

UCLASS()
class CODINGEXPERIENCE5_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetLocationFromVelocity(float &DeltaTime);
	void SetRotation(float &DeltaTime);

	void MoveForward(float Value);
	void MoveRight(float Value);
	FVector GetAirResistence();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USkeletalMeshComponent* KartMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* BoxCollision = nullptr;

	// Mass in kg
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Mass;

	// Max force applied to car when max throttle in Newtons
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxDrivingForce;

	// How many degrees per second can the vehicle turn
	// Degrees / s
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxDegreesPerSecond;

	// Higher means more drag
	// Drag in Kg / m
	UPROPERTY(EditAnywhere, Category = "Movement")
	float DragCoefficient;

private:

	FVector Velocity;

	float SteeringThrow;
	float Throttle;
};
