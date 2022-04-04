// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.

// AUTHOR: Justus Hörberg

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "UI/Newspaper/NewsArticle_DataAsset.h"
#include "NewsArticleWidget.generated.h"

UENUM()
enum class EArticleState : uint8
{
	Uninitialized = 0, // For PrevArticleState, so that OnStateChanged gets called during first Tick
	Inactive,
	Appearing,
	Waiting,
	Disappearing
};

// @TODO: Merge NewspaperWidget, NewsArticleWidget and InfoNoteWidget into one base class. They are too similar to each other to be separate classes.

/**
 * 
 */
UCLASS()
class COOLGP3PROJECT_API UNewsArticleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void InitializeArticle();

	/* Article headline. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Headline;

	/* The dwarf quote text. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CommentText;

	/* The story text. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StoryText;

	/* The photo, if one should be there. */
	UPROPERTY(meta = (BindWidget))
	class UImage* Photo;

	/* The animation that plays when the article appears on the paper. */
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ArticleTransitionIn;

	/* The animation that plays when the article is waiting for input. */
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ArticleWait_Loop;

	/* The animation that plays when the article disappears from the paper. */
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ArticleTransitionOut;

	/* Animation that mostly serves to hide the widget when inactive. */
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ArticleInvisibleInactive_Loop;

	/* The state of the article widget. */
	EArticleState ArticleState = EArticleState::Inactive;

	/* (Do NOT modify this manually!!!) The previous state of the article widget. */
	EArticleState PrevArticleState = EArticleState::Uninitialized;

public:

	/* Fills contents of the widget with the data from the article and displays it. Returns whether or not this was successful (usually fails if the widget is already in the process of activating another article). */
	UFUNCTION(BlueprintCallable)
		bool ActivateArticle(const FArticleData& Article);

	/* Hides the article. Returns whether or not this was successful (usually fails if the widget is not waiting for input). */
	UFUNCTION(BlueprintCallable)
	bool DeactivateArticle();

	UFUNCTION(BlueprintCallable)
	/* Returns whether or not the article widget is
	currently active (displaying an article). */
	EArticleState GetArticleState() const { return ArticleState; }

	/* Returns whether or not the widget is playing a transition animation. */
	bool IsInTransition() const;

protected:
	/* Checks if the active widget state has changed since the last frame and executes code if so. */
	void OnStateChanged(const EArticleState NewState);

	void Tick_Appearing(const FGeometry& MyGeometry, float InDeltaTime);
	void Tick_Waiting(const FGeometry& MyGeometry, float InDeltaTime);
	void Tick_Disappearing(const FGeometry& MyGeometry, float InDeltaTime);

};
