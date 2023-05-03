// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuSpaceStation.h"

// Sets default values
AMainMenuSpaceStation::AMainMenuSpaceStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->Mobility = EComponentMobility::Type::Movable;
	Root->SetupAttachment(GetRootComponent());
	//this->SetRootComponent(root);
	//RootComponent = Root;
	
	EmptyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Empty Transform"));
	EmptyMesh->SetupAttachment(Root);

	Mesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tall Station"));
	Mesh1->SetupAttachment(EmptyMesh);

	Mesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wide Station"));
	Mesh2->SetupAttachment(EmptyMesh);

}

// Called when the game starts or when spawned
void AMainMenuSpaceStation::BeginPlay()
{
	Super::BeginPlay();

	if(RandomizeDirection)
	{
		if(FMath::RandBool())
		{
			Rotations *= -1;
		}
	}

	if(FMath::RandBool())
	{
		Mesh1->SetVisibility(false);
		Mesh2->SetVisibility(true);
	}
	else
	{
		Mesh1->SetVisibility(true);
		Mesh2->SetVisibility(false);
	}
}

// Called every frame
void AMainMenuSpaceStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(Rotations);

}

