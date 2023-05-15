// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 1/02/23
//
//With reference to
//https://www.cs.ubc.ca/~rbridson/docs/bridson-siggraph07-poissondisk.pdf


#include "PoissonDiscSampling.h"


PoissonDiscSampling::PoissonDiscSampling()
{
}

PoissonDiscSampling::~PoissonDiscSampling()
{
}

//We are storing a 2D grid as a 1 dimensional array, so we need away to convert the index position
int PoissonDiscSampling::ArrayCoords(int x, int y, int width) 
{
	return  width * y + x;
}

TArray<FVector2D>  PoissonDiscSampling::PoissonDiscGenerator(float Radius, FVector2D Dimensions, int RejectionRate = 30)
{

	//make the size of each cell allow only one possible point
	float CellSize = Radius / sqrt(2);

	// initilise the grid with unvisited points
	FVector2D GridSize(FMath::CeilToInt(Dimensions.X/CellSize), FMath::CeilToInt(Dimensions.Y/CellSize));
	TArray<int> grid;
	grid.Init(0,GridSize.X * GridSize.Y); // The grid is implemented as a 1D array so we need a function to convert from 2D to 1D

	//Add a random point to the list to begin the generation
	TArray<FVector2D> ActivePoints;
	TArray<FVector2D> FinalPoints;
	
	//ActivePoints.Add(FVector2D(FMath::RandRange(0.0,dimensions.X),FMath::RandRange(0.0,dimensions.Y))); // Add a random position to spawnPoints;
	ActivePoints.Add(Dimensions/2); // Add the center to spawnPoints;
	
	while(ActivePoints.Num() > 0)
	{
		//Chose a random point
		int RandomIndex = FMath::RandRange(0,ActivePoints.Num() - 1);
		FVector2D ActivePoint = ActivePoints[RandomIndex];
		bool isValid = false;
		
		//randomly check the surrounding area for valid points, X amount of times
		for(int i = 0; i < RejectionRate;i++)
		{
			//Get a random angle
			float Angle = FMath::RandRange(0.0,PI * 2.0);
			FVector2D Direction(sin(Angle),cos(Angle)); // Convert to a vector
			FVector2D NewPoint = ActivePoint + Direction * FMath::RandRange(Radius,Radius * 2);
			
			if(IsValid(NewPoint, Dimensions, CellSize, Radius, FinalPoints, grid)) // Check if the new point is valid and pass the necessary info 
			{
				FinalPoints.Add(NewPoint);
				ActivePoints.Add(NewPoint);
				grid[ArrayCoords((int)(NewPoint.X/CellSize),(int)(NewPoint.Y/CellSize),GridSize.X)] = FinalPoints.Num();
				isValid = true;
				break;
			}
		}
		if(!isValid) // if the point wasnt accepted then remove from the active points
		{
			ActivePoints.RemoveAt(RandomIndex);
		}
	}
	return FinalPoints;
}


bool PoissonDiscSampling::IsValid(FVector2D NewPoint,FVector2D Dimensions, float CellSize,float Radius, const TArray<FVector2D>& FinalPoints, const TArray<int>& Grid)
{
	// check if the point is even within the valid dimensions
	if(NewPoint.X >= 0 && NewPoint.X < Dimensions.X && NewPoint.Y >= 0 && NewPoint.Y < Dimensions.Y)
	{
		//convert from world position to the grid array index position
		int PositionX = (int)(NewPoint.X/CellSize);
		int PositionY = (int)(NewPoint.Y/CellSize);
		
		FVector2D GridSize(FMath::CeilToInt(Dimensions.X/CellSize), FMath::CeilToInt(Dimensions.Y/CellSize));

		//We look at the surrounding grid points and check if the points overlaps the its radius
		for (int x = FMath::Max(0,PositionX -2); x <= FMath::Min(PositionX+2,GridSize.X-1); x++) {
			for (int y = FMath::Max(0,PositionY -2); y <= FMath::Min(PositionY+2,GridSize.Y-1); y++) {
				int index = Grid[ArrayCoords(x,y,GridSize.X)] - 1;
				if (index != -1) {
					float Distance = (NewPoint - FinalPoints[index]).SquaredLength(); // sqrt is slow so just compare the squared distances
					if (Distance < Radius*Radius) {
						return false;
					}
				}
			}
		}
		
		return true;
	}

	return false;
};
