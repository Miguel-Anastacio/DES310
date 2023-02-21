// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 19/02/23
//


#pragma once

#include "BaseState.h"
#include "CoreMinimal.h"

/**
 * 
 */
class DES310_PROJECT_API OrbitingState : public BaseState
{
public:
	OrbitingState();
	~OrbitingState();

	virtual void OnEnter() override;
	virtual void Run() override;
	virtual void OnExit() override;
};


