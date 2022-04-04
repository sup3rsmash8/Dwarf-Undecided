// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.
// AUTHOR: Justus Hörberg


#include "UI/Subtitles/CustomerPersonalDialogAsset.h"

void UCustomerPersonalDialogAsset::GetDialog(UDialogAsset* Asset, TArray<FDialogBox>& DialogArray)
{
	if (Asset)
	{
		TArray<FDialogBox> DialogBoxes;

		DialogBoxes += Asset->DialogBoxes;

		DialogArray = DialogBoxes;
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Assigning dialog to DialogArray from dialog asset failed because Asset was nullptr!"));
}

void UCustomerPersonalDialogAsset::GetDialog_FirstTimeTalk(TArray<FDialogBox>& DialogArray)
{
	GetDialog(FirstTimeDialog, DialogArray);
}

void UCustomerPersonalDialogAsset::GetDialog_SecondTimeTalk(TArray<FDialogBox>& DialogArray)
{
	GetDialog(SecondTimeDialog, DialogArray);
}

void UCustomerPersonalDialogAsset::GetDialog_GoodDeal(TArray<FDialogBox>& DialogArray)
{
	GetDialog(GoodDealDialog, DialogArray);
}

void UCustomerPersonalDialogAsset::GetDialog_OKDeal(TArray<FDialogBox>& DialogArray)
{
	GetDialog(OKDealDialog, DialogArray);
}

void UCustomerPersonalDialogAsset::GetDialog_BadDeal(TArray<FDialogBox>& DialogArray)
{
	GetDialog(BadDealDialog, DialogArray);
}

FString UCustomerPersonalDialogAsset::GetCustomerRandomName() const
{
	FString RandStr;
	if (CustomerPossibleNames.Num() > 0)
	{
		const int Random = FMath::RandRange(0, CustomerPossibleNames.Num() - 1);
		RandStr = CustomerPossibleNames[Random];
	}
	else
	{
		RandStr = TEXT("Error");
	}

	return RandStr;
}
