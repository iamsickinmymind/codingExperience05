// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "codingExperience5Hud.generated.h"


UCLASS(config = Game)
class AcodingExperience5Hud : public AHUD
{
	GENERATED_BODY()

public:
	AcodingExperience5Hud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface
};
