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
class DES310_PROJECT_API SelectingState : public BaseState
{
public:
	SelectingState();
	~SelectingState();

	void OnEnter() override;
	void Run() override;
	void OnExit() override;
	
};


