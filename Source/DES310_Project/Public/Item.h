// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"


UENUM(BlueprintType)
enum PartType
{
	SHIELD  UMETA(DisplayName = "SHIELD"),
	HULL   UMETA(DisplayName = "HULL"),
	BLASTER UMETA(DisplayName = "BLASTERS"),
	ENGINE UMETA(DisplayName = "ENGINE"),
};

USTRUCT(BlueprintType)
struct FPartModifiers
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		float DamageBonus = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		float SpeedBonus = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		float ShieldBonus = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		float HealthBonus = 1.0f;
};



UCLASS(Blueprintable)
class DES310_PROJECT_API UItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	FName ID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	FString Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	TEnumAsByte<PartType> Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	FPartModifiers Modifiers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	float Price = 10.0f;

};
