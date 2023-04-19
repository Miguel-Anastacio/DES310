// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 12/02/23
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "StatsComponent.h"
#include "GameSave.generated.h"

/**
 * 
 */
UCLASS()
class DES310_PROJECT_API UGameSave : public USaveGame
{
	GENERATED_BODY()
	
public:

	FPlayerSaveData SavedPlayerStats;

	
};
