// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
//#include "InventoryComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SpacehipCharacter.generated.h"

UENUM(BlueprintType)
enum PlayerCurrentState
{
	MOVING   UMETA(DisplayName = "MOVING"),
	FIGHTING     UMETA(DisplayName = "FIGHTING"),
	IDLE   UMETA(DisplayName = "IDLE"),
	ON_PLANET UMETA(DisplayName = 'ON_PLANET')
};

class UInventoryComponent;
class URandomEventsComponent;

UCLASS()
class DES310_PROJECT_API ASpaceshipCharacter : public ACharacter
{
	GENERATED_BODY()
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

	/*
	UPROPERTY(VisibleAnywhere)
		UInventoryComponent* PlayerInventoryComponent;

	UPROPERTY(VisibleAnywhere)
		URandomEventsComponent* EventsComponent;*/

	// designer can set the frequency of the roll for event
	UPROPERTY(EditAnywhere)
		float GameplayEventTick = 3.0f;

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


	/*
	UFUNCTION(BlueprintCallable)
		UInventoryComponent* GetPlayerInventory() {
		return PlayerInventoryComponent;
	}

	UFUNCTION(BlueprintCallable)
		URandomEventsComponent* GetEventsComponent() {
		return EventsComponent;
	}*/

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


protected:

	PlayerCurrentState State = IDLE;

	FVector TargetLocation = FVector(0, 0, 0);

	float TimePassedSinceLastEventTick = 0.0f;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MouseClick();

};
