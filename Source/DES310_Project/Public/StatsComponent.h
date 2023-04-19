// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: OWEN MCINTOSH 28/02/2023
*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/MemoryWriter.h"
#include "Containers/Array.h"
#include "Serialization/NameAsStringProxyArchive.h"
#include "StatsComponent.generated.h"

#define MAX_LEVEL 50

USTRUCT()
struct FPlayerSaveData
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FName playerName;
	
	/* Contains all 'SaveGame' marked variables of the Actor */
	UPROPERTY()
	TArray<uint8> ByteData;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class DES310_PROJECT_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame) int CurrentReputation;
	UPROPERTY(EditAnywhere) int XPToNext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame) int CurrentCurrency;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame) int Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int BaseSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Shields;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BaseShields;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) float CurrentShields;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int HullIntegrity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int BaseHullIntegrity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) int CurrentHullIntegrity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) int ATKPower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int BaseATKPower;

	UPROPERTY(EditAnywhere) float DamageTakenPerHit = 50;
protected:
	UPROPERTY(EditAnywhere) int SpeedIncrement;
	UPROPERTY(EditAnywhere) float ShieldIncrement;
	UPROPERTY(EditAnywhere) int HullIntegrityIncrement;
	UPROPERTY(EditAnywhere) int ATKPowerIncrement;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void XPSystem(int GainedXP);

	UFUNCTION()
	int CalcNextLevel();

	UFUNCTION()
	void LevelUpStats();

	UFUNCTION(BlueprintCallable)
	void IncreaseCurrency(int Amount);

	UFUNCTION(BlueprintCallable)
	void DecreaseCurrency(int Amount);

	UFUNCTION()
	void SaveStats();

UFUNCTION()
	bool AttemptLoad();

	UFUNCTION()
	void Init(int StarterLevel = 1, int Currency = 500);

	UFUNCTION(BlueprintCallable)
		void InitAllBaseStats(int hull, int classSpeed, float classShields, float attackPower);
	UFUNCTION(BlueprintCallable)
		void UpdateCurrentStats(float newHull, float newShields);

	void SetStatsBasedOnLevel(int level);

	int GetMaxSpeed() { return MAX_LEVEL * SpeedIncrement; };

	bool DodgeAttack();


};
