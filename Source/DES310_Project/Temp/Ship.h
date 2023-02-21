// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 02/02/23
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameSave.h"
#include "Ship.generated.h"

UCLASS()
class DES310_PROFPROJECT_API AShip : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UGameSave* SaveData;
	float playerHealth = 100;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void Save();
	void Load();
};
