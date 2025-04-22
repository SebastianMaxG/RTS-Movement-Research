// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

struct FCommandData;
/**
 * 
 */
UCLASS()
class GPP_RESEARCH_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	//ASPlayerController(const FObjectInitializer ObjectInitializer = FObjectInitializer::Get());
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void Handle_Selection(AActor* ActorToSelect);
	void Handle_Selection(TArray<AActor*>& ActorsToSelect);

	UFUNCTION()
	FVector GetMousePosOnTerain() const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	bool ActorSelected(AActor* Actor);

	UFUNCTION()
	void Select(AActor* Actor);

	UFUNCTION()
	void DeSelect(AActor* Actor);

	UFUNCTION()
	void ClearSelected();

	UPROPERTY()
	TArray<AActor*> Selected;

	//Formations
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Formations")
	float VerticalSpacing{100.f};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Formations")
	float HorizontalSpacing{100.f};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Formations")
	float CircleSpacing{150.f};


	//Command
public: 
	UFUNCTION()
	void CommandSelected(FCommandData CommandData);
};
