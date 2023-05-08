// Fill out your copyright notice in the Description page of Project Settings.
/*
	Author: MIGUEL ANASTACIO 15/02/2023
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryComponent.h"
#include "Vendor.generated.h"


UCLASS()
class DES310_PROJECT_API AVendor : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Database)
	UItemData* AllItemsDatabase;

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

	void AddRandomItemFromArray(TArray<UItem*> items);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void CreateRandomInventoryFromAllItems();



};
