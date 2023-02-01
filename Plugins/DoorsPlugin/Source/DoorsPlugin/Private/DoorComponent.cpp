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
	bShouldDoubleCheck = false;
	
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
				ActorsInProximity.Add(OtherActor);
				bCanBeInteractedWith = true;
			}
		}
		if (DoorType == EDoorType::EDT_AIDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UAIActionsInterface>())
			{
				ActorsInProximity.Add(OtherActor);
				if (bCustomAIInteractionCondition)
				{
					if (IAIActionsInterface::Execute_CanAIOpenDoor(OtherActor, Owner)) 
					{
						bShouldDoubleCheck = false;
						AIInteracted(true);
						
					}
					else 
					{
						ActorsInProximity.Remove(OtherActor);
						bShouldDoubleCheck = true;
					}
				}
				else
				{
					FVector AITarget = IAIActionsInterface::Execute_GetAIDestination(OtherActor);
					if (OpenBasedOnAIDestination(AITarget, OtherActor->GetActorLocation())) 
					{
						bShouldDoubleCheck = false;
						AIInteracted(true);
						
					}
					else
					{
						ActorsInProximity.Remove(OtherActor);
						bShouldDoubleCheck = true;
					}
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
				ActorsInProximity.Add(OtherActor);
				OpenAutomaticDoor();
			}
		}
		if (DoorType == EDoorType::EDT_AIDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UAIActionsInterface>())
			{
				ActorsInProximity.Add(OtherActor);
				OpenAutomaticDoor();

			}
		}
		
	}
	
}

void UDoorComponent::InteractionAreaExited(AActor* OtherActor)
{
	if (DoorFunction == EDoorFunction::EDF_Interactive || DoorFunction == EDoorFunction::EDF_Teleporting)
	{
		if (DoorType == EDoorType::EDT_PlayerDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UPlayerActionsInterface>()) {
				ActorsInProximity.Remove(OtherActor);
				bCanBeInteractedWith = false;
			}
		}
		if (DoorType == EDoorType::EDT_AIDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UAIActionsInterface>())
			{
				ActorsInProximity.Remove(OtherActor);
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
				ActorsInProximity.Remove(OtherActor);
				CloseAutomaticDoor();
			}
		}
		if (DoorType == EDoorType::EDT_AIDoor || DoorType == EDoorType::EDT_UniversalDoor)
		{
			if (OtherActor->Implements<UAIActionsInterface>())
			{
				ActorsInProximity.Remove(OtherActor);
				CloseAutomaticDoor();
			}
		}
		
	}
}

void UDoorComponent::OpenAutomaticDoor()
{
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
	
	if (ActorsInProximity.Num() == 0)
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
		if (ActorsInProximity.Num() == 0)
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

void UDoorComponent::PlayerInteracted(AActor* InteractingActor)
{
	
	if (ActorsInProximity.Contains(InteractingActor) && (DoorFunction == EDoorFunction::EDF_Interactive || DoorFunction == EDoorFunction::EDF_Teleporting))
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
	if (!ActorsInProximity.Contains(ActorToTeleport))
		return;
	FTransform TransformToTeleport;
	FVector LocationToTeleport;
	if (bGetTeleportLocationAtRuntime) 
	{
		if (ActorToTeleport->Implements<UPlayerActionsInterface>())
		{
			TransformToTeleport = IPlayerActionsInterface::Execute_GetTransformToTeleportTo(ActorToTeleport);
			
		}
		else if(ActorToTeleport->Implements<UAIActionsInterface>())
		{
			TransformToTeleport = IAIActionsInterface::Execute_GetTransformToTeleportTo(ActorToTeleport);
			
		}
	}
	else if (OtherDoor)
	{
		OtherDoor->DoorComponent->bWasJustTeleportedTo = true;
		TransformToTeleport.SetLocation (OtherDoor->LocationToTeleportActor->GetComponentLocation());
		TransformToTeleport.SetRotation(OtherDoor->LocationToTeleportActor->GetComponentQuat());
	}
	else
	{
		TransformToTeleport.SetLocation(TeleportingLocation);
	}
	
	
	ActorToTeleport->SetActorLocation(TransformToTeleport.GetLocation(), false, nullptr, ETeleportType::ResetPhysics);
	ActorToTeleport->SetActorRotation(TransformToTeleport.GetRotation(), ETeleportType::ResetPhysics);
	ActorToTeleport->SetActorScale3D(TransformToTeleport.GetScale3D());
	if (ActorToTeleport->Implements<UAIActionsInterface>()){
		IAIActionsInterface::Execute_AIActorTeleported(ActorToTeleport);
	}
	else {
		IPlayerActionsInterface::Execute_PlayerActorTeleported(ActorToTeleport);
	}

	bCanTeleport = false;
	CloseDoorMsg.Broadcast();
}

void UDoorComponent::DoorActionFinished()
{
	if (DoorFunction == EDoorFunction::EDF_Automatic  && ActorsInProximity.Num()  == 0)
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

