// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerCamera.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <EnhancedPlayerInput.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include <InputMappingContext.h>
#include "SPlayerController.h"
#include <Kismet/GameplayStatics.h>
#include "SelectionBox.h"

// Sets default values
ASPlayerCamera::ASPlayerCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 2000.f;
	SpringArmComponent->SetRelativeRotation(FRotator(-50.f, 0.f, 0.f));
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(this))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (!InputMapping.IsNull())
			{
				InputSystem->AddMappingContext(InputMapping.LoadSynchronous(), 0);
				
			}
		}
	}

}

// Called when the game starts or when spawned
void ASPlayerCamera::BeginPlay()
{
	Super::BeginPlay();
	//Set Start pos and start zoom
	m_TargetPos = GetActorLocation();
	m_TargetZoom = 2000.f;

	//Set initial rotation
	const FRotator rotation = SpringArmComponent->GetRelativeRotation();
	//m_TargetRotation = FRotator(rotation.Pitch + -50.f, rotation.Yaw, 0.f);
	m_TargetRotation = SpringArmComponent->GetRelativeRotation();

	SPlayer = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
	CreateSelectionBox();
}


//--------------------------------------------------------------------------------------------------------------------
// Camera Controll
//--------------------------------------------------------------------------------------------------------------------

void ASPlayerCamera::Forward(float AxisValue)
{
	if (AxisValue == 0) return;
	m_TargetPos += SpringArmComponent->GetForwardVector() * AxisValue * m_MoveSpeed;
}

void ASPlayerCamera::iForward(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Actor Forward"));

	Forward(Value.Get<float>());
}

void ASPlayerCamera::Right(float AxisValue)
{
	if (AxisValue == 0) return;
	m_TargetPos += SpringArmComponent->GetRightVector() * AxisValue * m_MoveSpeed;
}

void ASPlayerCamera::iRight(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Actor Right"));

	Right(Value.Get<float>());
}

void ASPlayerCamera::Zoom(float AxisValue)
{
	if (AxisValue == 0) return;
	constexpr float speed{ 100.f };
	const float zoom{ AxisValue * speed + m_TargetZoom};

	if (zoom > m_ZoomMax)
	{
		m_TargetZoom = m_ZoomMax;
	}
	else if (zoom < m_ZoomMin)
	{
		m_TargetZoom = m_ZoomMin;
	}
	else
		m_TargetZoom = zoom;
}

void ASPlayerCamera::iZoom(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Actor ZoomIn"));

	Zoom(Value.Get<float>());
}

void ASPlayerCamera::iZoomOut(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Actor ZoomOut"));

	Zoom(-float(Value.Get<bool>()));
}

void ASPlayerCamera::RotateRight()
{
	m_TargetRotation = UKismetMathLibrary::ComposeRotators(m_TargetRotation, FRotator(0.f, -30.f, 0.f));
	UE_LOG(LogTemp, Warning, TEXT("Actor Rot Right Called"));

}

void ASPlayerCamera::iRotateRight(const FInputActionValue& Value)
{

	UE_LOG(LogTemp, Warning, TEXT("Actor Rot Right"));

	if (Value.Get<bool>())
	{
		RotateRight();
	}
}

void ASPlayerCamera::RotateLeft()
{
	m_TargetRotation = UKismetMathLibrary::ComposeRotators(m_TargetRotation, FRotator(0.f, 30.f, 0.f));
}

void ASPlayerCamera::iRotateLeft(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		RotateLeft();
	}
}

void ASPlayerCamera::iEnableRotate(const FInputActionValue& Value)
{
	EnableRotate();
}

void ASPlayerCamera::iDisableRotate(const FInputActionValue& Value)
{
	DisableRotate();
}


void ASPlayerCamera::EnableRotate()
{
	m_CanRotate = true;
}

void ASPlayerCamera::DisableRotate()
{
	m_CanRotate = false;
}

void ASPlayerCamera::RotateHorizontal(float AxisValue)
{
	if (AxisValue == 0 || !m_CanRotate) return;
	m_TargetRotation = UKismetMathLibrary::ComposeRotators(m_TargetRotation, FRotator(0.f, AxisValue, 0.f));

}

void ASPlayerCamera::iRotateHorizontal(const FInputActionValue& Value)
{
	RotateHorizontal(Value.Get<float>());
}

void ASPlayerCamera::RotateVertical(float AxisValue)
{
	if (AxisValue == 0 || !m_CanRotate) return;
	m_TargetRotation = UKismetMathLibrary::ComposeRotators(m_TargetRotation, FRotator(AxisValue, 0.f, 0.f));

}

void ASPlayerCamera::iRotateVertical(const FInputActionValue& Value)
{
	RotateVertical(Value.Get<float>());
}


void ASPlayerCamera::CameraBounds()
{
	float newPitch = m_TargetRotation.Pitch;
	if (m_TargetRotation.Pitch < -(m_PitchMax))
	{
		newPitch = -(m_PitchMax);
	}
	else if (m_TargetRotation.Pitch > -(m_PitchMin))
	{
		newPitch = -(m_PitchMin);
	}


	m_TargetRotation = FRotator(newPitch, m_TargetRotation.Yaw, 0.f);

	m_TargetPos.Z = 0.f;
}



//--------------------------------------------------------------------------------------------------------------------
// Selection
//--------------------------------------------------------------------------------------------------------------------

AActor* ASPlayerCamera::GetSelectedObject()
{
	if (UWorld* World = GetWorld())
	{
		FVector WorldPos, WorldDir;
		SPlayer->DeprojectMousePositionToWorld(WorldPos, WorldDir);
		FVector End = WorldDir * 1000000.0f + WorldPos;

		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		FHitResult hit;
		if (World->LineTraceSingleByChannel(hit, WorldPos, End, ECC_Visibility, params))
		{
			if (AActor* HitActor = hit.GetActor())
			{
				return HitActor;
			}
		}
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------
// Mouse Input
//--------------------------------------------------------------------------------------------------------------------


void ASPlayerCamera::iMouseLeftPressed(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Left Mouse Press I"));
	if (!m_CurrentHoldTime)
	{
		m_CurrentHoldTime += 0.00001f;
		MouseLeftPressed();
	}
}


void ASPlayerCamera::MouseLeftPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Left Mouse Press"));
	if (!SPlayer) return;

	SPlayer->Handle_Selection(nullptr);
	m_BoxSelect = false;
	m_LeftMouseHitPos = SPlayer->GetMousePosOnTerain();
}

void ASPlayerCamera::iLeftMouseInputHeld(const FInputActionInstance& Value)
{

	m_CurrentHoldTime += Value.GetElapsedTime();
	if (m_CurrentHoldTime >= m_DragSelectTime)
	{
		LeftMouseInputHeld(1);

	}
	//UE_LOG(LogTemp, Warning, TEXT("Left Mouse Held IIIIIIIII"));

}

void ASPlayerCamera::LeftMouseInputHeld(float AxisValue)
{

	if (!SPlayer || AxisValue == 0) return;

	if (!m_BoxSelect && m_SelectionBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("Left Mouse Held"));

		m_SelectionBox->Start(m_LeftMouseHitPos, m_TargetRotation);
		m_BoxSelect = true;
	}
}

void ASPlayerCamera::iMouseLeftReleased(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Left Mouse Released I"));
	m_CurrentHoldTime = 0.f;
	MouseLeftReleased();
}

void ASPlayerCamera::MouseLeftReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("Left Mouse Released"));

	if (SPlayer)
	{
		if (m_BoxSelect && m_SelectionBox)
		{
			m_SelectionBox->End();
			m_BoxSelect = false;
		}
		else
		{
			SPlayer->Handle_Selection(GetSelectedObject());
		}
	}
}


void ASPlayerCamera::iMouseRightPressed(const FInputActionValue& Value)
{
	if (m_RightMousePressed) return;
	UE_LOG(LogTemp, Warning, TEXT("Right Mouse Press"));
	m_RightMousePressed = true;
	CommandStart(Value);

}

void ASPlayerCamera::MouseRightPressed()
{
}

void ASPlayerCamera::iMouseRightReleased(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Right Mouse Release"));
	m_RightMousePressed = false;

	Command(Value);
}

void ASPlayerCamera::MouseRightReleased()
{
}

void ASPlayerCamera::CreateSelectionBox()
{
	if (!SelectionBoxClass) return;

	if (UWorld* WorldContext = GetWorld())
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Instigator = this;
		spawnParams.Owner = this;
		m_SelectionBox = WorldContext->SpawnActor<ASelectionBox>(SelectionBoxClass, FVector::ZeroVector, FRotator::ZeroRotator, spawnParams);
		if (m_SelectionBox) m_SelectionBox->SetOwner(this);
	}
	
}




//--------------------------------------------------------------------------------------------------------------------
// Commands
//--------------------------------------------------------------------------------------------------------------------

void ASPlayerCamera::CommandStart(const FInputActionValue& Value)
{
	//useful if using multiple different commands
	if (!SPlayer) return;
	UE_LOG(LogTemp, Warning, TEXT("Command Start"));

	m_CommandPos = SPlayer->GetMousePosOnTerain();
}

void ASPlayerCamera::Command(const FInputActionValue& Value)
{
	if (!SPlayer) return;
	UE_LOG(LogTemp, Warning, TEXT("Command"));

	SPlayer->CommandSelected(CreateCommandData(ECommandType::Move));
}

void ASPlayerCamera::ShiftCommand(const FInputActionValue& Value)
{
	if (!SPlayer) return;

	SPlayer->CommandSelected(CreateCommandData(ECommandType::MoveFast));
}

void ASPlayerCamera::CtrlCommand(const FInputActionValue& Value)
{
	if (!SPlayer) return;

	SPlayer->CommandSelected(CreateCommandData(ECommandType::MoveSlow));
}

FCommandData ASPlayerCamera::CreateCommandData(const ECommandType Type) const
{
	if (!SPlayer) return FCommandData();

	FRotator commandRot{ FRotator::ZeroRotator};
	float commandCamYaw{ 0.f};

	const FVector commandEndPos = SPlayer->GetMousePosOnTerain();
	const FVector direction = commandEndPos - m_CommandPos;

	if (direction.Length() > 100.f)
	{
		const float rotationAngle = FMath::RadiansToDegrees(FMath::Atan2(direction.Y, direction.X));
		
		commandRot = FRotator(0.f, rotationAngle, 0.f);
		commandCamYaw = m_TargetRotation.Yaw;
	}
	if (m_Formationtype != EFormationType::Current)
	{
		commandCamYaw -= m_TargetRotation.Yaw;
	}

	return FCommandData(m_CommandPos, commandRot, commandCamYaw, m_Formationtype, Type);
}





//--------------------------------------------------------------------------------------------------------------------
// Engine Functions
//--------------------------------------------------------------------------------------------------------------------

// Called every frame
void ASPlayerCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraBounds();

	const FVector InterpolatedPos = UKismetMathLibrary::VInterpTo(GetActorLocation(), m_TargetPos, DeltaTime, m_MoveSpeed);
	SetActorLocation(InterpolatedPos);

	const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(SpringArmComponent->GetRelativeRotation(), m_TargetRotation, DeltaTime, m_MoveSpeed);
	SpringArmComponent->SetRelativeRotation(InterpolatedRotation);
	
	const float InterpolatedZoom = UKismetMathLibrary::FInterpTo(SpringArmComponent->TargetArmLength, m_TargetZoom, DeltaTime, m_ZoomSpeed);
	SpringArmComponent->TargetArmLength = InterpolatedZoom;

}

// Called to bind functionality to input
void ASPlayerCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//EnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	//EIC->BindAction(inputForward, ETriggerEvent::Triggered, this, &ASPlayerCamera::Forward);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping.Get(), 0);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(inputForward, ETriggerEvent::Triggered, this,				&ASPlayerCamera::iForward);
	Input->BindAction(inputRight, ETriggerEvent::Triggered, this,				&ASPlayerCamera::iRight);
	Input->BindAction(inputZoom, ETriggerEvent::Triggered, this,				&ASPlayerCamera::iZoom);
	Input->BindAction(inputRotateHorizontal, ETriggerEvent::Triggered, this,	&ASPlayerCamera::iRotateHorizontal);
	Input->BindAction(inputRotateVertical, ETriggerEvent::Triggered, this,		&ASPlayerCamera::iRotateVertical);
																								 
	Input->BindAction(inputRotateRight, ETriggerEvent::Triggered, this,			&ASPlayerCamera::iRotateRight);
	Input->BindAction(inputRotateLeft, ETriggerEvent::Triggered, this,			&ASPlayerCamera::iRotateLeft);
	Input->BindAction(inputRotate, ETriggerEvent::Started, this,				&ASPlayerCamera::iEnableRotate);
	Input->BindAction(inputRotate, ETriggerEvent::Completed, this,				&ASPlayerCamera::iDisableRotate);

	Input->BindAction(inputMouseLeft, ETriggerEvent::Triggered, this,				&ASPlayerCamera::iMouseLeftPressed);
	Input->BindAction(inputMouseLeftHold, ETriggerEvent::Ongoing, this,			&ASPlayerCamera::iLeftMouseInputHeld);
	Input->BindAction(inputMouseLeft, ETriggerEvent::Completed, this,			&ASPlayerCamera::iMouseLeftReleased);

	Input->BindAction(inputMouseRight, ETriggerEvent::Triggered, this,			&ASPlayerCamera::iMouseRightPressed);
	Input->BindAction(inputMouseRight, ETriggerEvent::Completed, this,			&ASPlayerCamera::iMouseRightReleased);

	//PlayerInputComponent->BindAxis(TEXT("Forward"), this, &ASPlayerCamera::Forward);
	//PlayerInputComponent->BindAxis(TEXT("Right"), this, &ASPlayerCamera::Right);
	//PlayerInputComponent->BindAxis(TEXT("Zoom"), this, &ASPlayerCamera::Zoom);
	//PlayerInputComponent->BindAxis(TEXT("RotateHorizontal"), this, &ASPlayerCamera::RotateHorizontal);
	//PlayerInputComponent->BindAxis(TEXT("RotateVertical"), this, &ASPlayerCamera::RotateVertical);

	//PlayerInputComponent->BindAction(TEXT("RotateRight"), IE_Pressed, this, &ASPlayerCamera::RotateRight);
	//PlayerInputComponent->BindAction(TEXT("RotateLeft"), IE_Pressed, this, &ASPlayerCamera::RotateLeft);
	//PlayerInputComponent->BindAction(TEXT("Rotate"), IE_Pressed, this, &ASPlayerCamera::EnableRotate);
	//PlayerInputComponent->BindAction(TEXT("Rotate"), IE_Released, this, &ASPlayerCamera::DisableRotate);

	//formations
	Input->BindAction(inputO,		ETriggerEvent::Triggered, this, &ASPlayerCamera::iO_Formation);
	Input->BindAction(inputLine,	ETriggerEvent::Triggered, this, &ASPlayerCamera::iLine_Formation);
	Input->BindAction(inputPike,	ETriggerEvent::Triggered, this, &ASPlayerCamera::iPike_Formation);
	Input->BindAction(inputCurrent, ETriggerEvent::Triggered, this, &ASPlayerCamera::iCurrent_Formation);
	Input->BindAction(inputV,		ETriggerEvent::Triggered, this, &ASPlayerCamera::iV_Formation);
	Input->BindAction(inputSpear,	ETriggerEvent::Triggered, this, &ASPlayerCamera::iSpear_Formation);
}


//--------------------------------------------------------------------------------------------------------------------
// Formations Input
//--------------------------------------------------------------------------------------------------------------------


void ASPlayerCamera::iO_Formation(const FInputActionValue& Value)
{
	m_Formationtype = EFormationType::O;
}

void ASPlayerCamera::iLine_Formation(const FInputActionValue& Value)
{
	m_Formationtype = EFormationType::line;
}

void ASPlayerCamera::iPike_Formation(const FInputActionValue& Value)
{
	m_Formationtype = EFormationType::pole;
}

void ASPlayerCamera::iCurrent_Formation(const FInputActionValue& Value)
{
	m_Formationtype = EFormationType::Current;
}

void ASPlayerCamera::iV_Formation(const FInputActionValue& Value)
{
	m_Formationtype = EFormationType::V;
}

void ASPlayerCamera::iSpear_Formation(const FInputActionValue& Value)
{
	m_Formationtype = EFormationType::Spear;
}