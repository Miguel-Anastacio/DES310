// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet_CPP.h"

#include "Components/StaticMeshComponent.h"

// Sets default values
ABullet_CPP::ABullet_CPP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Mesh"));
	
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetSimulatePhysics(true);
	//CubeMesh->SetEnableGravity(false);
	
	BulletCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Bullet Collision"));
	BulletCollision->SetupAttachment(BulletMesh);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh>BulletVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));
	if (BulletVisualAsset.Succeeded())
	{
		BulletMesh->SetStaticMesh(BulletVisualAsset.Object);
		//BulletMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		BulletMesh->SetWorldScale3D(FVector(0.0175, 0.26, 0.0275));

		BulletCollision->SetWorldScale3D(FVector(0.03, 0.4075, 0.0425));
		BulletCollision->SetRelativeLocation(FVector(0, 0, 1.425));
	}
}

// Called when the game starts or when spawned
void ABullet_CPP::BeginPlay()
{
	Super::BeginPlay();

	BulletMesh->SetEnableGravity(false);
	
	//BulletMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
	BulletMesh->SetWorldScale3D(FVector(0.0175, 0.26, 0.0275));

	BulletCollision->SetWorldScale3D(FVector(0.03, 0.4075, 0.0425));
	BulletCollision->SetRelativeLocation(FVector(0, 0, 1.425));
}

// Called every frame
void ABullet_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

