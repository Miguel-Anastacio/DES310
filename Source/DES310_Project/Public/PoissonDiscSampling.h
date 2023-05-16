// Fill out your copyright notice in the Description page of Project Settings.
// Reference https://www.cs.ubc.ca/~rbridson/docs/bridson-siggraph07-poissondisk.pdf
//
// Author: Liam Donald 2000495 1/02/23
//

#pragma once

#include "CoreMinimal.h"

//Used to Generate Random Points but evenly spread, so that they arent place on top of each other
class DES310_PROJECT_API PoissonDiscSampling
{
public:
	PoissonDiscSampling();
	~PoissonDiscSampling();

	static TArray<FVector2D>  PoissonDiscGenerator(float Radius,FVector2D Dimensions, int RejectionRate);
	
	static int ArrayCoords(int x, int y, int width); //convert from 2D coords to 1D
	static bool IsValid(FVector2D NewPoint,FVector2D Dimensions, float CellSize,float Radius ,const TArray<FVector2D>& Points, const TArray<int>& Grid);
};
