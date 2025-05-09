// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Selectable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USelectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GPP_RESEARCH_API ISelectable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual void Select() = 0;
	
	UFUNCTION()
	virtual void DeSelect() = 0;
	
	UFUNCTION()
	virtual void Highlight(const bool Highlight) = 0;

	UFUNCTION()
	virtual void ToggleHighlight(const bool Select) = 0;


};
