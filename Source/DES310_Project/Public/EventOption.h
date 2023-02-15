// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/
#pragma once

#include "CoreMinimal.h"

#include "UObject/NoExportTypes.h"
#include "EventOption.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DES310_PROJECT_API UEventOption : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Option)
	FText Description;

	// effect of this option
	// p.ex: starts combat then it has to say which is the opponent of the player
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Option)
	//TSubclassOf<class AEnemy> Enemy;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Option)
	float XPGained = 0;

};
