// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 02/02/23
//

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class DES310_PROFPROJECT_API  AStarPoint
{
public:
	AStarPoint();
	AStarPoint(FVector2D newPosition,int _id){position = newPosition; previous = nullptr;id = _id; };
	~AStarPoint();

	bool operator== (const AStarPoint& c2) const
	{
		return  id == c2.id;
	};
	
	bool operator<(const AStarPoint& c2) const
	{
		return id < c2.id;
	}
	
	FVector2D position;
	float g,f;
	bool blocked = false;
	int id = 0;
	bool hasPrevious = false;

	AStarPoint* previous;
	TArray<AStarPoint*> Connectors;
};

FORCEINLINE uint32 GetTypeHash(const AStarPoint& MidiTime) 
{
	uint32 Hash = FCrc::MemCrc32(&MidiTime, sizeof(AStarPoint)); //huh
	return Hash;
};

/*bool operator== (const AStarPoint& c2) const
{
	return FMath::IsNearlyEqual(position.X, c2.position.X) && FMath::IsNearlyEqual(position.Y, c2.position.Y);
};

bool operator<(const AStarPoint& c2) const
{
	if(position.X < c2.position.X - FLT_EPSILON) return true;
	if(position.X < c2.position.X + FLT_EPSILON) return false;

	if(position.Y < c2.position.Y - FLT_EPSILON) return true;
	if(position.Y < c2.position.Y + FLT_EPSILON) return false;

	return false;
}*/