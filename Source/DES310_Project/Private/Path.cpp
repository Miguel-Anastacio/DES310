// Fill out your copyright notice in the Description page of Project Settings.
//
// Author: Liam Donald 2000495 14/02/23
//

#include "Path.h"
// Sets default values
APath::APath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APath::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

