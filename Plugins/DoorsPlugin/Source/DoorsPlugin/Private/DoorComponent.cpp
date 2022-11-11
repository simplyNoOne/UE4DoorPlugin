// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorComponent.h"
#include "BaseDoors.h"
#include "PlayerActionsInterface.h"
#include "AIActionsInterface.h"

// Sets default values for this component's properties
UDoorComponent::UDoorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bCanBeInteractedWith = false;
		
	numActorsUsing = 0;
	
	// ...
}


// Called when the game starts
void UDoorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDoorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDoorComponent::InteractionAreaEntered(AActor* OtherActor)
{
	if (DoorFunction == EDoorFunction::EDF_Interactive || DoorFunction == EDoorFunction::EDF_Teleporting)
	{
		if (DoorType == EDoorType::EDT_PlayerDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UPlayerActionsInterface>())
				bCanBeInteractedWith = true;
		}
		if (DoorType == EDoorType::EDT_AIDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UAIActionsInterface>())
			{
				numActorsUsing++;
				if (bCustomAIInteractionCondition)
				{
					if (IAIActionsInterface::Execute_CanAIOpenDoor(OtherActor, Owner))
						AIInteracted();
				}
				else
				{
					FVector AITarget = IAIActionsInterface::Execute_GetAIDestination(OtherActor);
					if(OpenBasedOnAIDestination(AITarget, OtherActor->GetActorLocation()))
						AIInteracted();
				}
		
			}
		}
		
	}
	else if (DoorFunction == EDoorFunction::EDF_Automatic)
	{
		if (DoorType != EDoorType::EDT_DEFAULT)
		{
			if (OtherActor->Implements<UPlayerActionsInterface>() || OtherActor->Implements<UAIActionsInterface>())
				OpenAutomaticDoor();
		}
		
	}
	
}

void UDoorComponent::InteractionAreaExited(AActor* OtherActor)
{
	if (DoorFunction == EDoorFunction::EDF_Interactive || DoorFunction == EDoorFunction::EDF_Teleporting)
	{
		if (DoorType == EDoorType::EDT_PlayerDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UPlayerActionsInterface>())
				bCanBeInteractedWith = false;
		}
		if (DoorType == EDoorType::EDT_AIDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UAIActionsInterface>())
			{
				numActorsUsing--;
				if(numActorsUsing == 0)
					AIInteracted();
			}
		}

	}
	else if (DoorFunction == EDoorFunction::EDF_Automatic)
	{
		if (DoorType != EDoorType::EDT_DEFAULT)
		{
			if (OtherActor->Implements<UPlayerActionsInterface>() || OtherActor->Implements<UAIActionsInterface>())
				CloseAutomaticDoor();
		}
		
	}
}

void UDoorComponent::OpenAutomaticDoor()
{
	numActorsUsing++;
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
	numActorsUsing--;
	if (numActorsUsing == 0)
	{
		if (Owner->bIsOpen)
		{
			if (!(Owner->bIsBusy))
			{
				CloseDoorMsg.Broadcast();
			}
		}
			
	}
}

void UDoorComponent::AIInteracted()
{
	
	if (Owner->bIsOpen)
	{
		//numActorsUsing--;
		if (numActorsUsing == 0)
		{
			if (!(Owner->bIsBusy))
			{
				CloseDoorMsg.Broadcast();
			}
		}
	}
	else
	{
		//numActorsUsing++;
		if (!(Owner->bIsBusy))
		{
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
				CloseDoorMsg.Broadcast();
			}
		}
		else
		{
			if (!(Owner->bIsBusy))
			{
				OpenDoorMsg.Broadcast();
			}
		}
	}
}

bool UDoorComponent::OpenBasedOnAIDestination(FVector Destination, FVector ActorLocation)
{
	FVector point1 = Owner->Point1->GetRelativeLocation();
	FVector point2 = Owner->Point2->GetRelativeLocation();

	return (((Destination - point1).Size() < (Destination - point2).Size()) != ((ActorLocation - point1).Size() > (ActorLocation - point2).Size()));

}

