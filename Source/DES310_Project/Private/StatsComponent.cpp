// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: OWEN MCINTOSH 28/02/2023
*/

#include "StatsComponent.h"

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

	SpeedIncrement = 5;
	ShieldIncrement = 15;
	HullIntegrityIncrement = 50;
	ATKPowerIncrement = 18;
	
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
}

// Called when the game starts
void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	
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
	
	auto CarryOverXP = 0;
    
	do {

		// reduce XP required until next level
		XPToNext -= GainedXP;

		// if we haven't overflowed into the next level then stop trying to calculate anymore xp gain
		if (XPToNext > 0)
		{
			break;
		}
		GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("XP System"));
		// catch the overflow amount
		CarryOverXP = XPToNext * -1;
		GainedXP -= (GainedXP - CarryOverXP);
		LevelUpStats();
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
	UE_LOG(LogTemp, Warning, TEXT("level Up Stats"));
}

void UStatsComponent::IncreaseCurrency(int Amount)
{
	CurrentCurrency += Amount;
}

void UStatsComponent::DecreaseCurrency(int Amount)
{
	CurrentCurrency -= Amount;
}