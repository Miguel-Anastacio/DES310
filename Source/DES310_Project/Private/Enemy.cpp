#include "Enemy.h"
#include "Enemy.h"
#include "Enemy.h"
// Fill out your copyright notice in the Description page of Project Settings.
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "SpaceshipCharacter.h"


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

	for(int i = 0; i < BulletsFired.Num(); i++)
	{
		if(BulletsFired[i])
		{
			if(BulletsFired[i]->TimeAlive > 2.f)
			{
				BulletsFired[i]->Destroy();
			}
		}
	}
	
	if (FireRateTimer <= 0.f)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;

		FTransform BulletTransform;
		BulletTransform.SetTranslation(GetActorLocation() - (GetActorForwardVector() * BulletSpawnOffset));
		BulletTransform.SetRotation(GetActorForwardVector().ToOrientationQuat());
		BulletTransform.SetScale3D(GetTransform().GetScale3D());

		if(MyBullet)
			ABulletActor = GetWorld()->SpawnActor<ABullet_CPP>(MyBullet, BulletTransform, SpawnInfo);
		else
			GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Red, TEXT("My bullet is null"));
		if (ABulletActor)
		{
			ABulletActor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

			FVector ForwardVector = FVector(-1,0,0);
			FVector RightVector = FVector(0,1,0);
			FVector UpVector = FVector(0,0,1);

			FRotator Rotation = GetActorRotation();
	
			RightVector = Rotation.RotateVector(RightVector);
			UpVector = Rotation.RotateVector(UpVector);
			ForwardVector = Rotation.RotateVector(ForwardVector);

			FVector NorthVector = ForwardVector.RotateAngleAxis(BulletAngleRange,UpVector);
			FVector SouthVector = ForwardVector.RotateAngleAxis(-BulletAngleRange,UpVector);
			FVector WestVector = ForwardVector.RotateAngleAxis(BulletAngleRange,RightVector);
			FVector EastVector = ForwardVector.RotateAngleAxis(-BulletAngleRange,RightVector);

			/*DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + NorthVector * 1000, FColor::Emerald, false, 20, 0, 10);
			DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + SouthVector * 1000, FColor::Emerald, false, 20, 0, 10);
			DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + WestVector * 1000, FColor::Emerald, false, 20, 0, 10);
			DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + EastVector * 1000, FColor::Emerald, false, 20, 0, 10);*/
			
			float XPercent = FMath::RandRange(0.f,1.f);
			float YPercent = FMath::RandRange(0.f,1.f);
		
			FVector HorizontalVector = FMath::Lerp(EastVector,WestVector,XPercent);
			FVector VerticalVector = FMath::Lerp(NorthVector,SouthVector,YPercent);
			FVector Direction = FMath::Lerp(HorizontalVector,VerticalVector,0.5);
			//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Direction * 1000, FColor::Red, false, 20, 0, 10);
			ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			
			ABulletActor->ProjectileMovement->Activate(true);
			ABulletActor->ProjectileMovement->InitialSpeed = BulletSpeed;
			ABulletActor->ProjectileMovement->MaxSpeed = BulletSpeed;
			ABulletActor->ProjectileMovement->HomingTargetComponent = player->GetRootComponent();
			ABulletActor->ProjectileMovement->HomingAccelerationMagnitude = BulletSpeed * HomingStrength;
			ABulletActor->ProjectileMovement->bIsHomingProjectile = true;
			ABulletActor->ProjectileMovement->Velocity = Direction * BulletSpeed;

			
			BulletsFired.Add(ABulletActor);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Red, TEXT("Problem with bullet"));
		}
		FireRateTimer = FireRate;

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

	this->Destroy();
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
	FireRateTimer -= DeltaTime;
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
