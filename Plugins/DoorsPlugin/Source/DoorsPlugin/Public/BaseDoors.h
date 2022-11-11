// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseDoors.generated.h"

class UStaticMeshComponent;
class UChildActorComponent;
class UBoxComponent;
class UDoorComponent;
class ADoorKey;

UCLASS()
class DOORSPLUGIN_API ABaseDoors : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	USceneComponent* RootComp;

	UPROPERTY(EditDefaultsOnly, Category = "Physical")
	UStaticMeshComponent* DoorFrame;

	UPROPERTY(EditDefaultsOnly, Category = "Physical")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Physical")
	UBoxComponent* Side1;

	UPROPERTY(EditDefaultsOnly, Category = "Physical")
	UBoxComponent* Side2;


	UPROPERTY(EditDefaultsOnly, Category = "Physical")
	UBoxComponent* InteractionArea;

	UPROPERTY(VisibleAnywhere, Category = "DoorFunctionality")
	UChildActorComponent* AINavProxy;

	UPROPERTY(VisibleAnywhere, Category = "DoorFunctionality")
	UDoorComponent* DoorComponent;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "DoorFunctionality")
	bool bUnlocked;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DoorFunctionality")
	ADoorKey* Key;

	
public:	
	// Sets default values for this actor's properties
	ABaseDoors();

	UPROPERTY()
	USceneComponent* Point1;

	UPROPERTY()
	USceneComponent* Point2;

	bool bIsOpen;
	bool bIsBusy;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void InteractionAreaEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void InteractionAreaExited(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void UnlockDoor();

	UFUNCTION(BlueprintNativeEvent, Category = "CustomEvents")
	void OpenDoor();

	UFUNCTION(BlueprintNativeEvent, Category = "CustomEvents")
	void CloseDoor();

	UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	void DoorsInteracted();

	UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	void DoorsClosed();

	UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	void DoorsOpened();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsOpen();

};
