// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 02/02/23
//

#include "Triangles.h"

Triangle::Triangle()
{
}

Triangle::Triangle(FVector2D edge1, FVector2D edge2, FVector2D edge3)
{
	Vertex[0] = edge1;
	Vertex[1] = edge2;
	Vertex[2] = edge3;
}


Triangle::~Triangle()
{

}


//Circum Circle is a circle within the triangle that touches all edges
bool Triangle::IsInCircumCircle(FVector2D position)
{
	const FVector2D A = Vertex[0];
	const FVector2D B = Vertex[1];
	const FVector2D C = Vertex[2];

	const float LengthA = A.SquaredLength();
	const float LengthB = B.SquaredLength();
	const float LengthC = C.SquaredLength();

	const float CircumX = (LengthA * (C.Y - B.Y) + LengthB * (A.Y- C.Y) + LengthC * (B.Y- A.Y)) / (A.X * (C.Y- B.Y) + B.X * (A.Y - C.Y) + C.X * (B.Y - A.Y));
	const float CircumY = (LengthA * (C. X- B.X) + LengthB * (A.X - C.X) + LengthC * (B.X - A.X)) / (A.Y * (C.X - B.X) + B.Y * (A.X - C.X) + C.Y * (B.X - A.X));

	FVector2D Center(CircumX / 2, CircumY / 2); // Get the Center Point of the circle
	float Radius = FVector2D(A - Center).SquaredLength(); // Get distance from A to center, i.e the radius
	float Distance = FVector2D(position - Center).SquaredLength();
	return Distance <= Radius;


	///////////////////// old implementation, is useful for context
	
	FVector2D midpoint1(Vertex[0] - Vertex[1]);
	midpoint1 = midpoint1 / 2;
	midpoint1 = midpoint1 +Vertex[1];

	FVector2D midpoint2(Vertex[0] - Vertex[2]);
	midpoint2 = midpoint2 / 2;
	midpoint2 = midpoint2 + Vertex[2];

	FVector2D tangent1(-(Vertex[0].X - Vertex[1].X), Vertex[0].Y - Vertex[1].Y);
	FVector2D tangent2(-(Vertex[0].X - Vertex[2].X), Vertex[0].Y - Vertex[2].Y);

	float gradient1 = ((midpoint1.Y + tangent1.Y) - (midpoint1.Y)) / ((midpoint1.X + tangent1.X) - (midpoint1.X));
	float gradient2 = ((midpoint2.Y + tangent2.Y) - (midpoint2.Y)) / ((midpoint2.X + tangent2.X) - (midpoint2.X));

	float intercept1 = midpoint1.Y - gradient1 * midpoint1.X;
	float intercept2 = midpoint2.Y - gradient2 * midpoint2.X;

	float centerX = (intercept2 - intercept1) / (gradient1 - gradient2);
	float centerY = gradient1 * centerX + intercept1;

	FVector2D Diameter(Vertex[0] - FVector2D(centerX, centerY));
	float length = Diameter.Length();

	FVector2D distanceVec(position - FVector2D(centerX, centerY));
	float distance = distanceVec.Length();

	if(distance > (length/2))// Calculate if a point is in the circle
	{
		return true;
	}
	

	return false;
}

bool Triangle::ContainsVertex(FVector2D vertex)
{
	return FVector2D::Distance(vertex, Vertex[0]) < 0.01f
		 || FVector2D::Distance(vertex, Vertex[1]) < 0.01f
		 || FVector2D::Distance(vertex, Vertex[2]) < 0.01f;
}
