// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Vendor.generated.h"

class UInventoryComponent;

UCLASS()
class DES310_PROJECT_API AVendor : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(VisibleAnywhere)
	UInventoryComponent* InventoryComponent;


	UFUNCTION(BlueprintCallable)
	UInventoryComponent* GetVendorInventory() {
		return InventoryComponent;
	}
	// Sets default values for this actor's properties
	AVendor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
