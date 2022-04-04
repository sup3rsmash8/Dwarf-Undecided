// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.
// AUTHOR: Justus Hörberg

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/Subtitles/DialogAsset.h"
#include "CustomerPersonalDialogAsset.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class COOLGP3PROJECT_API UCustomerPersonalDialogAsset : public UDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	/* What possible names this customer can randomize to have. If you want the customer to have only one name, keep only one in the array. */
	TArray<FString> CustomerPossibleNames;

	UPROPERTY(EditAnywhere)
	/* What this customer will say the first time talking to them. */
	UDialogAsset* FirstTimeDialog;
	
	UPROPERTY(EditAnywhere)
	/* What this customer will say the second time (or more) they visit the shop Leaving this empty makes them repeat FirstTimeDialog. */
	UDialogAsset* SecondTimeDialog;

	UPROPERTY(EditAnywhere)
	/* What this customer will say when a good deal was reached. */
	UDialogAsset* GoodDealDialog;

	UPROPERTY(EditAnywhere)
	/* What this customer will say when an OK deal was reached. */
	UDialogAsset* OKDealDialog;

	UPROPERTY(EditAnywhere)
	/* What this customer will say when a bad deal was reached. */
	UDialogAsset* BadDealDialog;

	// Dialog getter base
	void GetDialog(UDialogAsset* Asset, TArray<FDialogBox>& DialogArray);

public:
	UFUNCTION(BlueprintCallable)
	/* Populates the array with dialog from FirstTimeDialog. */
	void GetDialog_FirstTimeTalk(TArray<FDialogBox>& DialogArray);

	UFUNCTION(BlueprintCallable)
	/* Populates the array with dialog from SecondTimeDialog. */
	void GetDialog_SecondTimeTalk(TArray<FDialogBox>& DialogArray);

	UFUNCTION(BlueprintCallable)
	/* Populates the array with dialog from GoodDealDialog. */
	void GetDialog_GoodDeal(TArray<FDialogBox>& DialogArray);

	UFUNCTION(BlueprintCallable)
	/* Populates the array with dialog from OKDealDialog. */
	void GetDialog_OKDeal(TArray<FDialogBox>& DialogArray);

	UFUNCTION(BlueprintCallable)
	/* Populates the array with dialog from BadDealDialog. */
	void GetDialog_BadDeal(TArray<FDialogBox>& DialogArray);

	UFUNCTION(BlueprintCallable)
	/* Populates the array with dialog from BadDealDialog. */
	FString GetCustomerRandomName() const;
};
