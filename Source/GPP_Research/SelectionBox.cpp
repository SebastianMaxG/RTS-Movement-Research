// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectionBox.h"
#include <Components/BoxComponent.h>
#include <Components/DecalComponent.h>
#include "SPlayerController.h"
#include <Kismet/GameplayStatics.h>
#include "Selectable.h"
#include <Kismet/KismetMathLibrary.h>


// Sets default values
ASelectionBox::ASelectionBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetBoxExtent(FVector(1.f, 1.f, 1.f));
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoxCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = BoxCollider;

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ASelectionBox::OnBoxColliderBeginOverlap);

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetupAttachment(RootComponent);

	m_BoxSelect = false;
}

// Called when the game starts or when spawned
void ASelectionBox::BeginPlay()
{
	Super::BeginPlay();

	SetActorEnableCollision(false);
	if (DecalComponent)
	{
		DecalComponent->SetVisibility(false);
	}
	
	SPlayer = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

}

// Called every frame
void ASelectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (m_BoxSelect)
	{
		Adjust();
		Manage();
	}
}

void ASelectionBox::Start(FVector Pos,const FRotator Rot)
{
	if (!DecalComponent) return;

	UE_LOG(LogTemp, Warning, TEXT("StartBoxDrag"));
	if (Pos == FVector::ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("Zero"));


	StartPos = FVector(Pos.X, Pos.Y, 0.f);
	StartRot = FRotator(0, Rot.Yaw, 0.f);


	InBox.Empty();
	CenterInBox.Empty();

	SetActorLocation(StartPos);
	SetActorRotation(StartRot);
	SetActorEnableCollision(true);

	DecalComponent->SetVisibility(true);
	m_BoxSelect = true;
}

void ASelectionBox::End()
{
	if (!SPlayer) return;

	m_BoxSelect = false;
	SetActorEnableCollision(false);
	DecalComponent->SetVisibility(false);

	if (CenterInBox.IsEmpty())
	{
		SPlayer->Handle_Selection(nullptr);
	}
	else
	{
		SPlayer->Handle_Selection(CenterInBox);
	}

	DecalComponent->DecalSize = FVector::ZeroVector;
	InBox.Empty();
	CenterInBox.Empty();
}

void ASelectionBox::Adjust() const
{
	if (!SPlayer || !BoxCollider || !DecalComponent) return;

	const FVector CurrentMouseLocOnTerrain = SPlayer->GetMousePosOnTerain();
	const FVector EndPoint = FVector(CurrentMouseLocOnTerrain.X, CurrentMouseLocOnTerrain.Y, 0.f);

	BoxCollider->SetWorldLocation(UKismetMathLibrary::VLerp(StartPos, EndPoint, 0.5f));

	FVector newExtent = FVector(GetActorLocation().X, GetActorLocation().Y, 0.f) - EndPoint;
	newExtent = GetActorRotation().GetInverse().RotateVector(newExtent);
	newExtent = newExtent.GetAbs();
	newExtent.Z += 100000.f;
	BoxCollider->SetBoxExtent(newExtent);
	
	//rotate for floor instead of wall;
	FVector DecalSize;
	DecalSize.X = newExtent.Z;
	DecalSize.Y = newExtent.Y;
	DecalSize.Z = newExtent.X;
	DecalComponent->DecalSize = DecalSize;
}

void ASelectionBox::Manage()
{
	if (!BoxCollider) return;

	for (size_t i = 0; i < InBox.Num(); i++)
	{
		FVector ActorCenter = InBox[i]->GetActorLocation();
		const FVector localActorCenter = BoxCollider->GetComponentTransform().InverseTransformPosition(ActorCenter);

		const FVector localExtends = BoxCollider->GetUnscaledBoxExtent();
		if (localActorCenter.X >= -localExtends.X && localActorCenter.X <= localExtends.X &&
			localActorCenter.Y >= -localExtends.Y && localActorCenter.Y <= localExtends.Y &&
			localActorCenter.Z >= -localExtends.Z && localActorCenter.Z <= localExtends.Z)
		{
			if (!CenterInBox.Contains(InBox[i]))
			{
			CenterInBox.Add(InBox[i]);
			HandleHighLight(InBox[i], true);

			}
		}
		else
		{
			CenterInBox.Remove(InBox[i]);
			HandleHighLight(InBox[i], false);
		}
	}
}

void ASelectionBox::HandleHighLight(AActor* ActorInBox, bool select) const
{
	if (ISelectable* selectable = Cast<ISelectable>(ActorInBox))
	{
		selectable->ToggleHighlight(select);
	}
}

void ASelectionBox::OnBoxColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (ISelectable* selectable = Cast<ISelectable>(OtherActor))
	{
		InBox.AddUnique(OtherActor);
	}
}


