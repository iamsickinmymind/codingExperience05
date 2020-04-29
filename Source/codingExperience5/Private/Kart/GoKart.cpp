// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKart.h"

#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"

AGoKart::AGoKart()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(FName("BoxCollision"));
	RootComponent = BoxCollision;

	KartMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Mesh"));
	KartMesh->SetupAttachment(GetRootComponent());

	Mass = 1000;
	MaxDrivingForce = 10000;
	MinimumTurningRadius = 10;
	DragCoefficient = 15;
	RollingResistenceCoefficient = 0.015;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;

	Force += GetAirResistence();
	Force += GetRollingResistence();

	FVector Acceleration = Force / Mass;

	Velocity += Acceleration * DeltaTime;

	SetRotation(DeltaTime);

	SetLocationFromVelocity(DeltaTime);
}

void AGoKart::SetRotation(float &DeltaTime)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinimumTurningRadius * SteeringThrow;
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);

	Velocity = RotationDelta.RotateVector(Velocity);

	AddActorWorldRotation(RotationDelta, true);
}

void AGoKart::SetLocationFromVelocity(float &DeltaTime)
{
	FVector Delta = Velocity * 100 * DeltaTime;

	FHitResult HitResult;
	AddActorWorldOffset(Delta, true, &HitResult);

	if (HitResult.bBlockingHit) {

		Velocity = FVector::ZeroVector;
	}
}

FVector AGoKart::GetAirResistence() {

	return -Velocity.GetSafeNormal() * (Velocity.SizeSquared()) * DragCoefficient;
}

FVector AGoKart::GetRollingResistence() {

	// Convert from cm to m and to positive number
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	float NormalForce = Mass * AccelerationDueToGravity;

	return -Velocity.GetSafeNormal() * RollingResistenceCoefficient * NormalForce;
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AGoKart::Server_MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AGoKart::Server_MoveRight);
}

void AGoKart::Server_MoveForward_Implementation(float Value) {

	//Velocity = GetActorForwardVector() * 20 * Value;
	Throttle = Value;
}

bool AGoKart::Server_MoveForward_Validate(float Value) {

	return FMath::Abs(Value) <= 1;
}

void AGoKart::Server_MoveRight_Implementation(float Value ) {

	SteeringThrow = Value;
}

bool AGoKart::Server_MoveRight_Validate(float Value) {

	return FMath::Abs(Value) <= 1;
}