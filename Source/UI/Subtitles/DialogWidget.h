// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.

// AUTHOR: Justus Hörberg

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Subtitles/DialogAsset.h"
#include "Components/Image.h"
#include "Components/Overlay.h"

#include "DialogWidget.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDialogBoxEvent, int, Decision);

UENUM()
enum class EDialogBoxState
{
	Inactive = 0,
	Opening,
	Typing,
	WaitingForInput,
	Closing,
};

/**
 * Widget that handles dialog by displaying prompts, etc.
 */
UCLASS()
class COOLGP3PROJECT_API UDialogWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void InitializeInputComponent() override;

	/* Initiates a textbox that takes in dialog from an array. Arguments can be added and delegates assigned that trigger when the textbox closes. */
	void InitiateTextbox(TArray<FDialogBox> DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FStringFormatArg> Args);

	/* Initiates a textbox that takes in dialog from an array. Arguments can be added, and delegates assigned that trigger when the textbox closes.
	 * This blueprint-friendly version might require string conversions in order for non-string arguments to be used. */
	UFUNCTION(BlueprintCallable)
	void InitiateTextbox_BP(TArray<FDialogBox> DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FString> Args);

	/* Initiates a textbox that takes in dialog from an asset. Arguments can be added and delegates assigned that trigger when the textbox closes. */
	void InitiateTextbox(UDialogAsset* DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FStringFormatArg> Args);

	/* Initiates a textbox that takes in dialog from an asset. Arguments can be added, and delegates assigned that trigger when the textbox closes.
	 * This blueprint-friendly version might require string conversions in order for non-string arguments to be used. */
	UFUNCTION(BlueprintCallable)
	void InitiateTextbox_BP_Asset(UDialogAsset* DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FString> Args);
	
	//UFUNCTION(BlueprintCallable)
	//void InitiateTextbox(TSubclassOf<UDialogAsset> DialogToDisplay, TArray<FString> Args);

	/* Returns whether or not the widget is currently on the final dialog box in the asset it is reading from. If the asset is null, always returns true. */
	UFUNCTION(BlueprintCallable)
	bool IsOnFinalDialogBox();

	/* In which state the textbox is currently in - is it opening, typing or closing, etc.?
	* WARNING: Do NOT write to this directly! To change a state, please use ChangeDialogBoxState() instead! */
	EDialogBoxState CurrentDialogBoxState;

protected:
	/* Whether or not this widget is currently active. */
	bool bIsActive = false;
	
	

	EDialogBoxState GetCurrentDialogBoxState() const { return CurrentDialogBoxState; }

	void ChangeDialogBoxState(EDialogBoxState NewState);
	
	/* If we can currently skip the text. */
	bool bIsSkippable;

	/* The text that is supposed to be printed out. */
	FString MessageString;
	
	/* The text as it is currently printed. */
	FString CurrentText;

	/* The dialog boxes that the textbox can currently display. */
	TArray<FDialogBox> LoadedDialogBoxes;

	/* String arguments to display, provided when the textbox was initiated. */
	TArray<FStringFormatArg> Arguments;

	/* When it hits 0 while typing out a message, adds the next character to the text widget. */
	float MessageTimer = 0;

	/* Whether or not the message is typing fast. */
	bool bIsSkipping;
	
	/* The current dialog box to read from the UDialogAsset. */
	int MessageIndex = 0;

	/* The current character position in the currently loaded text that the textbox is reading from. */
	int MessageCursorPosition = 0;

	/* The length of the currently loaded text. */
	int MessageFullLength = 0;

	
	FInputActionBinding ConfirmBinding;

	// Delegates
public:
	UPROPERTY()
	FOnDialogBoxEvent OnDialogBoxFinished;
	// End of Delegates
protected:
	UPROPERTY(Transient)
	UUMGSequencePlayer* TempSeqPlayer;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* TextBoxAppear;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* TextBoxWait_Loop;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* TextBoxDisappear;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* TextBoxInactive_Loop;
	// End of Transients

	// Widgets
	//UPROPERTY(meta = (BindWidget))
	//class UCanvasPanel* WidgetCanvas;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* WidgetOverlay;

	UPROPERTY(meta = (BindWidget))
	class UImage* TextboxBackdrop;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DialogTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NameTextBlock;
	// End of Widgets

	// Various Inspector Values
	
	/* The standard speed of the text. */
	UPROPERTY(EditAnywhere)
	float TextSpeed = 0.015f;
	
	/* The speed of the text when skipping. */
	UPROPERTY(EditAnywhere)
	float TextSpeedFast = 0.0075f;
	
	// End of Various Inspector Values

	/* Loads text from the currently loaded asset. Index corresponds to the dialog box to read from. */
	FDialogBox LoadAndInitializeText(const int Index) const;

	void TextboxActivate();
	void TextboxDeactivate();

	void StateEnter_Opening();
	void Tick_Opening(const FGeometry& MyGeometry, float InDeltaTime);
	
	void StateEnter_Typing();
	void Tick_Typing(const FGeometry& MyGeometry, float InDeltaTime);
	
	void StateEnter_WaitingForInput();
	void Tick_WaitingForInput(const FGeometry& MyGeometry, float InDeltaTime);
	
	void StateEnter_Closing();
	void Tick_Closing(const FGeometry& MyGeometry, float InDeltaTime);

	/* Handles events that occur when a prompt is confirmed in WaitingForInput. */
	void PromptExecute();

	FText CustomerName;

public:
	UFUNCTION()
	/* Input function called when pressing the interact button. Speeds up the text during typing, or goes to the next dialog box during waiting. */
	void OnPromptPressConfirm();

	/* Whether or not this widget is currently active. */
	bool IsCurrentlyActive() const { return bIsActive; }
};
