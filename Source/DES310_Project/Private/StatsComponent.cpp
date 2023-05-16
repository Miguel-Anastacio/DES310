// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: OWEN MCINTOSH 28/02/2023
*/

#include "StatsComponent.h"
#include "GameSave.h"
#include "GameInstance_CPP.h"
#include "SpaceshipCharacter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"


// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Init();

	// ...
}

void UStatsComponent::Init(int StarterLevel, int Currency)
{

	CurrentReputation = StarterLevel;
	XPToNext = CalcNextLevel();
	CurrentCurrency = Currency;

	SpeedIncrement = 10;
	ShieldIncrement = 15;
	HullIntegrityIncrement = 15;
	ATKPowerIncrement = 15;
	
	Speed = 100 + (CurrentReputation-1) * 5;				// max = 300, min = 100
	Shields = 20 + (CurrentReputation-1) * 15;				// max = 620, min = 20 (20% of HP -> 29.5% of HP)
	HullIntegrity = 100 + (CurrentReputation-1) * 50;		// max = 2100, min = 100
	ATKPower = 75 + (CurrentReputation - 1) * 18;			// max = 795, min = 75


	BaseSpeed = Speed;
	BaseHullIntegrity = HullIntegrity;
	BaseATKPower = ATKPower;
	BaseShields = Shields;

	
}

void UStatsComponent::InitAllBaseStats(int hull, int classSpeed, float shields, float attackPower)
{
	BaseHullIntegrity = hull;
	BaseATKPower = attackPower;
	BaseSpeed = classSpeed;
	BaseShields = shields;

	HullIntegrity = BaseHullIntegrity;
	Shields = BaseShields;
	ATKPower = BaseATKPower;
	Speed = BaseSpeed;

	// changed this to make speed more attractive
	// the 75 should be the value of the class available with the higher speed
	MaxSpeed = MAX_LEVEL * SpeedIncrement + 75 * 1.2;
}

void UStatsComponent::UpdateCurrentStats(float newHull, float newShields)
{
	CurrentHullIntegrity = newHull;
	CurrentShields = newShields;
}

void UStatsComponent::SetStatsBasedOnLevel(int level)
{
	// this is used only the enemy
	// he doesn't have items
	CurrentReputation = level;
	Shields = level * ShieldIncrement;
	HullIntegrity = level * HullIntegrityIncrement;
	Speed = level * SpeedIncrement;
	ATKPower = level * ATKPowerIncrement;

	CurrentHullIntegrity = HullIntegrity;
	CurrentShields = Shields;

}

bool UStatsComponent::DodgeAttack()
{
	MaxSpeed = MAX_LEVEL * SpeedIncrement + 75 * 1.2;
	if (Speed > FMath::RandRange(0, GetMaxSpeed()))
	{
		return true;
	}
	return false;
}

// Called when the game starts
void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();


	AttemptLoad();
	// ...
	

}


// Called every frame
void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStatsComponent::XPSystem(int GainedXP)
{

	if(CurrentReputation >= MAX_LEVEL)
	{
		return;
	}
	
	int CarryOverXP = 0;
    
	do {

		// reduce XP required until next level
		XPToNext -= GainedXP;

		// if we haven't overflowed into the next level then stop trying to calculate anymore xp gain
		if (XPToNext > 0)
		{
			break;
		}
		// catch the overflow amount
		CarryOverXP = XPToNext * -1;
		GainedXP -= (GainedXP - CarryOverXP);
		LevelUpStats();
		LevelUpDelegate.Broadcast();
		// calculate next level xp
		XPToNext = CalcNextLevel();

	} while (GainedXP > 0);


}

int UStatsComponent::CalcNextLevel() {

	return floor(((CurrentReputation * (CurrentReputation - 1)) / 15) * 100) + 100;

}

void UStatsComponent::LevelUpStats()
{
	CurrentReputation++;
	BaseSpeed += SpeedIncrement;
	BaseShields += ShieldIncrement ;
	BaseHullIntegrity += HullIntegrityIncrement;
	BaseATKPower += ATKPowerIncrement;

	ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(GetOwner());
	if (player)
	{
		player->ApplyInventoryToStats();
	}

	CurrentShields = Shields;
	CurrentHullIntegrity = HullIntegrity;
}

void UStatsComponent::IncreaseCurrency(int Amount)
{
	CurrentCurrency += Amount;
}

void UStatsComponent::DecreaseCurrency(int Amount)
{
	CurrentCurrency -= Amount;
}

bool UStatsComponent::AttemptLoad()
{

	if (!UGameplayStatics::DoesSaveGameExist(TEXT("Game_Save"), 0))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,	TEXT("SAVE NOT FOUND"));
		return false;
	}
	
	UGameSave* GameSave = (Cast<UGameInstance_CPP>(UGameplayStatics::GetGameInstance(GetWorld())))->GetGameData();

	FMemoryReader MemReader(GameSave->SavedPlayerStats.StatsByteData);
	
	FObjectAndNameAsStringProxyArchive Ar(MemReader, true);

	Ar.ArIsSaveGame = true;
	// Convert binary array back into actor's variables
	this->Serialize(Ar);

	return true;
	
}