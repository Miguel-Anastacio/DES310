// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpaceshipCharacter.h"
#include "Engine/GameInstance.h"
#include "GameInstance_CPP.generated.h"

/**
 *
 */
UCLASS()
class DES310_PROJECT_API UGameInstance_CPP : public UGameInstance
{
	GENERATED_BODY()
	void Init() override;

	FString SaveSlot = "Game_Save";

	UPROPERTY() class UGameSave* GameSave;

public:
	void SaveGameData(ARouteExample* CurrentRoute);
	UGameSave* GetGameData();

};
