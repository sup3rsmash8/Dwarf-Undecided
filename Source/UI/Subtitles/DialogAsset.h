// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.

// AUTHOR: Justus Hörberg

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/Subtitles/DialogBox.h"
#include "DialogAsset.generated.h"

//FDialogBox::DialogBoxError = const FDialogBox(FText::FromString(TEXT("ERROR LOL THIS DIDNT WORK OUT BUT YOU THOUGHT IT DID :)")), true);

/**
 * 
 */
UCLASS(Blueprintable)
class COOLGP3PROJECT_API UDialogAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FDialogBox> DialogBoxes;

	/* Returns a dialog box with a message at a given index. */
	FDialogBox GetDialogBox(const int Index) const
	{
		if (Index < DialogBoxes.Num())
		{
			return DialogBoxes[Index];
		}

		return FDialogBox();
	}
};
