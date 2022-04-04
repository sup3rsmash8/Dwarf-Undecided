// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.

// AUTHOR: Justus Hörberg

#include "UI/Newspaper/NewsArticleWidget.h"
#include "UI/Newspaper/NewsArticle_DataAsset.h"

void UNewsArticleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeArticle();
}

void UNewsArticleWidget::InitializeArticle()
{
	// Empty all fields
	Headline->SetText(FText::GetEmpty());
	CommentText->SetText(FText::GetEmpty());
	StoryText->SetText(FText::GetEmpty());
	Photo->SetBrushFromTexture(nullptr);
	ArticleState = EArticleState::Inactive;
}

bool UNewsArticleWidget::ActivateArticle(const FArticleData& Article)
{
	if (ArticleState != EArticleState::Inactive)
		return false;

	Headline->SetText(Article.GetHeadLine());
	CommentText->SetText(Article.GetComment());
	StoryText->SetText(Article.GetBody());

	UE_LOG(LogTemp, Log, TEXT("%s"), *(Headline->Text.ToString()));
	UTexture2D* Picture = Article.GetPhoto();
	if (Picture)
	{
		Photo->SetBrushFromTexture(Picture);
	}
	else
	{
		// Indicates "don't display it"
		Photo->SetBrushFromTexture(nullptr);
	}

	ArticleState = EArticleState::Appearing;

	return true;
}

bool UNewsArticleWidget::DeactivateArticle()
{
	if (ArticleState != EArticleState::Waiting)
		return false;

	ArticleState = EArticleState::Disappearing;

	return true;
}

void UNewsArticleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{	
	Super::NativeTick(MyGeometry, InDeltaTime);
	// Execute state-initializing code if the article state has changed.
	if (ArticleState != PrevArticleState)
	{
		OnStateChanged(ArticleState);
		PrevArticleState = ArticleState;
	}

	switch (ArticleState)
	{
	default:
	case EArticleState::Inactive:
		break;

	case EArticleState::Appearing:
		Tick_Appearing(MyGeometry, InDeltaTime);
		break;

	case EArticleState::Waiting:
		Tick_Waiting(MyGeometry, InDeltaTime);
		break;

	case EArticleState::Disappearing:
		Tick_Disappearing(MyGeometry, InDeltaTime);
		break;
	}
}

void UNewsArticleWidget::Tick_Appearing(const FGeometry& MyGeometry, float InDeltaTime)
{
	// Exit and wait for input if we're not playing the in-transition animation
	if (!ArticleTransitionIn || !IsAnimationPlaying(ArticleTransitionIn))
		ArticleState = EArticleState::Waiting;
}

void UNewsArticleWidget::Tick_Waiting(const FGeometry& MyGeometry, float InDeltaTime)
{
}

void UNewsArticleWidget::Tick_Disappearing(const FGeometry& MyGeometry, float InDeltaTime)
{
	// Inactivate if we're not playing the out-transition animation
	if (!ArticleTransitionOut || !IsAnimationPlaying(ArticleTransitionOut))
		ArticleState = EArticleState::Inactive;
}

bool UNewsArticleWidget::IsInTransition() const
{
	//return ArticleTransitionIn && IsAnimationPlaying(ArticleTransitionIn) || ArticleTransitionOut && IsAnimationPlaying(ArticleTransitionOut);
	return ArticleState == EArticleState::Appearing || ArticleState == EArticleState::Disappearing;
}

void UNewsArticleWidget::OnStateChanged(const EArticleState NewState)
{
	// As of now, all states involve StopAllAnimations(). 
	// This boilerplate mostly exists out of consistency because not
	// every case has to imply that a new animation is going to play.

	switch (NewState)
	{
	case EArticleState::Inactive:
		// We don't want overlapping animations, so we just stop everything else J.I.C
		if (IsAnyAnimationPlaying())
			StopAllAnimations();

		PlayAnimation(ArticleInvisibleInactive_Loop);
		break;

	case EArticleState::Appearing:
		if (IsAnyAnimationPlaying())
			StopAllAnimations();

		PlayAnimation(ArticleTransitionIn);
		break;

	case EArticleState::Waiting:
		if (IsAnyAnimationPlaying())
			StopAllAnimations();

		PlayAnimation(ArticleWait_Loop);
		break;


	case EArticleState::Disappearing:
		if (IsAnyAnimationPlaying())
			StopAllAnimations();

		PlayAnimation(ArticleTransitionOut);
		break;
	}
}
