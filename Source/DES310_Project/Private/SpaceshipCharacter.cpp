// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/

#include "SpaceshipCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "InventoryComponent.h"
#include "RandomEventsComponent.h"
#include "Planet.h"
#include "RouteExample.h"
#include "StatsComponent.h"
#include "Bullet_CPP.h"
#include "AbilityComponent.h"

// Sets default values
ASpaceshipCharacter::ASpaceshipCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(84.f, 96.0f);

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 2000.f;
	CameraBoom->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// messing around with lag
	CameraBoom->bEnableCameraLag = true;

	// create camera component
	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// declare trigger capsule
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	TriggerBox->InitBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetupAttachment(GetMesh());

	// declare static player mesh
	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	PlayerMesh->SetupAttachment(RootComponent);

	DeflectionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Deflection Mesh"));
	DeflectionMesh->SetupAttachment(RootComponent);

	DeflectionTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Deflection Trigger Box"));
	DeflectionTriggerBox->InitBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	DeflectionTriggerBox->SetCollisionProfileName(TEXT("Deflection Trigger"));
	DeflectionTriggerBox->SetupAttachment(DeflectionMesh);

	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield Mesh"));
	ShieldMesh->SetupAttachment(RootComponent);
	
	/*ShieldTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Shield Trigger Box"));
	ShieldTriggerBox->InitBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	ShieldTriggerBox->SetCollisionProfileName(TEXT("Shield Trigger"));
	ShieldTriggerBox->SetupAttachment(ShieldMesh);*/
	
	// create player inventory
	PlayerInventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	GetCharacterMovement()->GravityScale = 0.0f;

	StatsPlayerComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats"));

	AbilitiesComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("Abilities"));
}

void ASpaceshipCharacter::Attack(float DeltaTime, AEnemy* Enemy)
{
	CurrentTarget = Enemy->GetRootComponent();
	isAttacking = true;
	
	for(int i = 0; i < Bullets.Num(); i++) // TODO May be better to have the bullet handle its own destruction
	{
		if(Bullets[i])
		{
			if(Bullets[i]->TimeAlive > 2.f)
			{
				Bullets[i]->Destroy();
			}
		}
	}
	
	FireRateTimer -= GetWorld()->DeltaTimeSeconds;

	if (FireRateTimer <= 0.f && IsValid(Enemy))
	{
		if(AudioManager)
			if(AudioManager->ShootSoundComponent)
				AudioManager->ShootSoundComponent->Play();
			else
				GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Red, TEXT("Sound Cue"));
		else
			GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Red, TEXT("Audio Manager"));
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;

		FTransform BulletTransform;
		BulletTransform.SetTranslation(GetActorLocation() - (GetActorForwardVector()* BulletSpawnOffset));
		BulletTransform.SetRotation(GetActorForwardVector().ToOrientationQuat());
		BulletTransform.SetScale3D(GetTransform().GetScale3D());
		
		ABullet_CPP* ABulletActor = GetWorld()->SpawnActor<ABullet_CPP>(PlayerBulletBP, BulletTransform, SpawnInfo);
		ABulletActor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

		//ABulletActor->BulletMesh->SetPhysicsLinearVelocity(player->GetActorForwardVector()*-ppVec.X);

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

		/*
		DrawDebugLine(GetWorld(), player->GetActorLocation(), player->GetActorLocation() + NorthVector * 1000, FColor::Emerald, false, 20, 0, 10);
		DrawDebugLine(GetWorld(), player->GetActorLocation(), player->GetActorLocation() + SouthVector * 1000, FColor::Emerald, false, 20, 0, 10);
		DrawDebugLine(GetWorld(), player->GetActorLocation(), player->GetActorLocation() + WestVector * 1000, FColor::Emerald, false, 20, 0, 10);
		DrawDebugLine(GetWorld(), player->GetActorLocation(), player->GetActorLocation() + EastVector * 1000, FColor::Emerald, false, 20, 0, 10);
		*/


		float XPercent = FMath::RandRange(0.f,1.f);
		float YPercent = FMath::RandRange(0.f,1.f);
		
		FVector HorizontalVector = FMath::Lerp(EastVector,WestVector,XPercent);
		FVector VerticalVector = FMath::Lerp(NorthVector,SouthVector,YPercent);
		FVector Direction = FMath::Lerp(HorizontalVector,VerticalVector,0.5);

		//DrawDebugLine(GetWorld(), player->GetActorLocation(), player->GetActorLocation() + Direction * 1000, FColor::Red, false, 20, 0, 10);
		ABulletActor->ProjectileMovement->Activate(true);
		ABulletActor->ProjectileMovement->InitialSpeed = BulletSpeed;
		ABulletActor->ProjectileMovement->MaxSpeed = BulletSpeed;
		ABulletActor->ProjectileMovement->HomingTargetComponent = Enemy->GetRootComponent();
		ABulletActor->ProjectileMovement->HomingAccelerationMagnitude = BulletSpeed * HomingStrength;
		ABulletActor->ProjectileMovement->bIsHomingProjectile = true;
		ABulletActor->ProjectileMovement->Velocity = Direction * BulletSpeed;
		
		//ABulletActor->BulletMesh->SetPhysicsLinearVelocity(Direction * ppVec.X); // In the Documentation it says its better to use AddForce
		//ABulletActor->BulletMesh->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(FVector(0,0,0), ABulletActor->BulletMesh->GetComponentVelocity()));
		//UKismetMathLibrary::FindLookAtRotation(FVector(0,0,0), ABulletActor->BulletMesh->GetComponentVelocity()) May Be A Better way to make the bullet face the velocity
		
		Bullets.Add(ABulletActor);

		FireRateTimer = FireRate;
	}
}

void ASpaceshipCharacter::ResetCombat()
{

	//Health = InitialHealth;
	for (int i = 0; i < Bullets.Num(); i++)
	{
		if (Bullets[i])
			Bullets[i]->Destroy();

	}
	Bullets.Empty();

	//AudioManager->AlarmSoundComponent->Stop();
	isAttacking = false;
	AbilitiesComponent->DisableBulletDeflector();
	AbilitiesComponent->DisableSpecialAttack();
}

// Called when the game starts or when spawned
void ASpaceshipCharacter::BeginPlay()
{
	Super::BeginPlay();
	// set controller parameters
	PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
		PlayerController->bShowMouseCursor = true;

	//DeflectionMesh->SetVisibility(false);
	
	TargetLocation = GetActorLocation();
	ShieldMesh->SetVisibility(false);

	CompleteQuestDelegate.AddUniqueDynamic(this, &ASpaceshipCharacter::CompleteQuest);
	StartQuestDelegate.AddUniqueDynamic(this, &ASpaceshipCharacter::StartQuest);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASpaceshipCharacter::OnOverlapBegin);
	DeflectionTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASpaceshipCharacter::OnDeflectOverlapBegin);
	

	StepSpeed = (MaxMovementSpeed - MinMovementSpeed) / Steps;
	
	if (!StatsPlayerComponent)
	{
		GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Red, TEXT("Stats Problem"));
	}

	if(!StatsPlayerComponent->AttemptLoad())
	{
		GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Red, TEXT("Load Fail"));
		ApplyInventoryToStats();
		StatsPlayerComponent->UpdateCurrentStats(StatsPlayerComponent->HullIntegrity, StatsPlayerComponent->Shields);
	}

	
	DeflectionMesh->SetCollisionEnabled( ECollisionEnabled::NoCollision);
	DeflectionMesh->SetVisibility(false);
	
	DeflectionTriggerBox->SetActive(false);
	DeflectionTriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DeflectionTriggerBox->SetVisibility(false);

	ShieldMesh->SetVisibility(false);
	ShieldMesh->SetCollisionEnabled( ECollisionEnabled::NoCollision);

	FireRate = DefaultFireRate;

}

void ASpaceshipCharacter::SetStatsBasedOnClass()
{
	StatsPlayerComponent->InitAllBaseStats(PlayerShip.Hull, PlayerShip.Speed, PlayerShip.Shield, PlayerShip.AttackPower);
	ApplyInventoryToStats();
	StatsPlayerComponent->UpdateCurrentStats(0, 0);

	// set faction as well
	switch (PlayerShip.Type)
	{
	case FIGHTER_EGYPTIAN: PlayerFaction = false;
		break;
	case HEAVY_EGYPTIAN: PlayerFaction = false;
		break;
	case FIGHTER_VIKING: PlayerFaction = true;
		break;
	case HEAVY_VIKING: PlayerFaction = true;
		break;
	default:
		break;
	}
}

void ASpaceshipCharacter::ApplyInventoryToStats()
{
	ApplyItemToStats(PlayerInventoryComponent->GetEquippedShield());
	ApplyItemToStats(PlayerInventoryComponent->GetEquippedBlaster());
	ApplyItemToStats(PlayerInventoryComponent->GetEquippedHull());
	ApplyItemToStats(PlayerInventoryComponent->GetEquippedEngine());
}

void ASpaceshipCharacter::ApplyItemToStats(UItem* item)
{
	
	if(item)
	{
		StatsPlayerComponent->Shields = item->Modifiers.ShieldBonus * StatsPlayerComponent->BaseShields;
		StatsPlayerComponent->Speed = item->Modifiers.SpeedBonus * StatsPlayerComponent->BaseSpeed;
		StatsPlayerComponent->HullIntegrity = item->Modifiers.HealthBonus * StatsPlayerComponent->BaseHullIntegrity;
		StatsPlayerComponent->ATKPower = item->Modifiers.DamageBonus * StatsPlayerComponent->BaseATKPower;
		StatsPlayerComponent->UpdateCurrentStats(StatsPlayerComponent->HullIntegrity, StatsPlayerComponent->Shields);
	}
}

void ASpaceshipCharacter::UpdatePlayerStats(int xpGained)
{
	StatsPlayerComponent->XPSystem(xpGained);
	ApplyInventoryToStats();
	StatsPlayerComponent->UpdateCurrentStats(StatsPlayerComponent->HullIntegrity, StatsPlayerComponent->Shields);
}

// Called every frame
void ASpaceshipCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentFov = FMath::Clamp(90 + ((MovementSpeed - MinMovementSpeed) / MaxMovementSpeed) * 50, 90 , 140);
	TopDownCamera->SetFieldOfView(FMath::Lerp(TopDownCamera->FieldOfView, CurrentFov, DeltaTime));
	
	/*
	if(isFireRate)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Fire Rate Increased"));

		FireRateCharge -= DeltaTime;
		if(FireRateCharge < 0)
		{
			isFireRate = false;
		}
	}
	
	if(isShielding)
	{
		shieldingCharge -= DeltaTime;
		if(shieldingCharge < 0)
		{
			isShielding = false;
			ShieldMesh->SetVisibility(isShielding);
			ShieldMesh->SetActive(isShielding);
			ShieldMesh->SetCollisionEnabled( ECollisionEnabled::NoCollision);
		}
	}
	*/
	
	if(isDeflecting)
	{
		DeflectionMesh->AddRelativeRotation(FRotator(0,0,DeltaTime * 200));
		/*
		if(DeflectionTimer > DeflectionLength)
		{
			isDeflecting = false;

			DeflectionMesh->SetActive(false);
			DeflectionMesh->SetCollisionEnabled( ECollisionEnabled::NoCollision);
			DeflectionMesh->SetVisibility(false);
	
			DeflectionTriggerBox->SetActive(false);
			DeflectionTriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			DeflectionTriggerBox->SetVisibility(false);

			DeflectionTimer = 0;
			
		}*/
	}
}

// Called to bind functionality to input
void ASpaceshipCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// bind action
	//PlayerInputComponent->BindAction("Mouse Click", IE_Pressed, this, &ASpaceshipCharacter::MouseClick);
	PlayerInputComponent->BindAction("Reset Game", IE_Pressed, this, &ASpaceshipCharacter::ResetGame);

	//PlayerInputComponent->BindAction("Shield", IE_Pressed, this, &ASpaceshipCharacter::ShieldPressed);
	//PlayerInputComponent->BindAction("Shield", IE_Released, this, &ASpaceshipCharacter::ShieldReleased);
	
	PlayerInputComponent->BindAction("Deflect", IE_Pressed, this, &ASpaceshipCharacter::DeflectPressed);

	//PlayerInputComponent->BindAction("Fire Rate Increase", IE_Pressed, this, &ASpaceshipCharacter::FireRatePressed);
	//PlayerInputComponent->BindAction("Fire Rate Increase", IE_Released, this, &ASpaceshipCharacter::FireRateReleased);

	PlayerInputComponent->BindAction("Speed Up", IE_Pressed, this, &ASpaceshipCharacter::SpeedUp);
	PlayerInputComponent->BindAction("Speed Up", IE_Released, this, &ASpaceshipCharacter::SpeedDown);

	
}

void ASpaceshipCharacter::MouseClick()
{
	// only do this if the state is selecting
	if (IsInSelectScreen)
	{
		Selected = true;
	}
}

void ASpaceshipCharacter::ResetGame()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void ASpaceshipCharacter::ShieldPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,	TEXT("Shielding"));
	
	if(shieldingCharge <= 0 || !isAttacking)
		return;
	
	isShielding = true;

	ShieldMesh->SetVisibility(isShielding);
	ShieldMesh->SetActive(isShielding);
	ShieldMesh->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics);

}

void ASpaceshipCharacter::ShieldReleased()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,	TEXT("Not Shielding"));
	isShielding = false;
	ShieldMesh->SetVisibility(isShielding);
	ShieldMesh->SetActive(isShielding);
	ShieldMesh->SetCollisionEnabled( ECollisionEnabled::NoCollision);
}

void ASpaceshipCharacter::DeflectPressed()
{

	if (IsInSelectScreen)
	{
		Selected = true;
	}
	/*
	if(deflectCharges <= 0 || isDeflecting || !isAttacking)
		return;
	
	deflectCharges--;
	isDeflecting = true;

	DeflectionMesh->SetActive(true);
	DeflectionMesh->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics);
	DeflectionMesh->SetVisibility(true);
	
	DeflectionTriggerBox->SetActive(true);
	DeflectionTriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DeflectionTriggerBox->SetVisibility(true);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Deflection Enabled"));*/


}

void ASpaceshipCharacter::FireRatePressed()
{

	if(FireRateCharge < 0 || !isAttacking)
		return;

	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("Fire Rate Increased"));
	
	FireRate /= 2;

	isFireRate = true;
}

void ASpaceshipCharacter::FireRateReleased()
{
	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("Fire Rate Decreased"));

	FireRate *= 2; // Feel Like there could be data lost so just store a copy of the origional fireRate
	isFireRate = false;
}

void ASpaceshipCharacter::SpeedUp()
{
	if(!isAttacking && !IsInSelectScreen)
	{
	
	}
	CurrentFov += FovIncrease;
	MovementSpeed *=2;
}

void ASpaceshipCharacter::SpeedDown()
{
	CurrentFov = 90;
	MovementSpeed /=2;
}

void ASpaceshipCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABullet_CPP* BulletOBJ = Cast<ABullet_CPP>(OtherActor);
	if (BulletOBJ)
	{
		BulletOBJ->Destroy();
		float overflowDamage;


		//if (!StatsPlayerComponent->DodgeAttack())
		//{
			// take damage
			if (StatsPlayerComponent->CurrentShields > 0)
			{
				StatsPlayerComponent->CurrentShields -= StatsPlayerComponent->DamageTakenPerHit;
				if (StatsPlayerComponent->CurrentShields < 0)
				{
					overflowDamage = abs(StatsPlayerComponent->CurrentShields);
					StatsPlayerComponent->CurrentHullIntegrity -= overflowDamage;
					StatsPlayerComponent->CurrentShields = 0;
				}
			}
			else
			{
				StatsPlayerComponent->CurrentHullIntegrity -= StatsPlayerComponent->DamageTakenPerHit;
				if (StatsPlayerComponent->CurrentHullIntegrity <= 0)
				{
					StatsPlayerComponent->CurrentHullIntegrity = 0;
					Alive = false;
					
				}
			}

			DamageTakenDelegate.Broadcast();
		//}
		//else
		//{
			//DodgeDamageDelegate.Broadcast();

		//}

	}
}

void ASpaceshipCharacter::OnDeflectOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, 	OtherActor->GetName());
	
	if(OtherActor->ActorHasTag("EnemyBullet"))
	{
		const ABullet_CPP* Bullet = Cast<ABullet_CPP>(OtherActor);
		Bullet->ProjectileMovement->HomingTargetComponent = CurrentTarget;
	}
}


void ASpaceshipCharacter::StartQuest(UQuest* QuestStarted)
{
	ActiveQuest = QuestStarted;
}


void ASpaceshipCharacter::MoveCameraTo(AActor* Actor)
{
	/*
	struct FViewTargetTransitionParams TransitionParams;
	TransitionParams.bLockOutgoing = true;
	TransitionParams.BlendTime = 1.0f;
	PlayerController->SetViewTarget(Actor, TransitionParams);*/
}

void ASpaceshipCharacter::MoveTowards(FVector Target)
{
	FVector Direction = Target - GetActorLocation();
	Direction.Normalize();
	GetCharacterMovement()->Velocity = Direction * 400;
}

void ASpaceshipCharacter::WasQuestCompleted(FString planetName)
{
	if (ActiveQuest)
	{
		if (ActiveQuest->TargetName == planetName)
		{
			LastCompletedQuest = ActiveQuest;
			StatsPlayerComponent->IncreaseCurrency(LastCompletedQuest->CreditsGained);
			GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("XP"));
			StatsPlayerComponent->XPSystem(LastCompletedQuest->XPGained);
			ApplyInventoryToStats();
			CompleteQuestDelegate.Broadcast();
		}
	
	}
}

void ASpaceshipCharacter::CompleteQuest()
{
	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("XP"));
	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("Quest Completed"));
	ActiveQuest = nullptr;
}

void ASpaceshipCharacter::SaveGame()
{

	StatsPlayerComponent->SaveStats();
	
}