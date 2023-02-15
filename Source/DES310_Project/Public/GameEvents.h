// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/
#pragma once

#include "CoreMinimal.h"
#include "EventOption.h"
#include "UObject/NoExportTypes.h"
#include "GameEvents.generated.h"

UENUM(BlueprintType)
enum EventType
{
	STORY UMETA(DisplayName = "STORY"),
	COMBAT  UMETA(DisplayName = "COMBAT")
};

/**
 * 
 */
UCLASS(Blueprintable)
class DES310_PROJECT_API UGameEvents : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Event)
	FName ID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Event)
	FString Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Event)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Event)
	UTexture2D* Image;

	// chance of event firing in percentage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Event)
	int32 Chance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Event)
	TEnumAsByte<EventType> Type;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Event)
	TArray<TSubclassOf<class UEventOption>> Options;

	UPROPERTY(BlueprintReadOnly)
	TArray<UEventOption*> EventOptions;



};
