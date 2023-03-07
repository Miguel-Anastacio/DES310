// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Quest.generated.h"

class APlanet;
/**
 * 
 */
UCLASS(Blueprintable)
class DES310_PROJECT_API UQuest : public UObject
{
	GENERATED_BODY()
	
	public:

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
	FName ID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
	FString Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
	FText Objective;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
	TSubclassOf<class APlanet> Target;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
	FString TargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
	int32 XPGained;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
	int32 CreditsGained;

	/*
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	UTexture2D* Icon;
	*/

	/*
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		TEnumAsByte<PartType> Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
		FPartModifiers Modifiers;*/
};
