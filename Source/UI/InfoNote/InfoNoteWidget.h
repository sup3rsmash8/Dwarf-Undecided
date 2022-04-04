// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"

#include "UI/InfoNote/InfoNoteData.h"
#include "InfoNoteWidget.generated.h"

UENUM()
enum class ENoteState : uint8
{
	Uninitialized = 0, // For PrevNoteState, so that OnStateChanged gets called during first Tick
	Inactive,
	Appearing,
	TransitionIn,
	Waiting,
	TransitionOut,
	Disappearing
};

// @TODO: Merge NewspaperWidget, NewsArticleWidget and InfoNoteWidget into one base class. They are too similar to each other to be separate classes.

DECLARE_DYNAMIC_DELEGATE(FOnNoteEvent);

/**
 * 
 */
UCLASS()
class COOLGP3PROJECT_API UInfoNoteWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void BeginDestroy() override;

	/* Fills contents of the widget with the data from the note and displays it. Returns whether or not this was successful (usually fails if the widget is already in the process of activating another article). */
	UFUNCTION(BlueprintCallable)
	bool DisplayNote(TArray<FInfoNoteData> NoteInfo, const FOnNoteEvent& OnNoteOpenedDelegate, const FOnNoteEvent& OnNoteClosedDelegate);

	/* Hides the note. Returns whether or not this was successful (usually fails if the widget is not waiting for input). */
	UFUNCTION(BlueprintCallable)
	bool UndisplayNote();

protected:
	FInputActionBinding ConfirmBinding;

	/* The index of the note currently being read. */
	int NoteIndex = 0;

	/* The articles that will be shown. */
	UPROPERTY(Transient)
	TArray<FInfoNoteData> InfoNotes;

	/* Delegate that triggers when the newspaper is opened. */
	FOnNoteEvent OnNoteOpened;
	/* Delegate that triggers when the newspaper is closed. */
	FOnNoteEvent OnNoteClosed;

	/* The note that the text is written on. */
	UPROPERTY(meta = (BindWidget))
	class UImage* NoteTexture;

	/* The text written on the note. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NoteText;

	/* The animation that plays when the note appears. */
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* NoteAppear;

	/* The animation that plays when the text written on the note appears. */
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* NoteTransitionIn;

	/* The animation that plays when the note is waiting for input. */
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* NoteWait_Loop;

	/* The animation that plays when the text written on the note disappears. */
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* NoteTransitionOut;

	/* The animation that plays when the note appears. */
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* NoteDisappear;

	/* Animation that mostly serves to hide the widget when inactive. */
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* NoteInvisibleInactive_Loop;

	ENoteState NoteState = ENoteState::Inactive;
	ENoteState PrevNoteState = ENoteState::Uninitialized;

public:

	UFUNCTION(BlueprintCallable)
	/* Returns whether or not the article widget is
	currently active (displaying an article). */
	ENoteState GetArticleState() const { return NoteState; }

	/* Returns whether or not the widget is playing a transition animation. */
	bool IsInTransition() const;

	/* Returns whether or not the widget is currently in its inactive state. */
	bool IsInactive() const;

protected:
	/* Method for when the widget activates (Initialization, etc.). */
	virtual void Activate();
	/* Method for when the widget deactivates. */
	virtual void Deactivate();

	/* Method for application of the note text to be displayed. */
	void SetNoteText(FInfoNoteData NoteData);

	/* Checks if the active widget state has changed since the last frame and executes code if so. */
	void OnStateChanged(const ENoteState NewState);

	void Tick_Appearing(const FGeometry& MyGeometry, float InDeltaTime);
	void Tick_TransitionIn(const FGeometry& MyGeometry, float InDeltaTime);
	void Tick_Waiting(const FGeometry& MyGeometry, float InDeltaTime);
	void Tick_TransitionOut(const FGeometry& MyGeometry, float InDeltaTime);
	void Tick_Disappearing(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION()
	void OnPromptPressConfirm();
};
