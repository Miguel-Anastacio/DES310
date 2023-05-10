// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuShip.h"

// Sets default values
AMainMenuShip::AMainMenuShip()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* root = this->CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	root->Mobility = EComponentMobility::Type::Movable;
	this->SetRootComponent(root);

	Scarab = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scarab"));
	Scarab->SetupAttachment(RootComponent);

	Pyramid = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pyramid"));
	Pyramid->SetupAttachment(RootComponent);

	PyramidRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pyramid Ring"));
	PyramidRing->SetupAttachment(Pyramid);

	Longship = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Longship"));
	Longship->SetupAttachment(RootComponent);

	Fighter = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fighter"));
	Fighter->SetupAttachment(RootComponent);
}


// Called when the game starts or when spawned
void AMainMenuShip::BeginPlay()
{
	Super::BeginPlay();

	Scarab->SetVisibility(false);
	Pyramid->SetVisibility(false);
	PyramidRing->SetVisibility(false);
	Longship->SetVisibility(false);
	Fighter->SetVisibility(false);
	
	switch (FMath::RandRange(0,4))  {
	case 0:
		CurrentShip = Scarab;
		break;
	case 1:
		CurrentShip = Pyramid;
		PyramidRing->SetVisibility(true);
		break;
	case 2:
		CurrentShip = Longship;
		break;
	case 3:
		CurrentShip = Fighter;
		break;
	default:
		CurrentShip = Scarab;
		break;
	}

	CurrentShip->SetVisibility(true);
	StartingLocation = GetActorLocation();

}

// Called every frame
void AMainMenuShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timer += DeltaTime;

	SetActorLocation(StartingLocation + FVector(cos(timer * AxisSpeed.X + 53.1) * AxisRadius.X, cos(timer * AxisSpeed.Y + 3.1) * AxisRadius.Y, sin(timer * AxisSpeed.Z + 543.1) * AxisRadius.Z));
	//Mesh->SetActorRotation(FRotator(UKismetMathLibrary::FindLookAtRotation(AEnemyActor->GetActorLocation(), player->GetActorLocation())));

}

