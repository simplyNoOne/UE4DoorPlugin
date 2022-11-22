// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DoorComponent.generated.h"

class ABaseDoors;
class ADoorKey;

UENUM(BlueprintType)
enum class EDoorType : uint8
{
	EDT_DEFAULT UMETA(Display = "Default"),

	EDT_PlayerDoor UMETA(Display = "PlayerDoor"),
	EDT_AIDoor UMETA(Display = "AIDoor"),
	EDT_UniversalDoor UMETA(Display = "UniversalDoor")
};

UENUM(BlueprintType)
enum class EDoorFunction : uint8
{
	EDF_DEFAULT UMETA(Display = "Default"),

	EDF_Interactive UMETA(Display = "InteractiveDoor"),
	EDF_Automatic UMETA(Display = "AutomaticDoor"),
	EDF_Teleporting UMETA(Display = "TeleportingDoor")	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCloseDoor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOpenDoor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOORSPLUGIN_API UDoorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDoorComponent();

	UPROPERTY(BlueprintAssignable, Category = "CustomEvents")
	FOpenDoor OpenDoorMsg;

	UPROPERTY(BlueprintAssignable, Category = "CustomEvents")
	FCloseDoor CloseDoorMsg;

	UPROPERTY(EditAnywhere, Category = "DoorData")
	EDoorType DoorType;

	UPROPERTY(EditAnywhere, Category = "DoorData")
	EDoorFunction DoorFunction;

	/*Location to teleport the actor to, if this actor's DoorFunction is EDF_Teleporting and OtherDoor is NOT SET*/
	UPROPERTY(EditAnywhere, Category = "TeleportingDoor")
	FVector TeleportingLocation;

	/*Doors to teleport the actor to, if this actor's DoorFunction is EDF_Teleporting and GetTeleportLocationAtRuntime is set to false*/
	UPROPERTY(EditAnywhere, Category = "TeleportingDoor")
	ABaseDoors* OtherDoor;

	/*set true if you want to provide condition for opening doors for AI yourself (you need to implement CanAIOpenDoor function). If left unchecked, AI movement target vector must be passed in GetAIDestination*/
	UPROPERTY(EditAnywhere, Category = "DoorData")
	bool bCustomAIInteractionCondition;

	/*if set to true, GetLocationToTeleportTo() must be implemented on actor using either interface, if left unset TeleportingDoors will move actor to OtherDoor (if set), or to TeleportingLocation*/
	UPROPERTY(EditAnywhere, Category = "TeleportingDoor")
	bool bGetTeleportLocationAtRuntime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorData")
	bool bUnlocked;

	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DoorData")
	TArray<ADoorKey*> Keys;


	ABaseDoors* Owner;

	bool bCanTeleport;

	bool bCanBeInteractedWith;

	bool bWasJustTeleportedTo;

	bool bShouldDoubleCheck;

	TArray<AActor*> ActorsInProximity;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "DoorData")
	int numActorsUsing;
	


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool OpenBasedOnAIDestination(FVector Destination, FVector ActorLocation);

	UFUNCTION()
	void KeyCollected(ADoorKey* Key);

	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InteractionAreaEntered(AActor* OtherActor);
	void InteractionAreaExited(AActor* OtherActor);

	void OpenAutomaticDoor();
	void CloseAutomaticDoor();

	void AIInteracted(bool open);
	void PlayerInteracted(AActor* InteractingActor);

	void TeleportTriggered(AActor* ActorToTeleport);

	void DoorActionFinished();
		
};
