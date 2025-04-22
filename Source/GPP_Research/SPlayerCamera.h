// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIDataTypes.h"
#include "GameFramework/Pawn.h"
#include "SPlayerCamera.generated.h"

class ASPlayerController;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class ASelectionBox;
struct FInputActionValue;

UCLASS()
class GPP_RESEARCH_API ASPlayerCamera : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASPlayerCamera();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//CameraMovement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TSoftObjectPtr <UInputMappingContext> InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* inputForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* inputRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* inputZoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* inputZoomOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* inputRotateRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* inputRotateLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* inputRotate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* inputRotateHorizontal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* inputRotateVertical;


	UFUNCTION()
	void Forward(float AxisValue);
	UFUNCTION()
	void iForward(const FInputActionValue& Value);

	UFUNCTION()
	void Right(float AxisValue);
	UFUNCTION()
	void iRight(const FInputActionValue& Value);

	UFUNCTION()
	void Zoom(float AxisValue);
	UFUNCTION()
	void iZoom(const FInputActionValue& Value);
	UFUNCTION()
	void iZoomOut(const FInputActionValue& Value);

	UFUNCTION()
	void RotateRight();
	UFUNCTION()
	void iRotateRight(const FInputActionValue& Value);

	UFUNCTION()
	void RotateLeft();
	UFUNCTION()
	void iRotateLeft(const FInputActionValue& Value);

	UFUNCTION()
	void iEnableRotate(const FInputActionValue& Value);

	UFUNCTION()
	void iDisableRotate(const FInputActionValue& Value);

	UFUNCTION()
	void EnableRotate();
	
	UFUNCTION()
	void DisableRotate();

	UFUNCTION()
	void RotateHorizontal(float AxisValue);
	UFUNCTION()
	void iRotateHorizontal(const FInputActionValue& Value);
	
	UFUNCTION()
	void RotateVertical(float AxisValue);
	UFUNCTION()
	void iRotateVertical(const FInputActionValue& Value);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Settings|Camera")
	float m_MoveSpeed = 20.f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Settings|Camera")
	float m_RotateSpeed = 2.f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Settings|Camera")
	float m_PitchMin = 10.f;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Settings|Camera")
	float m_PitchMax = 80.f;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Settings|Camera")
	float m_ZoomSpeed = 2.f;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Settings|Camera")
	float m_ZoomMin = 500.f;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Settings|Camera")
	float m_ZoomMax = 4000.f;


	//Unit select + Move

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* inputMouseLeft;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* inputMouseLeftHold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* inputMouseRight;

	UFUNCTION()
	AActor* GetSelectedObject();

	UFUNCTION()
	void iMouseLeftPressed(const FInputActionValue& Value);

	UFUNCTION()
	void MouseLeftPressed();

	UFUNCTION() 
	void iLeftMouseInputHeld(const FInputActionInstance& Value);

	UFUNCTION() 
	void LeftMouseInputHeld(float AxisValue);

	UFUNCTION()
	void iMouseLeftReleased(const FInputActionValue& Value);

	UFUNCTION()
	void MouseLeftReleased();


	UFUNCTION()
	void iMouseRightPressed(const FInputActionValue& Value);

	UFUNCTION()
	void MouseRightPressed();

	UFUNCTION()
	void iMouseRightReleased(const FInputActionValue& Value);

	UFUNCTION()
	void MouseRightReleased();

	UFUNCTION()
	void CreateSelectionBox();

	UPROPERTY()
	ASPlayerController* SPlayer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Mouse")
	float m_DragSelectTime{0.5f};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Mouse")
	TSubclassOf<ASelectionBox> SelectionBoxClass;

	UPROPERTY()
	ASelectionBox* m_SelectionBox;

	UPROPERTY()
	FVector m_LeftMouseHitPos;

	UPROPERTY()
	bool m_BoxSelect;

	UPROPERTY()
	float m_CurrentHoldTime;

	//Commands for Movement
	UFUNCTION()
	void CommandStart(const FInputActionValue& Value);
	UFUNCTION()
	void Command(const FInputActionValue& Value);
	UFUNCTION()
	void ShiftCommand(const FInputActionValue& Value);
	UFUNCTION()
	void CtrlCommand(const FInputActionValue& Value);
	//Optional extra types (Shift click etc)
	UFUNCTION()
	FCommandData CreateCommandData(const ECommandType Type) const;

	UPROPERTY()
	FVector m_CommandPos;

private:

	UFUNCTION()
	void CameraBounds();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	UPROPERTY()
	FVector m_TargetPos;

	UPROPERTY()
	FRotator m_TargetRotation;

	UPROPERTY()
	float m_TargetZoom;

	UPROPERTY()
	bool m_CanRotate;

	UPROPERTY()
	bool m_RightMousePressed{false};


	//Formations

	protected:


	UPROPERTY()
	EFormationType m_Formationtype{EFormationType::Spear};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Formations")
	UInputAction* inputO;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Formations")
	UInputAction* inputLine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Formations")
	UInputAction* inputPike;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Formations")
	UInputAction* inputCurrent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Formations")
	UInputAction* inputV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Formations")
	UInputAction* inputSpear;

	UFUNCTION()
	void iO_Formation(const FInputActionValue& Value);

	UFUNCTION()
	void iLine_Formation(const FInputActionValue& Value);

	UFUNCTION()
	void iPike_Formation(const FInputActionValue& Value);
	
	UFUNCTION()
	void iCurrent_Formation(const FInputActionValue& Value);

	UFUNCTION()
	void iV_Formation(const FInputActionValue& Value);

	UFUNCTION()
	void iSpear_Formation(const FInputActionValue& Value);
};
