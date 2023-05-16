// Fill out your copyright notice in the Description page of Project Settings.

/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/
#include "Planet.h"
#include "Vendor.h"
#include "SpaceshipCharacter.h"
#include "Animation/AnimInstanceProxy.h"


//Set up all the Actor Components
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = DummyRoot;
	
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point"));
	PointLight->SetupAttachment(RootComponent);
	
	PlanetMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Planet Mesh"));
	PlanetMeshComponent->SetWorldLocation(FVector(0, 0, 0));
	if (PlanetMeshComponent)
		PlanetMeshComponent->SetupAttachment(RootComponent);

	Tags.Add(TEXT("Planet"));

	RotationPerFrame = FRotator(0.0, 0.0, 0.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->SetUsingAbsoluteScale(false);
	CameraBoom->TargetArmLength = 3000.f;
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	CameraBoom->SetWorldRotation(FRotator(FMath::RandRange(300, 360), FMath::RandRange(0, 360), 0));

	FocusPlanetCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Planet Camera"));
	FocusPlanetCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FocusPlanetCamera->bUsePawnControlRotation = false;

	Line1 = FText::FromString("Planet A");
	Line2 = FText::FromString("You Are Maybe Here");
	Line3 = FText::FromString("Distance: 100 AU");

}


void APlanet::OnPlanetDestroyed(AActor* Act)
{
	if (VendorActor) // Properly Destroy Vendor alongside planet
		VendorActor->Destroy();
}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();

	this->OnDestroyed.AddDynamic(this, &APlanet::OnPlanetDestroyed);
	
	FTransform SpawnTransform;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnTransform.SetRotation(FQuat4d(0, 0, 0, 1.0f));
	SpawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
	SpawnTransform.SetLocation(GetActorLocation());

	//Make it so the camera and light scale with the actor
	CameraBoom->TargetArmLength = CameraBoom->TargetArmLength * this->GetActorScale().Length();
	PointLight->SourceRadius = this->GetActorScale().GetMax(); 



	//SetRandomQuest();
}

// Called every frame
void APlanet::Tick(float DeltaTime)
{
	// on every frame change rotationg for a smooth rotating actor
	PlanetMeshComponent->AddWorldRotation(RotationPerFrame.Quaternion(), false, 0, ETeleportType::None);
	
	Super::Tick(DeltaTime);

}


void APlanet::SetPlanetIconUI()
{
	//Set Appropriate Icon Depend on the Planet
	if (IsFirstPlanet)
	{
		Icon = AllIcons[0];
	}
	else
	{
		if (IsCheckpoint)
		{
			Icon = AllIcons[1];
		}
		else
		{
			Icon = AllIcons[2];
		}
	}
}