// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "AudioManager.generated.h"

UCLASS()
class DES310_PROJECT_API AAudioManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAudioManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere) UAudioComponent* AmbientSoundComponent;
	UPROPERTY(VisibleAnywhere) UAudioComponent* BattleSoundComponent;
	UPROPERTY(VisibleAnywhere) UAudioComponent* ThrusterSoundComponent;
	UPROPERTY(VisibleAnywhere) UAudioComponent* AlarmSoundComponent;
	UPROPERTY(VisibleAnywhere) UAudioComponent* ShootSoundComponent;
	UPROPERTY(VisibleAnywhere) UAudioComponent* TurboSoundComponent;
	UPROPERTY(VisibleAnywhere) UAudioComponent* VictorySoundComponent;
	UPROPERTY(VisibleAnywhere) UAudioComponent* DefeatSoundComponent;

	UPROPERTY(EditAnywhere, Category = Sound)  TArray <class USoundCue*> SoundCues;

};
