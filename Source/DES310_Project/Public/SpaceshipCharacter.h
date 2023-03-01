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
#include "GameFramework/SpringArmComponent.h"
#include "SpaceshipCharacter.generated.h"

class APlanet;
class UInventoryComponent;
class URandomEventsComponent;
class ARouteExample;

UENUM(BlueprintType)
enum PlayerCurrentState
{
	IDLE   UMETA(DisplayName = "IDLE"),
	ON_PLANET UMETA(DisplayName = 'ON_PLANET')
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQuestCompletedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestStartedDelegate, UQuest*, NewQuest);

UCLASS()
class DES310_PROJECT_API ASpaceshipCharacter : public ACharacter
{
	GENERATED_BODY()


public:

	bool Selected = false;


	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* TopDownCamera;

	UPROPERTY(EditAnywhere, Category = Camera)
	USpringArmComponent* CameraBoom;

	// for some reason the default capsule component does not work
	UPROPERTY(VisibleAnywhere, Category = "Trigger Box")
	UBoxComponent* TriggerBox;
	
	// variable to store a reference to the player controller
	// prevents casting every time we need to use it
	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere)
	UInventoryComponent* PlayerInventoryComponent;


	// designer can set the frequency of the roll for event
	UPROPERTY(EditAnywhere)
	float GameplayEventTick = 3.0f;

	UPROPERTY(BlueprintReadWrite)
	APlanet* CurrentPlanet; 

	UPROPERTY(BlueprintReadWrite)
	UQuest* ActiveQuest = nullptr;

	UPROPERTY(BlueprintReadWrite)
	int Credits = 100.0f;

	// Sets default values for this character's properties
	ASpaceshipCharacter();

	UFUNCTION(BlueprintCallable)	
	PlayerCurrentState GetPlayerStatus() {
		return State;
	};

	UFUNCTION(BlueprintCallable)
	void SetPlayerStatus(PlayerCurrentState newState) {
		State = newState;
	};


	UFUNCTION(BlueprintCallable)
	UInventoryComponent* GetPlayerInventory() {
		return PlayerInventoryComponent;
	}



	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// change camera 
	void MoveCameraTo(AActor* Actors);

	// helper function to move towards a location
	void MoveTowards(FVector target);

	// Events delegates and fucntions binded to events
	UFUNCTION()
	void CompleteQuest();

	UPROPERTY(BlueprintAssignable, Category = "Custom Events", BlueprintCallable)
	FQuestCompletedDelegate CompleteQuestDelegate;


	UFUNCTION(BlueprintCallable)
		void StartQuest(UQuest* QuestStarted);

	UPROPERTY(BlueprintAssignable, Category = "Custom Events", BlueprintCallable)
		FQuestStartedDelegate StartQuestDelegate;

protected:

	PlayerCurrentState State = IDLE;

	FVector TargetLocation = FVector(0, 0, 0);

	float TimePassedSinceLastEventTick = 0.0f;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MouseClick();
};
