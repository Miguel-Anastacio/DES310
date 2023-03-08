// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Enemy Cube Mesh"));
	
	CubeMesh->SetupAttachment(RootComponent);
	CubeMesh->SetSimulatePhysics(false);
	//CubeMesh->SetEnableGravity(false);
	
	EnemyCube = CreateDefaultSubobject<UBoxComponent>(TEXT("Enemy Cube Collision"));
	EnemyCube->SetupAttachment(CubeMesh);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));
	if (CubeVisualAsset.Succeeded())
	{
		CubeMesh->SetStaticMesh(CubeVisualAsset.Object);
		CubeMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		CubeMesh->SetWorldScale3D(FVector(0.8f));
	}

	// declare trigger capsule
	/*TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger Capsule"));
	TriggerCapsule->SetupAttachment(RootComponent);
	TriggerCapsule->InitCapsuleSize(55.f, 96.0f);;
	TriggerCapsule->SetCollisionProfileName(TEXT("Trigger"));
	TriggerCapsule->SetGenerateOverlapEvents(true);*/

	//EnemyCube->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);

	HitsReceived = 0;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	EnemyCube->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);
	CubeMesh->SetEnableGravity(false);
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	ABullet_CPP* BulletOBJ = Cast<ABullet_CPP>(OtherActor);

	if (BulletOBJ) 
	{

		HitsReceived++;
		BulletOBJ->Destroy();
		BulletOBJ = nullptr;

		if(HitsReceived == 3)
		{
			this->Destroy();
		}
	}
}