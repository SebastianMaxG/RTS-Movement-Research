#pragma once

//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
#include <Blueprint/BlueprintSupport.h>
#include "AIDataTypes.generated.h"

// for adding different types of movement
UENUM(BlueprintType)
enum class ECommandType
{
	Move,
	MoveFast,
	MoveSlow
};

UENUM(BlueprintType)
enum class EFormationType
{
	Current,
	V,
	Spear,
	O,
	line,
	pole
};

USTRUCT(BlueprintType)
struct FCommandData
{
	GENERATED_BODY()

	FCommandData() : Pos(FVector::ZeroVector), Rot(FRotator::ZeroRotator), CameraYaw(0.f), Formation(EFormationType::Current), Type(ECommandType::Move) {}
	FCommandData(const FVector toPos, const FRotator toRot, const float cameraRot, const EFormationType formation, const ECommandType type) 
		: Pos(toPos), Rot(toRot), CameraYaw(cameraRot), Formation(formation), Type(type) {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Pos;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rot;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CameraYaw;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EFormationType Formation;

	//Extra
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECommandType Type;
};
