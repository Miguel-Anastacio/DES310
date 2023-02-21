// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 02/02/23
//

#include "DelaunayTriangulation.h"

DelaunayTriangulation::DelaunayTriangulation()
{
}

DelaunayTriangulation::~DelaunayTriangulation()
{
}


TArray<Triangle>  DelaunayTriangulation::GenerateTriangulation(const TArray<FVector2D>& vect)
{
	TArray<Triangle> triangleList;

	float minX = vect[0].X;
	float minY = vect[0].Y;
	float maxX = minX;
	float maxY = minY;

	for (auto vertex : vect) { // find the min and max XY positions of all the vertexes
		if (vertex.X < minX) minX = vertex.X;
		if (vertex.X > maxX) maxX = vertex.X;
		if (vertex.Y < minY) minY = vertex.Y;
		if (vertex.Y > maxY) maxY = vertex.Y;
	}

	float dx = maxX - minX;
	float dy = maxY - minY;
	float deltaMax = FMath::Max(dx, dy) * 2;
	
	FVector2D Edge1(minX - 1, minY - 1);
	FVector2D Edge2(minX - 1, maxY + deltaMax);
	FVector2D Edge3(maxX + deltaMax, minY - 1);

	Triangle SuperTriangle(Edge1, Edge2, Edge3);
	triangleList.Add(SuperTriangle);

	for (auto vertex : vect)
	{
		TArray<VertexEdge> polygon;
		
		for(int i = 0; i < triangleList.Num();i++)
		{
			if(triangleList[i].IsInCircumCircle(vertex))
			{
				triangleList[i].isBad = true;
				polygon.Add(VertexEdge(triangleList[i].Vertex[0],triangleList[i].Vertex[1]));
				polygon.Add(VertexEdge(triangleList[i].Vertex[1],triangleList[i].Vertex[2]));
				polygon.Add(VertexEdge(triangleList[i].Vertex[2],triangleList[i].Vertex[0]));
			}

		}

		for (int32 Index = triangleList.Num()-1; Index >= 0; --Index)
		{
			if (triangleList[Index].isBad)
			{
				const bool bAllowShrinking = false;
				triangleList.RemoveAt(Index, 1, bAllowShrinking);
			}
		}


		for (int i = 0; i < polygon.Num(); i++) {
			for (int j = i + 1; j < polygon.Num(); j++) {
				if (VertexEdge::AlmostEqual(polygon[i], polygon[j])) {
					polygon[i].isBad = true;
					polygon[j].isBad = true;
				}
			}
		}
		
		for (int32 Index = polygon.Num()-1; Index >= 0; --Index)
		{
			if (polygon[Index].isBad)
			{
				const bool bAllowShrinking = false;
				polygon.RemoveAt(Index, 1, bAllowShrinking);
			}
		}
		
		for (auto edge : polygon) {
			triangleList.Add(Triangle(edge.A, edge.B, vertex));
		}
	}

	for(int i = 0; i < triangleList.Num();i++)
	{
		if(triangleList[i].ContainsVertex(SuperTriangle.Vertex[0]) || triangleList[i].ContainsVertex(SuperTriangle.Vertex[1]) || triangleList[i].ContainsVertex(SuperTriangle.Vertex[2]))
		{
			triangleList[i].isBad = true;
		}
	}

	for (int32 Index = triangleList.Num()-1; Index >= 0; --Index)
	{
		if (triangleList[Index].isBad)
		{
			const bool bAllowShrinking = false;
			triangleList.RemoveAt(Index, 1, bAllowShrinking);
		}
	}

	return triangleList;
	
	/*
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("Line Drawn")));

	UWorld* world = GEngine->GetWorld();
	if (GetWorld())
	{

	}
	*/


}