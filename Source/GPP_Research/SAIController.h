// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIDataTypes.h"
#include "AIController.h"
#include "SAIController.generated.h"

class AGPP_ResearchCharacter;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReachedDestinationDelegate, const FCommandData, CommandData);

/**
 * 
 */
UCLASS()
class GPP_RESEARCH_API ASAIController : public AAIController
{
	GENERATED_BODY()

public:

	void CommandMove(const FCommandData CommandData);


	UPROPERTY()
	FReachedDestinationDelegate OnReachedDestination;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID,const FPathFollowingResult& Result) override;

	UPROPERTY()
	AGPP_ResearchCharacter* m_OwningCharacter;

	UPROPERTY()
	FCommandData m_CurrentCommandData;


	
};
