// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 02/02/23
//

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class DES310_PROJECT_API VertexEdge
{
public:
	VertexEdge();
	VertexEdge(FVector2D newA, FVector2D newB);
	~VertexEdge();

	static bool AlmostEqual(VertexEdge left, VertexEdge right);
	static bool AlmostEqual(float x, float y) ;
	static bool AlmostEqual(FVector2D left, FVector2D right) ;

	bool operator== (VertexEdge left)
	{
		return  A == left.A && B == left.B;
	}
	
	FVector2D A,B;
	bool isBad = false;
};
