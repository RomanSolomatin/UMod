// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "UModHUD.generated.h"

UCLASS()
class AUModHUD : public AHUD
{
	GENERATED_BODY()

public:
	AUModHUD(const FObjectInitializer& ObjectInitializer);

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

