// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 02/02/23
//

#pragma once
#include "AStarPoint.h"
#include <set>
#include "CoreMinimal.h"

/**
 * 
 */
class DES310_PROJECT_API  AStar
{
public:
	AStar();
	~AStar();


	void search(int beginID, int endID);
	float heuristic(AStarPoint start);
	float distance(AStarPoint start, AStarPoint end);
	void addNeighbours(AStarPoint c);
	int AddPoint(FVector2D newPoint);
	void ConnectPoints(int id, int to_id ,bool directional = true);
	int findPoint(FVector2D newPoint);

	TArray<AStarPoint> points;
	TArray<FVector2D> path;

	AStarPoint end;
	AStarPoint begin;
	
private:
	
	std::set<AStarPoint> open;
	std::set<AStarPoint> closed;
	

	
};
