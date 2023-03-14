// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 02/02/23
//

#include "VertexEdge.h"



VertexEdge::VertexEdge()
{
}

VertexEdge::VertexEdge(FVector2D newA, FVector2D newB)
{
	A = newA;
	B = newB;
}

VertexEdge::~VertexEdge()
{
}


bool VertexEdge::AlmostEqual(VertexEdge left, VertexEdge right) {
	return AlmostEqual(left.A, right.A) && AlmostEqual(left.B, right.B) || AlmostEqual(left.A, right.B) && AlmostEqual(left.B, right.A);
}
  

bool VertexEdge::AlmostEqual(float x, float y) {
	//return abs(x - y) <= FLT_EPSILON * abs(x + y) * 2 || abs(x - y) < 0.00001;
	return FMath::IsNearlyEqual(x,y);
}

bool VertexEdge::AlmostEqual(FVector2D left, FVector2D right) {
	return AlmostEqual(left.X, right.X) && AlmostEqual(left.Y, right.Y);
}