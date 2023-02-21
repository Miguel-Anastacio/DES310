// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 14/02/23
//

#include "Planet.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Root = this->CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	Root->Mobility = EComponentMobility::Type::Movable;
	this->SetRootComponent(Root);

	
	PlanetMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Planet Mesh"));
	if (PlanetMeshComponent)
	{
		PlanetMeshComponent->SetupAttachment(RootComponent);
	}



	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->SetUsingAbsoluteScale(false);
	CameraBoom->TargetArmLength = 3000.f;
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level


	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Planet Camera"));
	Camera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();
	CameraBoom->SetWorldRotation(FRotator(FMath::RandRange(300,360), FMath::RandRange(0,360), 0));
	PlanetMeshComponent->SetStaticMesh(PlanetMeshes[FMath::RandRange(0, PlanetMeshes.Num()-1)]);
	PlanetMeshComponent->SetMaterial(0,PlanetMaterials[FMath::RandRange(0, PlanetMaterials.Num()-1)]);
	
	/*float RandomAngleHorizontal = FMath::RandRange(0,360);
	float RandomAngleVertical= FMath::RandRange(0,90);
	FVector Position(cos(RandomAngleHorizontal),sin(RandomAngleHorizontal),sin(RandomAngleVertical));
	Position *= 100;

	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation() + Position, GetActorLocation());

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%s Camera Position"), *Position.ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%s Planet Position"), *GetActorLocation().ToString()));

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%s Rotation"), *Rotation.ToString()));

	
	Camera->SetRelativeLocation(Position,false, 0, ETeleportType::None);
	Camera->SetWorldRotation(Rotation,false, 0, ETeleportType::None);*/
}

// Called every frame
void APlanet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if(PlanetMeshComponent)
		PlanetMeshComponent->AddWorldRotation(FRotator(0.0f, angle * DeltaTime * RotationRate, 0), false, 0, ETeleportType::None);

}

void APlanet::SetRandomMesh()
{
	PlanetMeshComponent->SetStaticMesh(PlanetMeshes[FMath::RandRange(0, PlanetMeshes.Num()-1)]);
	PlanetMeshComponent->SetMaterial(0,PlanetMaterials[FMath::RandRange(0, PlanetMaterials.Num()-1)]);
}

