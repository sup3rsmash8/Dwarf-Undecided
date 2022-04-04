// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.

// Author: Justus Hörberg

#pragma once

#include "CoreMinimal.h"
#include "UI/Subtitles/DialogWidget.h"
#include "UI/Newspaper/NewspaperWidget.h"
#include "UI/Newspaper/NewsArticle_DataAsset.h"
#include "UI/InfoNote/InfoNoteWidget.h"
#include "UI/InfoNote/InfoNote_DataAsset.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

class UItemDisplayWidget;
class UGoldCarryWidget;
//class UNewspaperWidget;
//class FArticleData;
//class UDialogWidget;
//class UDialogAsset;

/**
 * 
 */
UCLASS()
class COOLGP3PROJECT_API AGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	AGameHUD();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginDestroy() override;
	UFUNCTION(BlueprintCallable)
	UNewspaperWidget* GetNewsPaper() const;
protected:
	/* Should the newspaper widget be removed from the viewport when it goes inactive? */
	UPROPERTY(EditDefaultsOnly)
	bool bRemoveNewspaperWidgetOnInactive = true;

	/* The item display widget that will be drawn on the HUD. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UItemDisplayWidget> ItemDisplayWidget;

	/* The gold widget that will be drawn on the HUD. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGoldCarryWidget> GoldCarryWidget;

	/* The gold widget that will be drawn on the HUD. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UDialogWidget> DialogWidget;

	/* The newspaper widget that will be drawn on the HUD. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UNewspaperWidget> NewspaperWidget;

	/* The info note widget that will be drawn on the HUD. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UInfoNoteWidget> InfoNoteWidget;

	/* The data asset the task note will display. */
	UPROPERTY(EditAnywhere)
	UInfoNote_DataAsset* TaskNoteData;

	/* DEBUG PROPERTY: The test dialog asset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UDialogAsset*> DebugDialogAsset;

	// Instances
	// @TODO: Change the raw widget pointers into WeakPtrs.
	
	UPROPERTY(BlueprintReadOnly)
	UItemDisplayWidget* ItemDisplayWidget_Inst;

	UPROPERTY(BlueprintReadOnly)
	UGoldCarryWidget* GoldCarryWidget_Inst;

	UPROPERTY(BlueprintReadOnly)
	UDialogWidget* DialogWidget_Inst;

	UPROPERTY(BlueprintReadOnly)
	UNewspaperWidget* NewspaperWidget_Inst;

	UPROPERTY(BlueprintReadOnly)
	UInfoNoteWidget* InfoNoteWidget_Inst;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	//	UGoldWidgetComponent* GoldWidget;

public:
	/* The item display that shows up when hovering over an item. */
	UItemDisplayWidget* GetItemDisplayWidget() const;

	/* The widget that displays the gold. */
	UGoldCarryWidget* GetGoldCarryWidget() const;

	/* The widget that handles dialog and text. */
	UDialogWidget* GetDialogWidget() const;

	UNewspaperWidget* GetNewspaperWidget() const;

	UInfoNoteWidget* GetInfoNoteWidget() const;

	/* Initiates a textbox that takes in dialog from an asset. Arguments can be added and delegates assigned that trigger when the textbox closes. */
	void InitiateTextbox(UDialogAsset* DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FStringFormatArg> Args);

	/* Initiates a textbox that takes in dialog from an array. Arguments can be added and delegates assigned that trigger when the textbox closes. */
	void InitiateTextbox(TArray<FDialogBox> DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FStringFormatArg> Args);

	/* Initiates a textbox that takes in dialog from an asset. Arguments can be added, and delegates assigned that trigger when the textbox closes.
	 * This blueprint-friendly version might require string conversions in order for non-string arguments to be used. */
	UFUNCTION(BlueprintCallable)
	void InitiateTextbox_Asset(UDialogAsset* DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FString> Args);

	/* Initiates a textbox that takes in dialog from an array. Arguments can be added, and delegates assigned that trigger when the textbox closes.
	* This blueprint-friendly version might require string conversions in order for non-string arguments to be used. */
	UFUNCTION(BlueprintCallable)
	void InitiateTextbox_Array(TArray<FDialogBox> DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FString> Args);

	UFUNCTION(BlueprintCallable)
	bool DisplayNewspaper(TArray<FArticleData> Articles, const FOnNewspaperEvent& OnNewspaperOpenDelegate, const FOnNewspaperEvent& OnNewspaperClosedDelegate, const FOnNewspaperEvent& OnArticleSpawn);

	/* Displays mom's note. */
	UFUNCTION(BlueprintCallable)
	bool DisplayNote(TArray<FInfoNoteData> NoteInfo, const FOnNoteEvent& OnNoteOpenedDelegate, const FOnNoteEvent& OnNoteClosedDelegate);

	/* Returns whether or not a textbox is already active. */
	bool IsTextboxAlreadyActive() const;

protected:
	UFUNCTION()
	virtual void RemoveNewspaperOnInactive();

	UFUNCTION()
	/* Displays the task list note. Is supposed to be bound to the Q-button. */
	void OnTaskListDisplay();

	UFUNCTION()
	/* Unrestricts player movement. Purely for OnTaskListDisplay(). */
	void UnrestrictPlayerMovement();
};
