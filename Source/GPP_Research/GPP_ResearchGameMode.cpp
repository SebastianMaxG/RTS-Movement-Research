// Copyright Epic Games, Inc. All Rights Reserved.

#include "GPP_ResearchGameMode.h"
#include "GPP_ResearchPlayerController.h"
#include "GPP_ResearchCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGPP_ResearchGameMode::AGPP_ResearchGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AGPP_ResearchPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}