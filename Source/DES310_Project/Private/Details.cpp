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
	
}

// Called every frame
void ADetails::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlanetMeshComponent->AddWorldRotation(RotationPerFrame);

}

