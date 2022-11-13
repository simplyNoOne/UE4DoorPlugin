// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDoors.h"
#include "DoorComponent.h"
#include "DoorKey.h"
#include "PlayerActionsInterface.h"
#include "AIActionsInterface.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
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
	Point1->SetRelativeLocation(FVector(0.f, 50.f, 0.f));
	Point2 = CreateDefaultSubobject< USceneComponent>("RefPoint2");
	Point2->SetupAttachment(GetRootComponent());
	Point2->SetRelativeLocation(FVector(0.f, -50.f, 0.f));

	TeleportTrigger = CreateDefaultSubobject<USphereComponent>("TeleportingTrigger");
	TeleportTrigger->SetupAttachment(GetRootComponent());
	TeleportTrigger->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	TeleportTrigger->SetSphereRadius(10.f);

	AINavProxy = CreateDefaultSubobject<UChildActorComponent>("NavProxy");
	AINavProxy->SetupAttachment(GetRootComponent());

	DoorComponent = CreateDefaultSubobject<UDoorComponent>("DoorComponent");
	DoorComponent->Owner = this;


	bIsOpen = false;
	bIsBusy = false;

	//bUnlocked = true;

}

// Called when the game starts or when spawned
void ABaseDoors::BeginPlay()
{
	AActor::BeginPlay();

	InteractionArea->OnComponentBeginOverlap.AddDynamic(this, &ABaseDoors::InteractionAreaEntered);
	InteractionArea->OnComponentEndOverlap.AddDynamic(this, &ABaseDoors::InteractionAreaExited);

	DoorComponent->OpenDoorMsg.AddDynamic(this, &ABaseDoors::OpenDoor);
	DoorComponent->CloseDoorMsg.AddDynamic(this, &ABaseDoors::CloseDoor);

	Side1->OnComponentBeginOverlap.AddDynamic(this, &ABaseDoors::Side1Overlapped);
	Side2->OnComponentBeginOverlap.AddDynamic(this, &ABaseDoors::Side2Overlapped);

	TeleportTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABaseDoors::TeleportOverlapped);

}

void ABaseDoors::InteractionAreaEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (DoorComponent -> bUnlocked)
	{
			DoorComponent->InteractionAreaEntered(OtherActor);
	}
		
	
}

void ABaseDoors::InteractionAreaExited(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (DoorComponent-> bUnlocked)
	{
			DoorComponent->InteractionAreaExited(OtherActor);
	}
		
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

void ABaseDoors::TeleportOverlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (DoorComponent->DoorFunction == EDoorFunction::EDF_Teleporting) 
	{
		if(OtherActor->Implements<UAIActionsInterface>() || OtherActor->Implements<UPlayerActionsInterface>())
		UE_LOG(LogTemp, Warning, TEXT("TELEPORT OVERLAPPED"))
		if (DoorComponent->bCanTeleport)
			DoorComponent->TeleportTriggered(OtherActor);
	}
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

void ABaseDoors::Side1Overlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UAIActionsInterface>() || OtherActor->Implements<UPlayerActionsInterface>())
		Side1Entered(OtherActor);
}

void ABaseDoors::Side2Overlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UAIActionsInterface>() || OtherActor->Implements<UPlayerActionsInterface>())
		Side2Entered(OtherActor);
}

void ABaseDoors::Side1Entered_Implementation(AActor* Actor) {}
void ABaseDoors::Side2Entered_Implementation(AActor* Actor) {}