// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 19/02/23
//

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class DES310_PROFPROJECT_API BaseState
{
public:
	BaseState();
	~BaseState();

	virtual void OnEnter();
	virtual void Run();
	virtual void OnExit();

	uintptr_t RouteParent;
};







