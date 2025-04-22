// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectionBox.generated.h"

class UBoxComponent;
class UDecalComponent;
class ASPlayerController;

UCLASS()
class GPP_RESEARCH_API ASelectionBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASelectionBox();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Start(FVector Pos, const FRotator Rot);

	UFUNCTION()
	void End();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void Adjust() const;

	UFUNCTION()
	void Manage();

	UFUNCTION()
	void HandleHighLight(AActor* ActorInBox, bool select) const;

	UFUNCTION()
	void OnBoxColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY()
	FVector StartPos;

	UPROPERTY()
	FRotator StartRot;

	UPROPERTY()
	TArray<AActor*> InBox;

	UPROPERTY()
	TArray<AActor*> CenterInBox;

private:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxCollider;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* DecalComponent;

	UPROPERTY()
	bool m_BoxSelect;

	UPROPERTY()
	ASPlayerController* SPlayer;
};
