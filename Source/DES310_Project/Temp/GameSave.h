// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 12/02/23
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameSave.generated.h"

/**
 * 
 */
UCLASS()
class DES310_PROFPROJECT_API UGameSave : public USaveGame
{
	GENERATED_BODY()
	
public:

	 float routeSeed;
	 float playerHealth = 69;
	 FVector playerPosition;

	
};
