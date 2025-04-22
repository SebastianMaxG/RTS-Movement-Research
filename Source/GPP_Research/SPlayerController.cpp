// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "Selectable.h"
#include "AIDataTypes.h"
#include "GPP_ResearchCharacter.h"

//ASPlayerController::ASPlayerController(const FObjectInitializer ObjectInitializer)
//{
//	
//}
//
//void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//
//}

void ASPlayerController::Handle_Selection(AActor* ActorToSelect)
{
	if (!ActorToSelect) return;
	if (ISelectable* selectable = Cast<ISelectable>(ActorToSelect))
	{
		if (ActorSelected(ActorToSelect))
		{
			DeSelect(ActorToSelect);

		}
		else
		{
			Select(ActorToSelect);

		}
	}
	else
	{
		ClearSelected();
	}
}

void ASPlayerController::Handle_Selection(TArray<AActor*>& ActorsToSelect)
{
	for (size_t i = 0; i < ActorsToSelect.Num(); i++)
	{
		Handle_Selection(ActorsToSelect[i]);
	}
}

FVector ASPlayerController::GetMousePosOnTerain() const
{
	FVector WorldPos, WorldDir;
	DeprojectMousePositionToWorld(WorldPos, WorldDir);

	FHitResult hit;
	if (GetWorld()->LineTraceSingleByChannel(hit, WorldPos, WorldPos + (WorldDir * 1000000.f), ECC_GameTraceChannel1))
	{
		if (hit.bBlockingHit)
		{
			return hit.Location;
		}
	}
	return FVector::ZeroVector;
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI inputMode;
	inputMode.SetHideCursorDuringCapture(false);
	SetInputMode(inputMode);
	bShowMouseCursor = true;
}

bool ASPlayerController::ActorSelected(AActor* Actor)
{
	if (Actor && Selected.Contains(Actor))
	{
		return true;
	}

	return false;
}

void ASPlayerController::Select(AActor* Actor)
{
	if (Actor)
	{
		if (ISelectable* selectable = Cast<ISelectable>(Actor))
		{
			selectable->Select();
			Selected.Add(Actor);
		}
	}
}

void ASPlayerController::DeSelect(AActor* Actor)
{
	if (Actor)
	{
		if (ISelectable* selectable = Cast<ISelectable>(Actor))
		{
			selectable->DeSelect();
			Selected.Remove(Actor);
		}
	}
}

void ASPlayerController::ClearSelected()
{
	for (size_t i = 0; i < Selected.Num(); i++)
	{
		if (Selected[i])
		{
			if (ISelectable* selectable = Cast<ISelectable>(Selected[i]))
			{
				selectable->DeSelect();
			}
		}
	}
	Selected.Empty();
}

void ASPlayerController::CommandSelected(FCommandData CommandData)
{
	if (Selected.Num() <= 0) return;
	if (Selected.Num() == 1)
	{
		if (AGPP_ResearchCharacter* selectedActor = Cast<AGPP_ResearchCharacter>(Selected[0]))
		{
			selectedActor->CommandMoveToPos(CommandData);
		}
		return;
	}

	switch (CommandData.Formation)
	{
	default:
	case EFormationType::Current:
	{

		FVector AveragePos{};


		for (int i{ 0 }; i < Selected.Num(); ++i)
		{
			AveragePos += Selected[i]->GetActorLocation();
		}
		AveragePos /= Selected.Num();
		for (int i{ 0 }; i < Selected.Num(); ++i)
		{

			if (AGPP_ResearchCharacter* selectedActor = Cast<AGPP_ResearchCharacter>(Selected[i]))
			{
				FVector targetPos = (Selected[i]->GetActorLocation() - AveragePos);
				targetPos = (CommandData.Rot - FRotator(0.f, CommandData.CameraYaw, 0.f)).RotateVector(targetPos);

				FCommandData individualCommand{ CommandData };
				individualCommand.Pos += targetPos;
				selectedActor->CommandMoveToPos(individualCommand);
			}
		}

		break;
	}
	case EFormationType::V:
	{
		bool isUnEven{ (bool)(Selected.Num() % 2) };
		FVector targetPos{};
		if (isUnEven)
		{
			if (AGPP_ResearchCharacter* selectedActor = Cast<AGPP_ResearchCharacter>(Selected[0]))
			{
				selectedActor->CommandMoveToPos(CommandData);
				targetPos.X += VerticalSpacing;
				targetPos.Y += HorizontalSpacing;
			}
		}
		else
		{
			targetPos.Y += HorizontalSpacing / 2.f;
		}
		for (int i{ isUnEven }; i < Selected.Num(); ++i)
		{
			if (AGPP_ResearchCharacter* selectedActor = Cast<AGPP_ResearchCharacter>(Selected[i]))
			{
				bool thisUnEven{ (bool)(i % 2) };
				FVector trueTargetPos{ targetPos.X , targetPos.Y * (thisUnEven * 2 - 1), 0.f };
				trueTargetPos = (CommandData.Rot - FRotator(0.f, CommandData.CameraYaw, 0.f)).RotateVector(trueTargetPos);
				FCommandData individualCommand{ CommandData };
				individualCommand.Pos += trueTargetPos;
				selectedActor->CommandMoveToPos(individualCommand);

				targetPos.X += VerticalSpacing * (thisUnEven ^ isUnEven);
				targetPos.Y += HorizontalSpacing * (thisUnEven ^ isUnEven);
			}
		}
		break;
	}
	case EFormationType::Spear:
	{
		bool isUnEven{ (bool)(Selected.Num() % 2) };
		FVector targetPos{};
		if (isUnEven)
		{
			if (AGPP_ResearchCharacter* selectedActor = Cast<AGPP_ResearchCharacter>(Selected[0]))
			{
				selectedActor->CommandMoveToPos(CommandData);
				targetPos.X -= VerticalSpacing;
				targetPos.Y += HorizontalSpacing;
			}
		}
		else
		{
			targetPos.Y += HorizontalSpacing / 2.f;
		}
		for (int i{ isUnEven }; i < Selected.Num(); ++i)
		{
			if (AGPP_ResearchCharacter* selectedActor = Cast<AGPP_ResearchCharacter>(Selected[i]))
			{
				bool thisUnEven{ (bool)(i % 2) };
				FVector trueTargetPos{ targetPos.X , targetPos.Y * (thisUnEven * 2 - 1), 0.f };
				trueTargetPos = (CommandData.Rot - FRotator(0.f, CommandData.CameraYaw, 0.f)).RotateVector(trueTargetPos);
				FCommandData individualCommand{ CommandData };
				individualCommand.Pos += trueTargetPos;
				selectedActor->CommandMoveToPos(individualCommand);

				targetPos.X -= VerticalSpacing * (thisUnEven ^ isUnEven);
				targetPos.Y += HorizontalSpacing * (thisUnEven ^ isUnEven);
			}
		}
		break;
	}
	case EFormationType::O:
	{
		const float angleDif{ (2 * PI / Selected.Num()) };
		//CircleSpacing = angle(rad) * radius ==> 
		const float radius = CircleSpacing / angleDif;
		float currentAngle{};

		for (int i{ 0 }; i < Selected.Num(); ++i)
		{

			if (AGPP_ResearchCharacter* selectedActor = Cast<AGPP_ResearchCharacter>(Selected[i]))
			{
				FVector targetPos{ radius * sin(currentAngle), radius * cos(currentAngle), 0.f };
				targetPos = (CommandData.Rot - FRotator(0.f, CommandData.CameraYaw, 0.f)).RotateVector(targetPos);

				FCommandData individualCommand{ CommandData };
				individualCommand.Pos += targetPos;
				selectedActor->CommandMoveToPos(individualCommand);

				currentAngle += angleDif;
			}
		}
		
		break;
	}
	case EFormationType::pole:
	{
		float offset{ -VerticalSpacing * Selected.Num() / 2.f };
		for (int i{ 0 }; i < Selected.Num(); ++i)
		{
			if (AGPP_ResearchCharacter* selectedActor = Cast<AGPP_ResearchCharacter>(Selected[i]))
			{
				FVector targetPos{ offset, 0.f, 0.f };
				targetPos = (CommandData.Rot - FRotator(0.f, CommandData.CameraYaw, 0.f)).RotateVector(targetPos);
				FCommandData individualCommand{ CommandData };
				individualCommand.Pos += targetPos;
				selectedActor->CommandMoveToPos(individualCommand);

				offset += VerticalSpacing;
			}
		}
		break;
	}
	case EFormationType::line:
	{
		float offset{ -HorizontalSpacing * Selected.Num() / 2.f };
		for (int i{ 0 }; i < Selected.Num(); ++i)
		{
			if (AGPP_ResearchCharacter* selectedActor = Cast<AGPP_ResearchCharacter>(Selected[i]))
			{
				FVector targetPos{0.f, offset, 0.f };
				targetPos = (CommandData.Rot - FRotator(0.f, CommandData.CameraYaw, 0.f)).RotateVector(targetPos);
				FCommandData individualCommand{ CommandData };
				individualCommand.Pos += targetPos;
				selectedActor->CommandMoveToPos(individualCommand);

				offset += HorizontalSpacing;
			}
		}
		break;
	}
	}

	
}
