// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 02/02/23
//

#include "AStar.h"

#include "AStarPoint.h"

AStar::AStar()
{
}

AStar::~AStar()
{
}

void AStar::search(int beginID, int endID)
{
	path.Empty();
	
	open.clear();
	closed.clear();
	
	if(beginID < 0 && endID < 0)
		return;
		
	end = points[endID];
	AStarPoint start = points[beginID];
	begin = start;
	start.g = 0;
	start.f = start.g + heuristic(start);

	open.insert(start);
	
	int counter = 0;

	while (!open.empty())
	{
		AStarPoint current;
		current.f = 1000000000;
		for (auto c : open)
		{
			if (current.f > c.f)
			{
				current = c;
			}
		}

		if (FMath::IsNearlyEqual(current.position.X , end.position.X) && FMath::IsNearlyEqual(current.position.Y , end.position.Y))
		{
			open.clear();
			closed.insert(current);
			break;
		}
	
		open.erase(current);
		addNeighbours(current);
		closed.insert(current);

		counter++;
	}
	

	AStarPoint trail;
	if (closed.find(end) != closed.end())
	{
		trail = *closed.find(end);
	}
	
	while(trail.previous)
	{
		if (closed.find(points[trail.previous->id])!= closed.end())
		{
			path.Add(trail.position);
			trail = *closed.find(points[trail.previous->id]);
		}

	}
	
	path.Add(trail.position);

}


float AStar::heuristic(AStarPoint start)
{
	return FVector2D::Distance(start.position,end.position);
}

float AStar::distance(AStarPoint start, AStarPoint e)
{
	return FVector2D::Distance(start.position,e.position);
}

int AStar::AddPoint(FVector2D newPoint)
{
	for(int i = 0; i < points.Num(); i++)
	{
		//if the point is already in the array then just return the index without adding
		if(FMath::IsNearlyEqual(points[i].position.X , newPoint.X) && FMath::IsNearlyEqual(points[i].position .Y , newPoint.Y))
		{
			return i;
		}

		/*if (newPoint == points[i].position)
		{
			return i;
		}*/
	}
	int id = points.Num(); 
	points.Add(AStarPoint(newPoint,id));
	return id;
}

int AStar::findPoint(FVector2D newPoint)
{
	for(int i = 0; i < points.Num(); i++)
	{
		if(FMath::IsNearlyEqual(points[i].position .X , newPoint.X) && FMath::IsNearlyEqual(points[i].position .Y , newPoint.Y))
		{
			return i;
		}
	}
	return -1;
}

void AStar::LockCurrentPath()
{
	for (int j = 0; j < path.Num() - 1; j++)
	{
		int id = findPoint(path[j]);
		if (id != -1 && j != 0) // point is not the first
		{
			points[id].blocked = true;
		}
	}
}

void AStar::ConnectPoints(int id, int to_id ,bool directional)
{
	//TODO add a check to see if connetion is already there, although it shouldnt change the output but would just be more accurate
	points[id].Connectors.Add(&points[to_id]);
	points[to_id].Connectors.Add(&points[id]);
	//if(directional)
}

void AStar::addNeighbours(AStarPoint point)
{

	std::set<AStarPoint>neighbours;

	for(int i = 0; i < point.Connectors.Num();i++)
	{
		if(!point.Connectors[i]->blocked)
		{
			AStarPoint newPoint = *(point.Connectors[i]);
			//TODO add where it came from... eg. it will be the point passed
			newPoint.previous = &points[point.id];
			newPoint.hasPrevious = true;
			neighbours.insert(newPoint);
		}
	}

	
	for (auto n : neighbours)
	{
		
		n.g = point.g + distance(n, point);
		n.f = n.g + heuristic(n);
		if (open.find(n) != open.end())
		{
			if (open.find(n)->f < n.f)
			{
				continue;
			}
		}
		if (closed.find(n) != closed.end())
		{
			if (closed.find(n)->f < n.f)
			{
				continue;
			}
		}
		open.insert(n);
		
	}
}
