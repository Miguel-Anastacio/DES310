// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityComponent.generated.h"


USTRUCT(BlueprintType)
struct FPlayerAbility
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ability) FString Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ability) FText Description;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ability) float Duration = 5.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ability) float Cooldown = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ability) bool InUse = false;
	float timer = 0.0f;

};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class DES310_PROJECT_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerAbility BulletDeflector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerAbility SpecialAttack;

	// Sets default values for this component's properties
	UAbilityComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DoBulletDeflector();
	void DisableBulletDeflector();
	void DoSpecialAttack();
	void DisableSpecialAttack();
	UFUNCTION(BlueprintCallable) void DoAbilitySelected(FString AbilityName);
		
};
