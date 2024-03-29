// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseState.generated.h"

/**
 * 
 */
UCLASS()
class DES310_PROJECT_API UBaseState : public UObject
{
	GENERATED_BODY()
	
public:
	
	virtual void OnEnter();
	virtual void Update();
	virtual void OnExit();

	virtual void Reset();

	
};
