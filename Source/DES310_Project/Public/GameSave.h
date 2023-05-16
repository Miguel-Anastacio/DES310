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
USTRUCT()
struct FRouteObjectPair
{
	GENERATED_BODY()

	UPROPERTY()
	FString PlanetName;
	
	UPROPERTY()
	int Index;

	UPROPERTY()
	FVector ObjectPosition;
	
};

USTRUCT()
struct FRouteData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FRouteObjectPair> Planets;
	
	UPROPERTY()
	TArray<FRouteObjectPair> Details;
	
	UPROPERTY()
	TArray<FVector> Spline1Points;

	UPROPERTY()
	TArray<FVector> Spline2Points;

	UPROPERTY()
	TArray<FVector> Spline3Points;

	UPROPERTY()
	float SkyboxHue;

	UPROPERTY()
	int32 CurrentQuestID;
};

USTRUCT()
struct FPlayerSaveData
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FName playerName;
	
	/* Contains all 'SaveGame' marked variables of the Actor */
	UPROPERTY()
	TArray<uint8> StatsByteData;

	UPROPERTY()
	FVector PlayerPos;

	UPROPERTY()
	TArray<int> InventoryItemIDs;
	
};

UCLASS()
class DES310_PROJECT_API UGameSave : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	FPlayerSaveData SavedPlayerStats;

	UPROPERTY()
	FRouteData SavedRouteData;
	
};
