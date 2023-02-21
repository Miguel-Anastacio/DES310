// Fill out your copyright notice in the Description page of Project Settings.
// Reference https://www.cs.ubc.ca/~rbridson/docs/bridson-siggraph07-poissondisk.pdf
//
// Author: Liam Donald 2000495 1/02/23
//

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class DES310_PROFPROJECT_API PoissonDiscSampling
{
public:
	PoissonDiscSampling();
	~PoissonDiscSampling();

	static TArray<FVector2D>  PoissonDiscGenerator(float radius,FVector2D dimensions, int RejectionRate);
	
	static int ArrayCoords(int x, int y, int width);
	static bool IsValid(FVector2D newPoint,FVector2D dimensions, float cellSize,float radius ,const TArray<FVector2D>& points, const TArray<int>& grid);
};
