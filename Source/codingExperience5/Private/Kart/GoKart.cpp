// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKart.h"

#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/GKMovementComponent.h"
#include "Components/GKMovementRepComponent.h"

#include "DrawDebugHelpers.h"

AGoKart::AGoKart()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(FName("Box Collision"));
	RootComponent = BoxCollision;

	KartMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Mesh"));
	KartMesh->SetupAttachment(GetRootComponent());

	GKMovementComponent = CreateDefaultSubobject<UGKMovementComponent>(FName("Movement Component"));
	GKMovementReplicator = CreateDefaultSubobject<UGKMovementRepComponent>(FName("Movement Replicator"));

	NetUpdateFrequency = 25.f;
	SetReplicates(true);
}

void AGoKart::BeginPlay()
{
	Super::BeginPlay();
}

void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AGoKart::MoveRight);
}

void AGoKart::MoveForward(float Value) {

	GKMovementComponent->SetThrottle(Value);
}

void AGoKart::MoveRight(float Value) {

	GKMovementComponent->SetSteeringThrow(Value);
}

