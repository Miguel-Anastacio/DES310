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


bool Triangle::IsInCircumCircle(FVector2D position)
{
	FVector2D a = Vertex[0];
	FVector2D b = Vertex[1];
	FVector2D c = Vertex[2];

	float ab = a.SquaredLength();
	float cd = b.SquaredLength();
	float ef = c.SquaredLength();

	float circumX = (ab * (c.Y - b.Y) + cd * (a.Y- c.Y) + ef * (b.Y- a.Y)) / (a.X * (c.Y- b.Y) + b.X * (a.Y - c.Y) + c.X * (b.Y - a.Y));
	float circumY = (ab * (c. X- b.X) + cd * (a.X - c.X) + ef * (b.X - a.X)) / (a.Y * (c.X - b.X) + b.Y * (a.X - c.X) + c.Y * (b.X - a.X));

	FVector2D circum(circumX / 2, circumY / 2);
	float circumRadius = FVector2D(a - circum).SquaredLength();
	float dist = FVector2D(position - circum).SquaredLength();
	return dist <= circumRadius;


	/////////////////////
	
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
