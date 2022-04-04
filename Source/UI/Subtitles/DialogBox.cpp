// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.


#include "UI/Subtitles/DialogBox.h"
#include "UI/Subtitles/DialogAsset.h"

//TMap<EDialogErrorReason, FText> FDialogBox::DialogErrorMessages = TMap<EDialogErrorReason, FText>/*const TMap<EDialogErrorReason, FText>()*/
//{
//	{ EDialogErrorReason::IndexOutOfRange, "Babe" },
//};

const FDialogBox FDialogBox::GetErrorDialog(const EDialogErrorReason Reason)
{
	FString ErrorStr = TEXT("ERROR: ");

	switch (Reason)
	{
	default:
		ErrorStr += TEXT("No reason :)");
		break;

	case EDialogErrorReason::IndexOutOfRange:
		ErrorStr += TEXT("Dialog box index was out of range.");
		break;

	case EDialogErrorReason::DialogAssetNull:
		ErrorStr += TEXT("Dialog asset was nullptr.");
		break;

	case EDialogErrorReason::DialogAssetEmpty:
		ErrorStr += TEXT("Dialog asset did not contain any dialog box entries.");
		break;
	}

	FText ErrorReasonText = FText::FromString(ErrorStr);

	return FDialogBox(ErrorReasonText, true);
}

//TArray<FDialogBox> FDialogBox::GenerateMessageFromMultipleAssets(TArray<UDialogAsset*> DialogAssets)
//{
//	TArray<FDialogBox> Result;
//	for (const UDialogAsset* Asset : DialogAssets)
//	{
//		if (Asset)
//		{
//			for (FDialogBox DialogBox : Asset->DialogBoxes)
//			{
//				Result.Add(DialogBox);
//			}
//		}
//	}
//
//	// Throw an error if none
//	if (Result.Num() <= 0)
//		Result.Add(FDialogBox::GetErrorDialog(EDialogErrorReason::DialogAssetEmpty));
//
//	return Result;
//}