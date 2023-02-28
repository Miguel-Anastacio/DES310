// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "UObject/NoExportTypes.h"
#include "MovingState.generated.h"

/**
 * 
 */
UCLASS()
class DES310_PROJECT_API UMovingState : public UBaseState
{
	GENERATED_BODY()

	public:

	virtual void OnEnter() override;
	virtual void Update() override;
	virtual void OnExit() override;

	virtual void Reset() override;
};
