// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/
#pragma once

#include "CoreMinimal.h"
#include "GameEvents.h"
#include "Engine/DataAsset.h"
#include "RandomEventsData.generated.h"

/**
 * 
 */
UCLASS()
class DES310_PROJECT_API URandomEventsData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Data")
	TArray<TSubclassOf<class UGameEvents>> Data;

};
