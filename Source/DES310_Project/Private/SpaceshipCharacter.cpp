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
	CameraBoom->CameraLagSpeed = 200.0f;

	// create camera component
	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// declare trigger capsule
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	TriggerBox->InitBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetupAttachment(GetMesh());

	// create player inventory
	PlayerInventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	GetCharacterMovement()->GravityScale = 0.0f;
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

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASpaceshipCharacter::OnOverlapBegin);
	CompleteQuestDelegate.AddUniqueDynamic(this, &ASpaceshipCharacter::CompleteQuest);
	StartQuestDelegate.AddUniqueDynamic(this, &ASpaceshipCharacter::StartQuest);

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
}

void ASpaceshipCharacter::MouseClick()
{
	Selected = true;
}

void ASpaceshipCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

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
		APlanet* Target = ActiveQuest->Target.GetDefaultObject();
		if (Target)
		{
			if (Target->Name == planetName)
			{
				CompleteQuestDelegate.Broadcast();
			}
		}
	}
}

void ASpaceshipCharacter::CompleteQuest()
{
	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("Quest Completed"));
	ActiveQuest = nullptr;
}
