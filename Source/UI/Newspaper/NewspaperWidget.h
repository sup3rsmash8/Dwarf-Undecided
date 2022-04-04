// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.

// AUTHOR: Justus Hörberg

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Image.h"
#include "Components/Overlay.h"
#include "UI/Newspaper/NewsArticleWidget.h"
#include "UI/Newspaper/NewsArticle_DataAsset.h"
#include "NewspaperWidget.generated.h"


//struct FArticleData;

UENUM()
enum class ENewspaperState : uint8
{
	Uninitialized = 0, // For PrevNewspaperState, so that OnStateChanged gets called during first Tick
	Inactive = 0,
	Appearing,
	OnScreen_TransitionIn,
	OnScreen,
	OnScreen_TransitionOut,
	Disappearing
};

DECLARE_DYNAMIC_DELEGATE(FOnNewspaperEvent);

// @TODO: Merge NewspaperWidget, NewsArticleWidget and InfoNoteWidget into one base class. They are too similar to each other to be separate classes.

/**
 * The widget that displays all the stories at the end of a day.
 */
UCLASS()
class COOLGP3PROJECT_API UNewspaperWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void InitializeInputComponent() override;

	/* Delegate that triggers when the newspaper is opened. */
	FOnNewspaperEvent OnNewspaperOpened;
	/* Delegate that triggers when the newspaper is closed. */
	FOnNewspaperEvent OnNewspaperClosed;
	/* Delegate that triggers when the newspaper is done loading an article. */
	FOnNewspaperEvent OnArticleSpawn;

	/* Displays the newspaper with the headlines provided in Articles. Returns whether or not the newspaper was able to be generated and displayed (usually because a newspaper is already on display). */
	UFUNCTION(BlueprintCallable)
	bool GenerateAndDisplayNewspaper(TArray<FArticleData> Articles, const FOnNewspaperEvent& OnNewspaperOpenDelegate, const FOnNewspaperEvent& OnNewspaperClosedDelegate, const FOnNewspaperEvent& OnArticleSpawnDelegate);
	
	/* Returns whether or not the widget is inactive. */
	UFUNCTION(BlueprintCallable)
	bool IsInactive() const { return NewspaperState == ENewspaperState::Inactive; }

	//NewspaperWidget_Inst->RemoveFromViewport();

	/* Hides the newspaper. Returns whether the newspaper could be hidden or not. */
	//UFUNCTION(BlueprintCallable)
	//bool HideNewspaper();

	UFUNCTION(BlueprintCallable)
	void OnPromptPressConfirm();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void DeactivateWidget();

protected:
	FInputActionBinding ConfirmBinding;
	float Check = 0.0f;
	/* Should the widget be removed from viewport as soon as it becomes inactive? */
	UPROPERTY(EditDefaultsOnly)
		bool bRemoveSelfOnInactive = false;


	/* The overlay containing everything related to the page. */
	//UPROPERTY(meta = (BindWidget))
	//class UOverlay* PageOverlay;

	/* The image representing the newspaper page. */
	UPROPERTY(meta = (BindWidget))
	class UImage* PageTexture;

	/* Input that registers that the input prompt has been confirmed. */
	//bool bConfirmInputBuffer = false;

	/* The index of the article currently being read. */
	int ArticleIndex = 0;

	/* The articles that will be shown. */
	UPROPERTY(Transient)
	TArray<FArticleData> NewsArticles;

	/* The widget representing the articles. */
	UPROPERTY(meta = (BindWidget))
	class UNewsArticleWidget* NewsArticleWidget;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* NewspaperInactiveInvisible_Loop;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* NewspaperAppear;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* NewspaperWait_Loop;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* NewspaperDisappear;

	/* The state of the newspaper widget. */
	ENewspaperState NewspaperState = ENewspaperState::Inactive;

	/* (Do NOT modify this manually!!!) The previous state of the newspaper widget. */
	ENewspaperState PrevNewspaperState = ENewspaperState::Uninitialized;

	/* Checks if the active widget state has changed since the last frame and executes code if so. */
	void OnStateChanged(const ENewspaperState NewState);

	void Tick_Appearing(const FGeometry& MyGeometry, float InDeltaTime);
	void Tick_OnScreen_TransitionIn(const FGeometry& MyGeometry, float InDeltaTime);
	void Tick_OnScreen(const FGeometry& MyGeometry, float InDeltaTime);
	void Tick_OnScreen_TransitionOut(const FGeometry& MyGeometry, float InDeltaTime);
	void Tick_Disappearing(const FGeometry& MyGeometry, float InDeltaTime);

};
