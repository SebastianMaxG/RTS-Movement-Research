// Copyright Epic Games, Inc. All Rights Reserved.

#include "GPP_ResearchCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "AIDataTypes.h"
#include <Kismet/KismetMathLibrary.h>
#include "SAIController.h"


AGPP_ResearchCharacter::AGPP_ResearchCharacter()
{
	
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	m_CharacterMoveComp = GetCharacterMovement();
	if (m_CharacterMoveComp)
	{
		m_CharacterMoveComp->bOrientRotationToMovement = true; // Rotate character to moving direction
		m_CharacterMoveComp->RotationRate = FRotator(0.f, 640.f, 0.f);
		m_CharacterMoveComp->bConstrainToPlane = true;
		m_CharacterMoveComp->bSnapToPlaneAtStart = true;
		m_MaxSpeed = m_CharacterMoveComp->MaxWalkSpeed;
	}

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// auto set AI
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

void AGPP_ResearchCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (m_ShouldOrientate)
	{
		SetOrientation(DeltaSeconds);

		if (IsOrientated())
		{
			m_ShouldOrientate = 0;
		}
	}
}

void AGPP_ResearchCharacter::Select()
{
	UE_LOG(LogTemp, Warning, TEXT("Actor Select"));

	m_IsSelected = true;
	Highlight(true);
}

void AGPP_ResearchCharacter::DeSelect()
{
	UE_LOG(LogTemp, Warning, TEXT("Actor DeSelect"));

	m_IsSelected = false;
	Highlight(false);

}

void AGPP_ResearchCharacter::Highlight(const bool Highlight)
{
	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);
	for (UPrimitiveComponent* VisualComp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		{
			Prim->SetRenderCustomDepth(Highlight);
		}
	}
}

void AGPP_ResearchCharacter::ToggleHighlight(const bool Select)
{
	if (Select)
	{
		Highlight(!m_IsSelected);
	}
	else
	{
		Highlight(m_IsSelected);
	}
}

void AGPP_ResearchCharacter::CommandMoveToPos(const FCommandData CommandData)
{
	switch (CommandData.Type)
	{
	default:
	case ECommandType::Move:
		SetRun();
		break;
	case ECommandType::MoveFast:
		SetSprint();
		break;
	case ECommandType::MoveSlow:
		SetWalk();
		break;

	}

	CommandMove(CommandData);
}

void AGPP_ResearchCharacter::CommandMove(const FCommandData CommandData)
{
	if (!m_SAIController) return;
	m_SAIController->OnReachedDestination.Clear();
	m_SAIController->OnReachedDestination.AddDynamic(this, &AGPP_ResearchCharacter::DestinationReached);
	m_SAIController->CommandMove(CommandData);

	SetMoveMarker(CommandData.Pos);
}

void AGPP_ResearchCharacter::DestinationReached(const FCommandData CommandData)
{
	m_TargetOrentation = CommandData.Rot;
	m_ShouldOrientate = 1;

	if (m_MoveMarker)
	m_MoveMarker->Destroy();
}

void AGPP_ResearchCharacter::SetRun()
{
	if (m_CharacterMoveComp)
	{
		m_CharacterMoveComp->MaxWalkSpeed = m_MaxSpeed;
	}
}

void AGPP_ResearchCharacter::SetSprint()
{
	if (m_CharacterMoveComp)
	{
		m_CharacterMoveComp->MaxWalkSpeed = m_MaxSpeed * 1.25f;

	}
}

void AGPP_ResearchCharacter::SetWalk()
{
	if (m_CharacterMoveComp)
	{
		m_CharacterMoveComp->MaxWalkSpeed = m_MaxSpeed * 0.5f;

	}
}

void AGPP_ResearchCharacter::SetOrientation(float DeltaSeconds)
{
	FRotator interpolatedRot = UKismetMathLibrary::RInterpTo(
		FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw, 0.f),
		m_TargetOrentation, DeltaSeconds, 2.f);
	SetActorRotation(interpolatedRot);
}

bool AGPP_ResearchCharacter::IsOrientated()
{
	const FRotator currentRotation = GetActorRotation();

	if (FMath::IsNearlyEqual(currentRotation.Yaw, m_TargetOrentation.Yaw, 0.25f))
	{
		return true;
	}

	return false;
}

void AGPP_ResearchCharacter::SetMoveMarker(const FVector Pos)
{
	if (MoveMarkerClass)
	{
		if (m_MoveMarker)
		{
			m_MoveMarker->Destroy();
		}

		FActorSpawnParameters param;
		param.Instigator = this;
		param.Owner = this;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (UWorld* world = GetWorld())
		{
			m_MoveMarker  = world->SpawnActor<AActor>(MoveMarkerClass, GetPositionTransform(Pos), param);
		}
	}
}

FTransform AGPP_ResearchCharacter::GetPositionTransform(const FVector Position) const
{
	FHitResult hit{};
	FCollisionQueryParams CollisionParam{};
	FVector TraceStart = Position;
	TraceStart.Z += 10000.f;
	FVector TraceEnd = Position;
	TraceEnd.Z -= 10000.f;

	if (UWorld* world = GetWorld())
	{
		if (world->LineTraceSingleByChannel(hit, TraceStart, TraceEnd, ECC_GameTraceChannel1, CollisionParam))
		{
			if (hit.bBlockingHit)
			{
				FTransform hitTransform;
				hitTransform.SetLocation(hit.ImpactPoint + FVector(1.f,1.f,1.25f));
				FRotator terrainRotation = UKismetMathLibrary::MakeRotFromZX(hit.Normal, FVector::UpVector);
				terrainRotation += FRotator(90.f, 0.f, 0.f);
				hitTransform.SetRotation(terrainRotation.Quaternion());


				return hitTransform;
			}
			UE_LOG(LogTemp, Warning, TEXT("Doesn't hit terrain"));


		}
	}
	return FTransform::Identity;
}

