// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityComponent.h"
#include "SpaceshipCharacter.h"
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
	if (BulletDeflector.InUse)
	{
		BulletDeflector.timer += DeltaTime;
		if (BulletDeflector.timer > BulletDeflector.Duration)
		{
			// disable bullet deflector
			DisableBulletDeflector();
		}
	}

	if (SpecialAttack.InUse)
	{
		SpecialAttack.timer += DeltaTime;
		if (SpecialAttack.timer > SpecialAttack.Duration)
		{
			// disable special attack
			DisableSpecialAttack();
		}
	}
	// ...
}

void UAbilityComponent::DoBulletDeflector()
{
	ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(GetOwner());

	if (player->deflectCharges <= 0 || player->isDeflecting || !player->isAttacking)
		return;

	player->deflectCharges--;
	player->isDeflecting = true;
	player->DeflectionMesh->SetActive(true);
	player->DeflectionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	player->DeflectionMesh->SetVisibility(true);

	player->DeflectionTriggerBox->SetActive(true);
	player->DeflectionTriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	player->DeflectionTriggerBox->SetVisibility(true);

	BulletDeflector.InUse = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Deflection Enabled"));
}

void UAbilityComponent::DisableBulletDeflector()
{
	ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(GetOwner());

	player->isDeflecting = false;

	player->DeflectionMesh->SetActive(false);
	player->DeflectionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	player->DeflectionMesh->SetVisibility(false);

	player->DeflectionTriggerBox->SetActive(false);
	player->DeflectionTriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	player->DeflectionTriggerBox->SetVisibility(false);

	BulletDeflector.InUse = false;
	BulletDeflector.timer = 0.f;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Deflection Disabled"));
}

void UAbilityComponent::DoSpecialAttack()
{
	ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(GetOwner());

	if (player->FireRateCharge < 0 || !player->isAttacking)
		return;

	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("Fire Rate Increased"));

	player->FireRate /= 2;
	player->isFireRate = true;
	SpecialAttack.InUse = true;
}

void UAbilityComponent::DisableSpecialAttack()
{
	ASpaceshipCharacter* player = Cast<ASpaceshipCharacter>(GetOwner());
	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Blue, TEXT("Fire Rate Decreased"));

	player->FireRate = player->DefaultFireRate; // Feel Like there could be data lost so just store a copy of the origional fireRate
	player->isFireRate = false;
	SpecialAttack.InUse = false;
	SpecialAttack.timer = 0.f;
}

void UAbilityComponent::DoAbilitySelected(FString AbilityName)
{
	if (AbilityName == BulletDeflector.Name)
		DoBulletDeflector();
	else if (AbilityName == SpecialAttack.Name)
		DoSpecialAttack();
}

