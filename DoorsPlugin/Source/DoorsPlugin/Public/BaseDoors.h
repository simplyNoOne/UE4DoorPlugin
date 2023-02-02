// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseDoors.generated.h"

class UStaticMeshComponent;
class UChildActorComponent;
class UBoxComponent;
class USphereComponent;
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
	UBoxComponent* TeleportingTrigger;

	UPROPERTY(EditDefaultsOnly, Category = "Physical")
	UBoxComponent* InteractionArea;

	UPROPERTY(EditDefaultsOnly, Category = "Physical")
	UBoxComponent* AdditionalAIInteractionTrigger;


	UPROPERTY(VisibleAnywhere, Category = "DoorFunctionality")
	UChildActorComponent* AINavProxy;

	


	
public:	
	// Sets default values for this actor's properties
	ABaseDoors();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "DoorFunctionality")
	UDoorComponent* DoorComponent;

	UPROPERTY()
	USceneComponent* Point1;

	UPROPERTY()
	USceneComponent* Point2;

	UPROPERTY(EditDefaultsOnly, Category = "Physical")
	USceneComponent* LocationToTeleportActor;

	

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
	void AdditionalTriggerEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION(BlueprintNativeEvent, Category = "CustomEvents")
	void OpenDoor();

	UFUNCTION(BlueprintNativeEvent, Category = "CustomEvents")
	void CloseDoor();

	UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	void DoorsInteracted(AActor* InteractingActor);

	UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	void DoorsClosed();

	UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	void DoorsOpened();


	UFUNCTION()
	void Side2Overlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void Side1Overlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, Category = "CustomEvents")
	void Side1Entered(AActor* Actor);

	UFUNCTION(BlueprintNativeEvent, Category = "CustomEvents")
	void Side2Entered(AActor* Actor);

	UFUNCTION()
	void OnTeleportOverlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsOpen();

};
