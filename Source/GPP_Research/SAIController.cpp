// Fill out your copyright notice in the Description page of Project Settings.


#include "SAIController.h"
#include "GPP_ResearchCharacter.h"

void ASAIController::CommandMove(const FCommandData CommandData)
{
	m_CurrentCommandData = CommandData;

	MoveToLocation(CommandData.Pos);
}

void ASAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	m_OwningCharacter = Cast<AGPP_ResearchCharacter>(InPawn);
	if (m_OwningCharacter)
	{
		m_OwningCharacter->SetAIController(this);
	}
}

void ASAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	OnReachedDestination.Broadcast(m_CurrentCommandData);
}
