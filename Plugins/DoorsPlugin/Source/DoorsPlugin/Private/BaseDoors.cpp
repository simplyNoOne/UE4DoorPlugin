// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDoors.h"
#include "DoorComponent.h"
#include "DoorKey.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABaseDoors::ABaseDoors()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = RootComp;

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>("DoorFrame");
	DoorFrame->SetupAttachment(GetRootComponent());

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh");
	DoorMesh->SetupAttachment(DoorFrame);

	Side1 = CreateDefaultSubobject<UBoxComponent>("Side1");
	Side1->SetupAttachment(GetRootComponent());

	Side2 = CreateDefaultSubobject<UBoxComponent>("Side2");
	Side2->SetupAttachment(GetRootComponent());

	InteractionArea = CreateDefaultSubobject<UBoxComponent>("InteractionArea");
	InteractionArea->SetupAttachment(GetRootComponent());

	Point1 = CreateDefaultSubobject< USceneComponent>("RefPoint1");
	Point1->SetupAttachment(GetRootComponent());
	Point2 = CreateDefaultSubobject< USceneComponent>("RefPoint2");
	Point2->SetupAttachment(GetRootComponent());

	AINavProxy = CreateDefaultSubobject<UChildActorComponent>("NavProxy");

	DoorComponent = CreateDefaultSubobject<UDoorComponent>("DoorComponent");
	DoorComponent->Side1 = Side1->GetComponentLocation();
	DoorComponent->Side2 = Side2->GetComponentLocation();
	DoorComponent->Owner = this;



	bIsOpen = false;
	bIsBusy = false;

	bUnlocked = true;

}

// Called when the game starts or when spawned
void ABaseDoors::BeginPlay()
{
	AActor::BeginPlay();

	InteractionArea->OnComponentBeginOverlap.AddDynamic(this, &ABaseDoors::InteractionAreaEntered);
	InteractionArea->OnComponentEndOverlap.AddDynamic(this, &ABaseDoors::InteractionAreaExited);

	DoorComponent->OpenDoorMsg.AddDynamic(this, &ABaseDoors::OpenDoor);
	DoorComponent->CloseDoorMsg.AddDynamic(this, &ABaseDoors::CloseDoor);

	if(Key)
		Key->KeyPickedUp.AddDynamic(this, &ABaseDoors::UnlockDoor);

	
}

void ABaseDoors::InteractionAreaEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bUnlocked)
	{
		
			DoorComponent->InteractionAreaEntered(OtherActor);
	}
		
	
}

void ABaseDoors::InteractionAreaExited(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bUnlocked)
	{
		
			DoorComponent->InteractionAreaExited(OtherActor);
	}
		
}

void ABaseDoors::UnlockDoor()
{
	bUnlocked = true;
}

void ABaseDoors::OpenDoor_Implementation()
{
	bIsBusy = true;
}

void ABaseDoors::CloseDoor_Implementation()
{
	bIsBusy = true;
}

void ABaseDoors::DoorsInteracted()
{
	DoorComponent->PlayerInteracted();
}

void ABaseDoors::DoorsClosed()
{
	bIsOpen = false;
	bIsBusy = false;
}

void ABaseDoors::DoorsOpened()
{
	bIsOpen = true;
	bIsBusy = false;
}

// Called every frame
void ABaseDoors::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);

}

bool ABaseDoors::IsOpen()
{
	return bIsOpen;
}

