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

}

// Called every frame
void ASpaceshipCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UGameEvents* event = nullptr;
	/*
	switch (State)
	{
	case MOVING:
		//MoveTowards(TargetLocation);
		// here it would get the chance of an event along this route instead of just a magic number
		TimePassedSinceLastEventTick += DeltaTime;
		/*
		if(TimePassedSinceLastEventTick > GameplayEventTick)
		{
			TimePassedSinceLastEventTick = 0.0f;
			event = EventsComponent->RollForEvent(20);
			if (event)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, event->Name);
				State = ON_PLANET;
			}
		}
		break;
	case FIGHTING:
		break;
	case IDLE:
		GetCharacterMovement()->Velocity = FVector(0, 0, 0);
		break;
	case ON_PLANET:
		GetCharacterMovement()->Velocity = FVector(0, 0, 0);
		break;
	default:
		break;
	}*/
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
	FHitResult Hit;
	/*
	if (State == IDLE)
	{
		if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, Hit))
		{
			if (Hit.GetActor()->ActorHasTag(TEXT("Planet")))
			{
				TargetLocation = Hit.GetActor()->GetActorLocation();
				State = MOVING;
			}
		}
	}*/

	Selected = true;

}

void ASpaceshipCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// stop moving
	if (OtherActor->ActorHasTag(TEXT("Planet")))
	{
		State = ON_PLANET;
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Reached Planet"));
		CurrentPlanet = Cast<APlanet>(OtherActor);

		// test if we ended a quest
		// compare current planet to target from quest
		if (ActiveQuest)
		{
			APlanet* Target = ActiveQuest->Target.GetDefaultObject();
			if (Target)
			{
				if (Target->Name == CurrentPlanet->Name)
				{
					// quest completed
					GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("Quest Completed"));
					ActiveQuest = nullptr;
				}
			}
		}
		
	}
}


void ASpaceshipCharacter::StartQuest(UQuest* QuestStarted)
{
	ActiveQuest = QuestStarted;
	State = IDLE;
	MoveCameraTo(this);
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
