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

	Mesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fighter"));
	Mesh1->SetupAttachment(RootComponent);

	Mesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Heavy"));
	Mesh2->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMainMenuShip::BeginPlay()
{
	Super::BeginPlay();

	if (FMath::RandBool())
	{
		Mesh1->SetVisibility(false);
		Mesh2->SetVisibility(true);
	}
	else
	{
		Mesh1->SetVisibility(true);
		Mesh2->SetVisibility(false);
	}
	
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

