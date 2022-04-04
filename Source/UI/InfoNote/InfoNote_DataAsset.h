// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.
// AUTHOR: Justus Hörberg

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/InfoNote/InfoNoteData.h"
#include "InfoNote_DataAsset.generated.h"

/**
 * 
 */
UCLASS()
class COOLGP3PROJECT_API UInfoNote_DataAsset : public UDataAsset
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	TArray<FInfoNoteData> InfoNotes;

public:
	UFUNCTION(BlueprintCallable)
	/* Pastes the contents of this data asset into the target reference. */
	void GetInfoNotes(TArray<FInfoNoteData>& TargetArray);
};
