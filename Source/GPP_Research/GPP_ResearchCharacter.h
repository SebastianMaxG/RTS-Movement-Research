// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Selectable.h"
#include "GameFramework/Character.h"
#include "GPP_ResearchCharacter.generated.h"

class ASAIController;
struct FCommandData;


UCLASS(Blueprintable)
class AGPP_ResearchCharacter : public ACharacter, public ISelectable
{
	GENERATED_BODY()

public:
	AGPP_ResearchCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	bool m_IsSelected;
public:
	virtual void Select() override;
	virtual void DeSelect() override;
	virtual void Highlight(const bool Highlight) override;
	virtual void ToggleHighlight(const bool Select) override;

	UFUNCTION()
	void CommandMoveToPos(const FCommandData CommandData);

	UFUNCTION()
	void SetAIController(ASAIController* NewAIController) 
	{ m_SAIController = NewAIController; };

protected:

	UFUNCTION()
	void CommandMove(const FCommandData CommandData);

	UFUNCTION()
	void DestinationReached(const FCommandData CommandData);

	UFUNCTION()
	void SetRun();

	UFUNCTION()
	void SetSprint();

	UFUNCTION()
	void SetWalk();
	
	UFUNCTION()
	void SetOrientation(float DeltaSeconds);

	UFUNCTION()
	bool IsOrientated();

	UPROPERTY()
	UCharacterMovementComponent* m_CharacterMoveComp;

	UPROPERTY()
	float m_MaxSpeed;

	UPROPERTY()
	FRotator m_TargetOrentation;

	UPROPERTY()
	bool m_ShouldOrientate;

	UPROPERTY()
	ASAIController* m_SAIController;

	//Move Marker

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI Settings")
	TSubclassOf<AActor> MoveMarkerClass;

	UPROPERTY()
	AActor* m_MoveMarker;

	UFUNCTION()
	void SetMoveMarker(const FVector Pos);

	UFUNCTION()
	FTransform GetPositionTransform(const FVector Position) const;
};

