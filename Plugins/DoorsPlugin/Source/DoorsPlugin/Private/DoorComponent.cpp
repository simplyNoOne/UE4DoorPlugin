// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorComponent.h"
#include "BaseDoors.h"
#include "DoorKey.h"
#include "PlayerActionsInterface.h"
#include "AIActionsInterface.h"

// Sets default values for this component's properties
UDoorComponent::UDoorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bCanBeInteractedWith = false;
	bCanTeleport = false;
	numActorsUsing = 0;
	bUnlocked = true;
	bGetTeleportLocationAtRuntime = false;
	bWasJustTeleportedTo = false;
	
	// ...
}


// Called when the game starts
void UDoorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Keys.Num() != 0)
		for(auto Key : Keys)
		Key->KeyPickedUp.AddDynamic(this, &UDoorComponent::KeyCollected);
	
}


// Called every frame
void UDoorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDoorComponent::InteractionAreaEntered(AActor* OtherActor)
{
	numActorsUsing++;
	if (bWasJustTeleportedTo) {
		bWasJustTeleportedTo = false;
		return;
	}
	if (DoorFunction == EDoorFunction::EDF_Interactive || DoorFunction == EDoorFunction::EDF_Teleporting)
	{
		if (DoorType == EDoorType::EDT_PlayerDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UPlayerActionsInterface>())
			{
				numActorsUsing++;
				bCanBeInteractedWith = true;
			}
		}
		if (DoorType == EDoorType::EDT_AIDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UAIActionsInterface>())
			{
				numActorsUsing++;
				if (bCustomAIInteractionCondition)
				{
					if (IAIActionsInterface::Execute_CanAIOpenDoor(OtherActor, Owner))
						AIInteracted(true);
				}
				else
				{
					FVector AITarget = IAIActionsInterface::Execute_GetAIDestination(OtherActor);
					if(OpenBasedOnAIDestination(AITarget, OtherActor->GetActorLocation()))
						AIInteracted(true);
				}
		
			}
		}
		
	}
	else if (DoorFunction == EDoorFunction::EDF_Automatic)
	{
		if (DoorType == EDoorType::EDT_PlayerDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UPlayerActionsInterface>())
			{
				numActorsUsing++;
				OpenAutomaticDoor();
			}
		}
		if (DoorType == EDoorType::EDT_AIDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UAIActionsInterface>())
			{
				numActorsUsing++;
				OpenAutomaticDoor();

			}
		}
		
	}
	
}

void UDoorComponent::InteractionAreaExited(AActor* OtherActor)
{

	numActorsUsing--;
	if (DoorFunction == EDoorFunction::EDF_Interactive || DoorFunction == EDoorFunction::EDF_Teleporting)
	{
		if (DoorType == EDoorType::EDT_PlayerDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UPlayerActionsInterface>()) {
				numActorsUsing--;
				bCanBeInteractedWith = false;
			}
		}
		if (DoorType == EDoorType::EDT_AIDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UAIActionsInterface>())
			{
				numActorsUsing--;
				if(numActorsUsing == 0)
					AIInteracted(false);
			}
		}

	}
	else if (DoorFunction == EDoorFunction::EDF_Automatic)
	{
		if (DoorType == EDoorType::EDT_PlayerDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UPlayerActionsInterface>())
			{
				numActorsUsing--;
				CloseAutomaticDoor();
			}
		}
		if (DoorType == EDoorType::EDT_AIDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UAIActionsInterface>())
			{
				numActorsUsing--;
				CloseAutomaticDoor();
			}
		}
		
	}
}

void UDoorComponent::OpenAutomaticDoor()
{
	//numActorsUsing++;
	if (!(Owner->bIsOpen))
	{
		if (!(Owner->bIsBusy))
		{
			OpenDoorMsg.Broadcast();
		}
			
	}
		
}

void UDoorComponent::CloseAutomaticDoor()
{
	
	//numActorsUsing--;
	if (numActorsUsing <= 0)
	{
		numActorsUsing = 0;
		if (Owner->bIsOpen)
		{
			if (!(Owner->bIsBusy))
			{
				CloseDoorMsg.Broadcast();
			}
		}
			
	}
}

void UDoorComponent::AIInteracted(bool open)
{
	
	if (Owner->bIsOpen && !open)
	{
		if (numActorsUsing <= 0)
		{
			numActorsUsing = 0;
			if (!(Owner->bIsBusy))
			{
				if(DoorFunction == EDoorFunction::EDF_Teleporting)
					bCanTeleport = false;
				CloseDoorMsg.Broadcast();
			}
		}
	}
	else if(!Owner->bIsOpen)
	{
	
		if (!(Owner->bIsBusy) && open)
		{
			if (DoorFunction == EDoorFunction::EDF_Teleporting)
				bCanTeleport = true;
			OpenDoorMsg.Broadcast();
		}
	}
}

void UDoorComponent::PlayerInteracted()
{
	if (bCanBeInteractedWith)
	{
		if (Owner ->bIsOpen)
		{
			if (!(Owner->bIsBusy))
			{
				if (DoorFunction == EDoorFunction::EDF_Teleporting)
					bCanTeleport = false;
				CloseDoorMsg.Broadcast();
			}
		}
		else
		{
			if (!(Owner->bIsBusy))
			{
				if (DoorFunction == EDoorFunction::EDF_Teleporting)
					bCanTeleport = true;
				OpenDoorMsg.Broadcast();
			}
		}
	}
}

void UDoorComponent::TeleportTriggered(AActor* ActorToTeleport)
{
	FTransform TransformToTeleport;
	FVector LocationToTeleport;
	if (bGetTeleportLocationAtRuntime) 
	{
		if (ActorToTeleport->Implements<UPlayerActionsInterface>())
		{
			TransformToTeleport = IPlayerActionsInterface::Execute_GetTransformToTeleportTo(ActorToTeleport);
			//LocationToTeleport = IPlayerActionsInterface::Execute_GetLocationToTeleportTo(ActorToTeleport);
		}
		else if(ActorToTeleport->Implements<UAIActionsInterface>())
		{
			TransformToTeleport = IAIActionsInterface::Execute_GetTransformToTeleportTo(ActorToTeleport);
			//LocationToTeleport = IAIActionsInterface::Execute_GetLocationToTeleportTo(ActorToTeleport);
		}
	}
	else if (OtherDoor)
	{
		//LocationToTeleport = OtherDoor->LocationToTeleportActor->GetComponentLocation();
		OtherDoor->DoorComponent->bWasJustTeleportedTo = true;
		TransformToTeleport.SetLocation (OtherDoor->LocationToTeleportActor->GetComponentLocation());
		TransformToTeleport.SetRotation(OtherDoor->LocationToTeleportActor->GetComponentQuat());
	}
	else
	{
		//LocationToTeleport = TeleportingLocation;
		TransformToTeleport.SetLocation(TeleportingLocation);
	}
	UE_LOG(LogTemp, Warning, TEXT("YEEEET"))
	
	
	ActorToTeleport->SetActorLocation(TransformToTeleport.GetLocation(), false, nullptr, ETeleportType::ResetPhysics);
	ActorToTeleport->SetActorRotation(TransformToTeleport.GetRotation());

	bCanTeleport = false;
	CloseDoorMsg.Broadcast();
}

void UDoorComponent::DoorActionFinished()
{
	if (DoorFunction == EDoorFunction::EDF_Automatic && numActorsUsing <= 0)
		CloseDoorMsg.Broadcast();
}

bool UDoorComponent::OpenBasedOnAIDestination(FVector Destination, FVector ActorLocation)
{
	FVector point1 = Owner->Point1->GetComponentLocation();
	FVector point2 = Owner->Point2->GetComponentLocation();

	return (((Destination - point1).Size() < (Destination - point2).Size()) == ((ActorLocation - point1).Size() > (ActorLocation - point2).Size()));

}

void UDoorComponent::KeyCollected(ADoorKey* Key)
{
	Keys.Remove(Key);
	if(Keys.Num()==0)
		bUnlocked = true;
}

