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

	for (const auto vertex : vect) { // find the min and max XY positions of all the vertexes
		if (vertex.X < minX) minX = vertex.X;
		if (vertex.X > maxX) maxX = vertex.X;
		if (vertex.Y < minY) minY = vertex.Y;
		if (vertex.Y > maxY) maxY = vertex.Y;
	}

	//Find the max difference between the x and y axis
	const float DifferenceX = maxX - minX;
	const float DifferenceY = maxY - minY;
	const float DifferenceMax = FMath::Max(DifferenceX, DifferenceY) * 2;

	//Create the points that surround every single point
	const FVector2D Edge1(minX - 1, minY - 1);
	const FVector2D Edge2(minX - 1, maxY + DifferenceMax);
	const FVector2D Edge3(maxX + DifferenceMax, minY - 1);

	const Triangle SuperTriangle(Edge1, Edge2, Edge3); // Super Triangle encompasses every single point
	triangleList.Add(SuperTriangle);

	//Loop through all vertexes
	for (auto vertex : vect)
	{
		TArray<VertexEdge> polygon;
		
		for(int i = 0; i < triangleList.Num();i++)
		{
			if(triangleList[i].IsInCircumCircle(vertex))// if the current point is inside the triangles circle
			{
				triangleList[i].isBad = true; // signify that this triangle is bad, we know this since another point is within the triangle, therefor is can be split down more
				
				//Add the edges to the polygon list
				polygon.Add(VertexEdge(triangleList[i].Vertex[0],triangleList[i].Vertex[1])); 
				polygon.Add(VertexEdge(triangleList[i].Vertex[1],triangleList[i].Vertex[2]));
				polygon.Add(VertexEdge(triangleList[i].Vertex[2],triangleList[i].Vertex[0]));
			}

		}

		//Iterate through triangle list and remove any triangles that we know are bad
		for (int32 Index = triangleList.Num()-1; Index >= 0; --Index)
		{
			if (triangleList[Index].isBad)
			{
				const bool bAllowShrinking = false;
				triangleList.RemoveAt(Index, 1, bAllowShrinking);
			}
		}

		
		//TODO might get away with disabling 1
		for (int i = 0; i < polygon.Num(); i++) {
			for (int j = i + 1; j < polygon.Num(); j++) {
				if (VertexEdge::AlmostEqual(polygon[i], polygon[j])) {
					polygon[i].isBad = true;
					polygon[j].isBad = true;
				}
			}
		}

		//Iterate through the polygon and remove any edges that are bad
		for (int32 Index = polygon.Num()-1; Index >= 0; --Index)
		{
			if (polygon[Index].isBad)
			{
				const bool bAllowShrinking = false;
				polygon.RemoveAt(Index, 1, bAllowShrinking);
			}
		}

		//Anything remaining gets added to the triangle list
		for (auto edge : polygon) {
			triangleList.Add(Triangle(edge.A, edge.B, vertex));
		}
	}

	//Remove the beginning super triangle from the triangle list
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
}