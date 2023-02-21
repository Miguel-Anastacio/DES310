// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 02/02/23
//

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class DES310_PROJECT_API  Triangle
{
public:
	Triangle();
	Triangle(FVector2D edge1, FVector2D edge2, FVector2D edge3);
	~Triangle();

	bool IsInCircumCircle(FVector2D position);
	bool ContainsVertex(FVector2D vertex);
	
	bool operator== (Triangle left)
	{
		return (left.Vertex[0] == Vertex[0] || left.Vertex[0] == Vertex[1] || left.Vertex[0] == Vertex[2])
			&& (left.Vertex[1] == Vertex[0] || left.Vertex[1] == Vertex[1] || left.Vertex[1] == Vertex[2])
			&& (left.Vertex[2] == Vertex[0] || left.Vertex[2] == Vertex[1] || left.Vertex[2] == Vertex[2]);
	}
	
	FVector2D Vertex[3];
	bool isBad = false;

};
