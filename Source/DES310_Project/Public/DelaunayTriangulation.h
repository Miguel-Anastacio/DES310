// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 02/02/23
//
//With reference
//https://en.wikipedia.org/wiki/Delaunay_triangulation
#pragma once

#include "CoreMinimal.h"
#include "Triangles.h"
#include "VertexEdge.h"

//Used to create evenly spread connections between provided points
class DES310_PROJECT_API DelaunayTriangulation
{
public:
	DelaunayTriangulation();
	~DelaunayTriangulation();

	static TArray<Triangle>  GenerateTriangulation(const TArray<FVector2D>& vect);
};
