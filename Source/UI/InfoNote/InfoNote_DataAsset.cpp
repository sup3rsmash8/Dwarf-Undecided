// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.
// AUTHOR: Justus Hörberg

#include "UI/InfoNote/InfoNote_DataAsset.h"

void UInfoNote_DataAsset::GetInfoNotes(TArray<FInfoNoteData>& TargetArray)
{
	TargetArray = InfoNotes;
}
