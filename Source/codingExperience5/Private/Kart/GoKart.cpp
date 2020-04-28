// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKart.h"

#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(FName("BoxCollision"));
	RootComponent = BoxCollision;

	KartMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Mesh"));
	KartMesh->SetupAttachment(GetRootComponent());

	Mass = 1000;
	MaxDrivingForce = 10000;
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;

	FVector Acceleration = Force / Mass;

	Velocity += Acceleration * DeltaTime;

	SetLocationFromVelocity(DeltaTime);
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

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AGoKart::MoveForward);
}

void AGoKart::MoveForward(float Value) {

	//Velocity = GetActorForwardVector() * 20 * Value;
	Throttle = Value;
}