// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityComponent.h"

// Sets default values for this component's properties
UAbilityComponent::UAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAbilityComponent::DoBulletDeflector()
{
	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("BulletDeflector"));
}

void UAbilityComponent::DoSpecialAttack()
{
	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("SpecialAttack"));
}

void UAbilityComponent::DoAbilitySelected(FString AbilityName)
{
	if (AbilityName == BulletDeflector.Name)
		DoBulletDeflector();
	else if (AbilityName == SpecialAttack.Name)
		DoSpecialAttack();
}

