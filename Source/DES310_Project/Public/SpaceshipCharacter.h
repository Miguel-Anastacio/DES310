// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "InventoryComponent.h"
#include "Quest.h"
#include "AudioManager.h"
#include "Enemy.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/PrimitiveComponent.h"
#include "SpaceshipCharacter.generated.h"

class APlanet;
class UStatsComponent;
class URandomEventsComponent;
class ARouteExample;
class UAbilityComponent;

UENUM(BlueprintType)
enum PlayerCurrentState
{
	IDLE UMETA(DisplayName = "IDLE"),
	ON_PLANET UMETA(DisplayName = 'ON_PLANET')
};


UENUM(BlueprintType)
enum ShipClassType
{
	HEAVY_VIKING UMETA(DisplayName = "Heavy Viking"),
	FIGHTER_VIKING UMETA(DisplayName = "Fighter Viking"),
	HEAVY_EGYPTIAN UMETA(DisplayName = "Heavy Egyptian"),
	FIGHTER_EGYPTIAN UMETA(DisplayName = "Fighter Egyptian"),
};

UENUM(BlueprintType)
enum EngineStatus
{
	ACCELERATING UMETA(DisplayName = "Accelerating"),
	AT_MAX_SPEED UMETA(DisplayName = "At Max Speed"),
	SLOWING_DOWN UMETA(DisplayName = "Slowing Down"),
	CRUISING UMETA(DisplayName = "Cruising"),
};
 
USTRUCT(BlueprintType)
struct FPlayerShipClass
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere) FText ClassDescription;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) float Speed;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) float Shield;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) float Hull;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) float AttackPower;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) TEnumAsByte<ShipClassType> Type;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UTexture2D* ClassImage;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQuestCompletedDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDamageTakenDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDodgeDamageDelegate);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestStartedDelegate, UQuest*, NewQuest);

UCLASS()
class DES310_PROJECT_API ASpaceshipCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	bool Selected = false;
	bool IsInSelectScreen = false;
	bool Alive = true;

	//Actor Components
	UPROPERTY(VisibleAnywhere, Category = Camera)UCameraComponent* TopDownCamera;
	UPROPERTY(EditAnywhere, Category = Camera)USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = "Trigger Box")UBoxComponent* TriggerBox; // for some reason the default capsule component does not work
	UPROPERTY(EditAnywhere, Category = "Deflection Device") UStaticMeshComponent* DeflectionMesh;
	UPROPERTY(VisibleAnywhere, Category = "Trigger Box") UBoxComponent* DeflectionTriggerBox;

	UPROPERTY(EditAnywhere, Category = "Deflection Device") UStaticMeshComponent* ShieldMesh;
	//UPROPERTY(VisibleAnywhere, Category = "Trigger Box") UBoxComponent* ShieldTriggerBox; No collision interaction, so trigger is unneeded
	
	//Shield Variables
	bool isShielding = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)float shieldingCharge = 100;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)float InitShieldingCharge = 100;
	
	bool isDeflecting = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)int deflectCharges = 5;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)int InitDeflectCharges = 5;

	bool isFireRate = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)float FireRateCharge = 100;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)float InitFireRateCharge = 100;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)float FovIncrease = 30;
	float CurrentFov = 90;
	
	float DeflectionTimer = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)float DeflectionLength = 1.5f;

	// variable to store a reference to the player controller
	// prevents casting every time we need to use it
	UPROPERTY()APlayerController* PlayerController;
	UPROPERTY(VisibleAnywhere)UInventoryComponent* PlayerInventoryComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)UStatsComponent* StatsPlayerComponent;
	UPROPERTY(EditAnywhere)float GameplayEventTick = 3.0f; // designer can set the frequency of the roll for event


	UPROPERTY(BlueprintReadWrite)APlanet* CurrentPlanet;
	// this is the quest you should save
	UPROPERTY(BlueprintReadWrite)UQuest* ActiveQuest = nullptr;
	UPROPERTY(EditAnywhere)TArray<TSubclassOf<UQuest>> QuestTemplates;

	UPROPERTY(BlueprintReadWrite)UQuest* LastCompletedQuest = nullptr;
	UPROPERTY(BlueprintReadWrite) int Credits = 100.0f;

	// Sets default values for this character's properties
	ASpaceshipCharacter();

	void Attack(float DeltaTime, AEnemy* Enemy);
	void ResetCombat();

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MovementSpeed = 1250;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxMovementSpeed = 1750;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MinMovementSpeed = 1250;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int Steps = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StepSpeed = 200;
	
	//Attack Variables
	UPROPERTY(EditAnywhere)TArray<ABullet_CPP*> Bullets;
	UPROPERTY(EditAnywhere)TSubclassOf<class ABullet_CPP> PlayerBulletBP;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UAbilityComponent* AbilitiesComponent;


	UPROPERTY(EditAnywhere, Category = Fight)float BulletSpawnOffset = -150.f;
	UPROPERTY(EditAnywhere, Category = Fight)float BulletSpeed = 1000.f;
	UPROPERTY(EditAnywhere, Category = Fight)float BulletAngleRange = 60;

	UPROPERTY(EditAnywhere, Category = Fight)float HomingStrength = 1.f;
	UPROPERTY(EditAnywhere, Category = Fight)float FireRate = 1.5f;
	UPROPERTY(EditAnywhere, Category = Fight)float DefaultFireRate = 1.5f;
	float FireRateTimer = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)bool isAttacking = false;
	UPROPERTY(VisibleAnywhere)AAudioManager* AudioManager;

	UPROPERTY() USceneComponent* CurrentTarget;// can maybe just be a actor pointer incase the player can target things other than enemeis

	// class of the ship 
	// we have to save the type of ship so that we can load the appropriate player model
	UPROPERTY(BlueprintReadWrite) FPlayerShipClass PlayerShip;
	// player faction, probably better to have an enum but there is only two factions so: true - viking , false - egyptian
	// save this so that we can set the right frames in the UI
	UPROPERTY(BlueprintReadWrite) bool PlayerFaction;

	UFUNCTION(BlueprintCallable)
	void SetStatsBasedOnClass();

	UFUNCTION(BlueprintCallable)
	PlayerCurrentState GetPlayerStatus()
	{
		return State;
	};

	UFUNCTION(BlueprintCallable)
	void SetPlayerStatus(PlayerCurrentState newState)
	{
		State = newState;
	};


	UFUNCTION(BlueprintCallable)
	UInventoryComponent* GetPlayerInventory()
	{
		return PlayerInventoryComponent;
	}

	UFUNCTION(BlueprintCallable)
	void ApplyInventoryToStats();
	UFUNCTION()
	void ApplyItemToStats(UItem* item);

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerStats(int xpGained);

	UFUNCTION()
		void LoadQuestBasedOnID(int ID);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
	                    class UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDeflectOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
	                           class UPrimitiveComponent* OtherComp,
	                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// change camera 
	void MoveCameraTo(AActor* Actors);

	// helper function to move towards a location
	void MoveTowards(FVector target);

	void WasQuestCompleted(FString planetName);

	// Events delegates and fucntions binded to events
	UFUNCTION()
	void CompleteQuest();
	void SaveGame();

	UPROPERTY(BlueprintAssignable, Category = "Custom Events", BlueprintCallable)
	FQuestCompletedDelegate CompleteQuestDelegate;


	UFUNCTION(BlueprintCallable)
	void StartQuest(UQuest* QuestStarted);

	UPROPERTY(BlueprintAssignable, Category = "Custom Events", BlueprintCallable)
	FQuestStartedDelegate StartQuestDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Custom Events", BlueprintCallable)
	FDamageTakenDelegate DamageTakenDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Custom Events", BlueprintCallable)
	FDodgeDamageDelegate DodgeDamageDelegate;


	// Miguel: My take on the minigame
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BaseAcceleration = 10.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxAcceleration= 100.f;

	UPROPERTY(VisibleAnywhere)
	float CurrentAcceleration = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AccelerationDecrement = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxSpeed = 1750.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TimeAtMaxSpeed = 5.f;

	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EngineStatus> EngineStatus = CRUISING;

	UPROPERTY(VisibleAnywhere)
	float SpeedTimer = 0.f;

	UFUNCTION(BlueprintCallable)
	// pass 1 to increase, -1 to decrease
	void UpdateAcceleration(int multiplier);

	void UpdatePlayerSpeed(float DeltaTime);

	PlayerCurrentState State = IDLE;

	FVector TargetLocation = FVector(0, 0, 0);

	float TimePassedSinceLastEventTick = 0.0f;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Input Functions
	void MouseClick();
	void ResetGame();
	void ShieldPressed();
	void ShieldReleased();
	void DeflectPressed();
	void FireRatePressed();
	void FireRateReleased();
	void SpeedUp();
	void SpeedDown();
};
