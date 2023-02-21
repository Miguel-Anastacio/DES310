// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 1/02/23
//


#include "PoissonDiscSampling.h"

#include "Kismet/KismetMathLibrary.h"

PoissonDiscSampling::PoissonDiscSampling()
{
}

PoissonDiscSampling::~PoissonDiscSampling()
{
}

int PoissonDiscSampling::ArrayCoords(int x, int y, int width)
{
	return  width * y + x;
}

/*bool PoissonDiscSampling::IsValid(FVector2D newPoint,FVector2D dimensions, float cellSize,float radius, const TArray<FVector2D>& points, const TArray<int>& grid)
{
	if(newPoint.X >= 0 && newPoint.X < dimensions.X && newPoint.Y >= 0 && newPoint.Y < dimensions.Y)
	{
		int cellX = (int)(newPoint.X/cellSize);
		int cellY = (int)(newPoint.Y/cellSize);
		
		FVector2D gridSize(FMath::CeilToInt(dimensions.X/cellSize), FMath::CeilToInt(dimensions.Y/cellSize));
		int searchStartX = FMath::Max(0,cellX -2);
		int searchEndX = FMath::Min(cellX+2,gridSize.X-1);
		int searchStartY = FMath::Max(0,cellY -2);
		int searchEndY = FMath::Min(cellY+2,gridSize.Y-1);

		for (int x = searchStartX; x <= searchEndX; x++) {
			for (int y = searchStartY; y <= searchEndY; y++) {
				int pointIndex = grid[ArrayCoords(x,y,gridSize.X)]-1;
				if (pointIndex != -1) {
					float sqrDst = (newPoint - points[pointIndex]).SquaredLength();
					if (sqrDst < radius*radius) {
						return false;
					}
				}
			}
		}
		return true;
	}
	return false;
};





TArray<FVector2D>  PoissonDiscSampling::PoissonDiscGenerator(float radius, FVector2D dimensions, int RejectionRate = 30)
{
	float cellSize = radius/sqrt(2);
	FVector2D gridSize(FMath::CeilToInt(dimensions.X/cellSize), FMath::CeilToInt(dimensions.Y/cellSize));
	TArray<int> grid;
	grid.Init(0,gridSize.X * gridSize.Y);
	TArray<FVector2D> points;
	TArray<FVector2D> spawnPoints;

	spawnPoints.Add(dimensions/2); // Add the center to spawnPoints;
	while(spawnPoints.Num() > 0)
	{
		int spawnIndex = FMath::RandRange(0,spawnPoints.Num() - 1);
		FVector2D spawnCenter = spawnPoints[spawnIndex];
		bool candidateAccepted = false;
		
		for(int i = 0; i < RejectionRate; i++)
		{
			float angle = UKismetMathLibrary::RandomFloat() * PI * 2; // Get a random angle in Radians
			FVector2D direction(sin(angle),cos(angle));
			FVector2D newPoint = spawnCenter + direction * FMath::RandRange(radius,radius * 2);

			if (IsValid(newPoint, dimensions, cellSize, radius, points, grid)) {
				points.Add(newPoint);
				spawnPoints.Add(newPoint);
				grid[ArrayCoords((int)(newPoint.X/cellSize),(int)(newPoint.Y/cellSize),gridSize.X)] = points.Num();
				candidateAccepted = true;
				break;
			}
		}
		if (!candidateAccepted) {
			spawnPoints.RemoveAt(spawnIndex);
		}
	}
	return points;
}*/


TArray<FVector2D>  PoissonDiscSampling::PoissonDiscGenerator(float radius, FVector2D dimensions, int RejectionRate = 30)
{

	//make the size of each cell allow only one possible point
	float cellSize = radius / sqrt(2);

	// initilise the grid with unvisited points
	FVector2D gridSize(FMath::CeilToInt(dimensions.X/cellSize), FMath::CeilToInt(dimensions.Y/cellSize));
	TArray<int> grid;
	grid.Init(0,gridSize.X * gridSize.Y); // The grid is implemented as a 1D array so we need a function to convert from 2D to 1D

	//Add a random point to the list to begin the generation
	TArray<FVector2D> ActivePoints;
	TArray<FVector2D> FinalPoints;
	
	//ActivePoints.Add(FVector2D(FMath::RandRange(0.0,dimensions.X),FMath::RandRange(0.0,dimensions.Y))); // Add a random position to spawnPoints;
	ActivePoints.Add(dimensions/2); // Add the center to spawnPoints;
	
	while(ActivePoints.Num() > 0)
	{
		//Chose a random point
		int randomIndex = FMath::RandRange(0,ActivePoints.Num() - 1);
		FVector2D ActivePoint = ActivePoints[randomIndex];
		bool isValid = false;
		
		//randomly check the surrounding area for valid points, X amount of time
		for(int i = 0; i < RejectionRate;i++)
		{
			float angle = UKismetMathLibrary::RandomFloat() * PI * 2;
			FVector2D direction(sin(angle),cos(angle));
			FVector2D newPoint = ActivePoint + direction * FMath::RandRange(radius,radius * 2);
			
			if(IsValid(newPoint, dimensions, cellSize, radius, FinalPoints, grid)) // Check if the new point is valid and pass the necessary info 
			{
				FinalPoints.Add(newPoint);
				ActivePoints.Add(newPoint);
				grid[ArrayCoords((int)(newPoint.X/cellSize),(int)(newPoint.Y/cellSize),gridSize.X)] = FinalPoints.Num();
				isValid = true;
				break;
			}
		}
		if(!isValid) // if the point wasnt accepted then remove from the active points
		{
			ActivePoints.RemoveAt(randomIndex);
		}
	}
	return FinalPoints;
}


bool PoissonDiscSampling::IsValid(FVector2D newPoint,FVector2D dimensions, float cellSize,float radius, const TArray<FVector2D>& FinalPoints, const TArray<int>& grid)
{
	// check if the point is even within the valid dimensions
	if(newPoint.X >= 0 && newPoint.X < dimensions.X && newPoint.Y >= 0 && newPoint.Y < dimensions.Y)
	{
		//convert from world position to the grid array index position
		int positionX = (int)(newPoint.X/cellSize);
		int positionY = (int)(newPoint.Y/cellSize);
		
		FVector2D gridSize(FMath::CeilToInt(dimensions.X/cellSize), FMath::CeilToInt(dimensions.Y/cellSize));

		//We look at the surrounding grid points and check if the points overlaps the its radius
		for (int x = FMath::Max(0,positionX -2); x <= FMath::Min(positionX+2,gridSize.X-1); x++) {
			for (int y = FMath::Max(0,positionY -2); y <= FMath::Min(positionY+2,gridSize.Y-1); y++) {
				int index = grid[ArrayCoords(x,y,gridSize.X)] - 1;
				if (index != -1) {
					float sqrDst = (newPoint - FinalPoints[index]).SquaredLength();
					if (sqrDst < radius*radius) {
						return false;
					}
				}
			}
		}
		
		return true;
	}

	return false;
};
