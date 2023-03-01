// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Planet.h"
#include "Path.h"
#include "Components/SplineComponent.h"
#include "PathData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DES310_PROJECT_API UPathData : public UObject
{
	GENERATED_BODY()

public:

	TArray<USplineComponent*> Splines;
	TArray<APlanet*> Stops;
	int Index = 0;
	int Max = 0; // Could probably just use Stops.Num
	UPROPERTY(BlueprintReadOnly)
	int EventChance = 10;
	UPROPERTY(BlueprintReadOnly)
	int StoryQuestChance = 10;
	UPROPERTY(BlueprintReadOnly)
	int RandomQuestChance = 10;
	
};
