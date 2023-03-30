#include "Enemy.h"
#include "Enemy.h"
#include "Enemy.h"
// Fill out your copyright notice in the Description page of Project Settings.
#include "Kismet/KismetMathLibrary.h"
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

	EnemyStats = CreateDefaultSubobject<UStatsComponent>(TEXT("Enemy Stats"));

	// declare trigger capsule
	/*TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger Capsule"));
	TriggerCapsule->SetupAttachment(RootComponent);
	TriggerCapsule->InitCapsuleSize(55.f, 96.0f);;
	TriggerCapsule->SetCollisionProfileName(TEXT("Trigger"));
	TriggerCapsule->SetGenerateOverlapEvents(true);*/

	//EnemyCube->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	EnemyCube->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);
	CubeMesh->SetEnableGravity(false);
	Health = InitialHealth;
	
}

void AEnemy::Attack()
{
	if (FireRate <= 0.f)
	{
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;
		FVector SpawnLocation = GetActorLocation() - (GetActorForwardVector() * BulletSpawnOffset);

		if(MyBullet)
			ABulletActor = GetWorld()->SpawnActor<ABullet_CPP>(MyBullet, SpawnLocation, GetActorRotation(), SpawnInfo);
		else
			GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Red, TEXT("My bullet is null"));
		if (ABulletActor)
		{
			ABulletActor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

			ABulletActor->SetActorLocation(GetActorLocation() - (GetActorForwardVector() * BulletSpawnOffset));
			ABulletActor->BulletMesh->SetPhysicsLinearVelocity(GetActorForwardVector() * BulletSpeed);

			FRotator Rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerLocation);

			ABulletActor->SetActorRotation(Rot);

			BulletsFired.Add(ABulletActor);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Red, TEXT("Problem with bullet"));
		}
		FireRate = 1.5;

	}
}

void AEnemy::ResetEnemy()
{
	Health = InitialHealth;
	for (int i = 0; i < BulletsFired.Num(); i++)
	{
		if (BulletsFired[i])
			BulletsFired[i]->Destroy();

	}
	BulletsFired.Empty();
}

void AEnemy::SetEnemyLevel(int playerLevel)
{
	int minLevel = playerLevel - MinLevelOffset;
	int maxLevel = playerLevel + MaxLevelOffset;
	if (minLevel < 1)
		minLevel = 1;

	if(maxLevel > MAX_LEVEL)
		maxLevel = MAX_LEVEL;
	int EnemyLevel = FMath::RandRange(minLevel, maxLevel);
	EnemyStats->SetStatsBasedOnLevel(EnemyLevel);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	FireRate -= DeltaTime;
	Super::Tick(DeltaTime);

}

void AEnemy::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABullet_CPP* BulletOBJ = Cast<ABullet_CPP>(OtherActor);

	if (BulletOBJ) 
	{
		EnemyStats->CurrentHullIntegrity -= EnemyStats->DamageTakenPerHit;
		BulletOBJ->Destroy();
		BulletOBJ = nullptr;
		float overflowDamage = 0.f;
		if (EnemyStats->CurrentShields > 0)
		{
			EnemyStats->CurrentShields -= EnemyStats->DamageTakenPerHit;
			if (EnemyStats->CurrentShields < 0)
			{
				overflowDamage = abs(EnemyStats->CurrentShields);
				EnemyStats->CurrentHullIntegrity -= overflowDamage;
				EnemyStats->CurrentShields = 0;
			}
		}
		else
		{
			EnemyStats->CurrentHullIntegrity -= EnemyStats->DamageTakenPerHit;
			if (EnemyStats->CurrentHullIntegrity < 0)
				EnemyStats->CurrentHullIntegrity = 0;
		}


		if(EnemyStats->CurrentHullIntegrity <= 0)
		{
			this->Destroy();
		}
	}
}
