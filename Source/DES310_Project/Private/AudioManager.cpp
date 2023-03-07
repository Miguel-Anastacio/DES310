// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioManager.h"

// Sets default values
AAudioManager::AAudioManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	AmbientSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Ambient Audio Component"));
	BattleSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Battle Audio Component"));
	ThrusterSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Thruster Audio Component"));

}

// Called when the game starts or when spawned
void AAudioManager::BeginPlay()
{
	Super::BeginPlay();

		
	if(SoundCues[0]){
		AmbientSoundComponent->SetSound(SoundCues[0]);
	}
		
	if(SoundCues[1]){
		BattleSoundComponent->SetSound(SoundCues[1]);
	}
		
	if(SoundCues[2]){
		ThrusterSoundComponent->SetSound(SoundCues[2]);
	}
	
	
}

// Called every frame
void AAudioManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

