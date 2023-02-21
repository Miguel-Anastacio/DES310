// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 02/02/23
//

#pragma once

#include "CoreMinimal.h"
#include "Triangles.h"
#include "VertexEdge.h"

/**
 * 
 */
class DES310_PROFPROJECT_API DelaunayTriangulation
{
public:
	DelaunayTriangulation();
	~DelaunayTriangulation();

	static TArray<Triangle>  GenerateTriangulation(const TArray<FVector2D>& vect);
};
