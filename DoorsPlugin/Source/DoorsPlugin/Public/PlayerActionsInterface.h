// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerActionsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerActionsInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DOORSPLUGIN_API IPlayerActionsInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
	FTransform GetTransformToTeleportTo();

	UFUNCTION(BlueprintNativeEvent)
	void PlayerActorTeleported();
};
