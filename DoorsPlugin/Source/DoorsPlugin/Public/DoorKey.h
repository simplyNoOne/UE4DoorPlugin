// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorKey.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKeyPickedUp, ADoorKey*, Key);

UCLASS()
class DOORSPLUGIN_API ADoorKey : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorKey();

	UPROPERTY(BlueprintAssignable, Category = "CustomEvents")
	FKeyPickedUp KeyPickedUp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	void Interacted();
};
