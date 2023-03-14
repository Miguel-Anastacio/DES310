// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: OWEN MCINTOSH 28/02/2023
*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"

#define MAX_LEVEL 40

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int CurrentReputation;
	UPROPERTY(EditAnywhere) int XPToNext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int CurrentCurrency;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int Speed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int BaseSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Shields;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float BaseShields;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int HullIntegrity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int BaseHullIntegrity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int ATKPower;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int BaseATKPower;

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
	void Init(int StarterLevel = 1, int Currency = 500);
};