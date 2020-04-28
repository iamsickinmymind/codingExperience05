// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "codingExperience5GameMode.h"
#include "codingExperience5Pawn.h"
#include "codingExperience5Hud.h"

AcodingExperience5GameMode::AcodingExperience5GameMode()
{
	DefaultPawnClass = AcodingExperience5Pawn::StaticClass();
	HUDClass = AcodingExperience5Hud::StaticClass();
}
