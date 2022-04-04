// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogBox.generated.h"

/* Enum containing error message types for the way dialog boxes can screw up. */
UENUM()
enum class EDialogErrorReason
{
	IndexOutOfRange = 0,
	DialogAssetNull = 1,
	DialogAssetEmpty = 2
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDialogBox
{
	GENERATED_BODY()

		/* Creation with empty constructor implies something went horribly wrongggg */
	FDialogBox()
	{
		const FText Txt = FText::FromString(TEXT("ERROR LOL THIS DIDNT WORK OUT BUT YOU THOUGHT IT DID :)"));
		Message = Txt;
		bSkippable = true;
	}

	FDialogBox(FText Text, bool bCanSkip = true)
	{
		Message = Text;
		bSkippable = bCanSkip;
	}

	/* The message that will show up in the prompt. */
	UPROPERTY(EditAnywhere, meta = (MultiLine = "true"))
		FText Message;

	/* If true, the player can speed up the text by holding a button. */
	UPROPERTY(EditAnywhere)
		bool bSkippable = true;

public:
	/* Returns a dialog box containing an error message. Use this for cases where a dialog box failed to load, for example if a DialogAsset returns nullptr or something. */
	static const FDialogBox GetErrorDialog(const EDialogErrorReason Reason);

	/* Generates a dialog box consisting of dialog from multiple assets. All text is added in the order the dialog assets were added in the array. */
	//static TArray<FDialogBox> GenerateMessageFromMultipleAssets(TArray<UDialogAsset*> DialogAssets);
};