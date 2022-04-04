// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.
// AUTHOR: Justus Hörberg

#pragma once

#include "CoreMinimal.h"
#include "InfoNoteData.generated.h"

/**
 * Struct containing text that can be written onto InfoNoteWidgets.
 */
USTRUCT(BlueprintType)
struct COOLGP3PROJECT_API FInfoNoteData
{
	GENERATED_BODY()

	FInfoNoteData();
	FInfoNoteData(FText Text);
	UPROPERTY(EditAnywhere, meta = (MultiLine = "true"))
	FText NoteText;
};
