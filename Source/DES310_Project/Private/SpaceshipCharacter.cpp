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

	// create player inventory
	PlayerInventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	GetCharacterMovement()->GravityScale = 0.0f;

	StatsPlayerComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats"));
}

// Called when the game starts or when spawned
void ASpaceshipCharacter::BeginPlay()
{
	Super::BeginPlay();
	// set controller parameters
	PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
		PlayerController->bShowMouseCursor = true;

	TargetLocation = GetActorLocation();

	CompleteQuestDelegate.AddUniqueDynamic(this, &ASpaceshipCharacter::CompleteQuest);
	StartQuestDelegate.AddUniqueDynamic(this, &ASpaceshipCharacter::StartQuest);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASpaceshipCharacter::OnOverlapBegin);
	
	if (!StatsPlayerComponent)
	{
		GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Red, TEXT("Stats Problem"));
	}
	ApplyInventoryToStats();
	StatsPlayerComponent->UpdateCurrentStats(StatsPlayerComponent->HullIntegrity, StatsPlayerComponent->Shields);
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
	StatsPlayerComponent->Shields = item->Modifiers.ShieldBonus * StatsPlayerComponent->BaseShields;
	StatsPlayerComponent->Speed = item->Modifiers.SpeedBonus * StatsPlayerComponent->BaseSpeed;
	StatsPlayerComponent->HullIntegrity = item->Modifiers.HealthBonus * StatsPlayerComponent->BaseHullIntegrity;
	StatsPlayerComponent->ATKPower = item->Modifiers.DamageBonus * StatsPlayerComponent->BaseATKPower;
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
}

// Called to bind functionality to input
void ASpaceshipCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// bind action
	PlayerInputComponent->BindAction("Mouse Click", IE_Pressed, this, &ASpaceshipCharacter::MouseClick);
	PlayerInputComponent->BindAction("Reset Game", IE_Pressed, this, &ASpaceshipCharacter::ResetGame);
}

void ASpaceshipCharacter::MouseClick()
{
	// only do this if the state is selecting
	if(IsInSelectScreen)
		Selected = true;
}



void ASpaceshipCharacter::ResetGame()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void ASpaceshipCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABullet_CPP* BulletOBJ = Cast<ABullet_CPP>(OtherActor);
	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("Overlap Detected"));
	if (BulletOBJ)
	{
		BulletOBJ->Destroy();
		float overflowDamage;
		// take damage
		if (StatsPlayerComponent->CurrentShields > 0)
		{
			StatsPlayerComponent->CurrentShields -= StatsPlayerComponent->DamageTakenPerHit;
			if (StatsPlayerComponent->CurrentShields < 0)
			{
				overflowDamage = abs(StatsPlayerComponent->CurrentShields);
				StatsPlayerComponent->CurrentHullIntegrity -= overflowDamage;
				GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("Taking Hull Damage"));
				StatsPlayerComponent->CurrentShields = 0;
			}
		}
		else
		{
			StatsPlayerComponent->CurrentHullIntegrity -= StatsPlayerComponent->DamageTakenPerHit;
			if (StatsPlayerComponent->CurrentHullIntegrity < 0)
				StatsPlayerComponent->CurrentHullIntegrity = 0;
				
			GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("Taking Hull Damage"));
		}

		DamageTakenDelegate.Broadcast();

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
