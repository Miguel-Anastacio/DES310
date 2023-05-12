// Fill out your copyright notice in the Description page of Project Settings.


#include "Details.h"

// Sets default values
ADetails::ADetails()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point"));
	PointLight->SetupAttachment(RootComponent);
	
	PlanetMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Planet Mesh"));
	PlanetMeshComponent->SetupAttachment(RootComponent);
	
	
}

// Called when the game starts or when spawned
void ADetails::BeginPlay()
{
	Super::BeginPlay();
	StartingLocation = GetActorLocation();
	AxisRadius.X = FMath::RandRange(10.0f, 100.0f);
	AxisRadius.Y = FMath::RandRange(10.0f, 100.0f);
	AxisRadius.Z = FMath::RandRange(10.0f, 100.0f);

	RotationPerFrame.Yaw = FMath::RandRange(-10.f, 10.f);

	AxisSpeed.X = FMath::RandRange(0.5f, 1.5f);
	AxisSpeed.Y = FMath::RandRange(0.5f, 1.5f);
	AxisSpeed.Z = FMath::RandRange(0.5f, 1.5f);

	AxisRadius *= GetActorScale3D();
}

// Called every frame
void ADetails::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlanetMeshComponent->AddWorldRotation(RotationPerFrame * DeltaTime);
	timer += DeltaTime;
	SetActorLocation(StartingLocation + FVector(cos(timer * AxisSpeed.X + 53.1) * AxisRadius.X, cos(timer * AxisSpeed.Y + 3.1) * AxisRadius.Y, sin(timer * AxisSpeed.Z + 543.1) * AxisRadius.Z));
}

