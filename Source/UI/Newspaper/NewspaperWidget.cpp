// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.

// AUTHOR: Justus Hörberg

#include "UI/Newspaper/NewspaperWidget.h"
#include "UI/HUD/GameHUD.h"
#include "GameSystem/PlayerMultiController.h"
#include "Player/PlayerCharacter.h"

void UNewspaperWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Declare them pages
	//if (PageOverlay)
	//{
	//	if (PageTexture)
	//		PageOverlay->AddChild(PageTexture);

	//	if (NewsArticleWidget)
	//		PageOverlay->AddChild(NewsArticleWidget);
	//}
}

void UNewspaperWidget::BeginDestroy()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (World->IsGameWorld())
		{
			if (APlayerMultiController* PMC = Cast<APlayerMultiController>(GetWorld()->GetFirstPlayerController()))
			{
				PMC->OnConfirm.RemoveDynamic(this, &UNewspaperWidget::OnPromptPressConfirm);
			}

			// For safety measures, we call this delegate even on outside 
			// destruction, to prevent any potential softlocks related to 
			// this. Not necessary if already inactive, though.
			if (NewspaperState != ENewspaperState::Inactive)
			{
				OnNewspaperClosed.ExecuteIfBound();
			}
		}
	}

	Super::BeginDestroy();
}

void UNewspaperWidget::InitializeInputComponent()
{
	Super::InitializeInputComponent();
}

bool UNewspaperWidget::GenerateAndDisplayNewspaper(TArray<FArticleData> Articles, const FOnNewspaperEvent& OnNewspaperOpenDelegate, const FOnNewspaperEvent& OnNewspaperClosedDelegate, const FOnNewspaperEvent& OnArticleSpawnDelegate)
{
	// Articles must have a length larger than 0. What else are we supposed to
	// display, eh?
	if (NewspaperState != ENewspaperState::Inactive || Articles.Num() <= 0)
		return false;

	// Bind open delegate. Clear otherwise, in case of an invalid bond
	if (OnNewspaperOpenDelegate.IsBound())
		OnNewspaperOpened = OnNewspaperOpenDelegate;
	else
		OnNewspaperOpened.Clear();

	// Bind closed delegate. Clear otherwise, in case of an invalid bond
	if (OnNewspaperClosedDelegate.IsBound())
		OnNewspaperClosed = OnNewspaperClosedDelegate;
	else
		OnNewspaperClosed.Clear();

	// Bind article spawn delegate. Clear otherwise, in case of an invalid bond
	if (OnArticleSpawnDelegate.IsBound())
		OnArticleSpawn = OnArticleSpawnDelegate;
	else
		OnArticleSpawn.Clear();

	NewsArticles = Articles;
	NewspaperState = ENewspaperState::Appearing;
	ArticleIndex = 0;
	return true;
}

void UNewspaperWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(Check < 1)
		Check += InDeltaTime;

	// DEBUG
	//APlayerController* Ctrl = GetWorld()->GetFirstPlayerController();
	//if (Ctrl->WasInputKeyJustPressed(EKeys::LeftMouseButton))
	//{
	//	OnPromptPressConfirm();
	//}
	// NO MORE DEBUG

	// Execute state-initializing code if the article state has changed.
	if (NewspaperState != PrevNewspaperState)
	{
		// This must occur before PrevNewspaperState gets set,
		// in case we want to execute difference-dependent code.
		OnStateChanged(NewspaperState);
		PrevNewspaperState = NewspaperState;
	}

	switch (NewspaperState)
	{
	default:
	case ENewspaperState::Inactive:
		break;

	case ENewspaperState::Appearing:
		Tick_Appearing(MyGeometry, InDeltaTime);
		break;

	case ENewspaperState::OnScreen_TransitionIn:
		Tick_OnScreen_TransitionIn(MyGeometry, InDeltaTime);
		break;

	case ENewspaperState::OnScreen:
		Tick_OnScreen(MyGeometry, InDeltaTime);
		break;

	case ENewspaperState::OnScreen_TransitionOut:
		Tick_OnScreen_TransitionOut(MyGeometry, InDeltaTime);
		break;

	case ENewspaperState::Disappearing:
		Tick_Disappearing(MyGeometry, InDeltaTime);
		break;
	}
}

void UNewspaperWidget::OnStateChanged(const ENewspaperState NewState)
{
	switch (NewState)
	{
	case ENewspaperState::Appearing:
		// It's when we awaken the newspaper that the delegate
		// is supposed to run.
		if (PrevNewspaperState == ENewspaperState::Inactive)
		{
			OnNewspaperOpened.ExecuteIfBound();

			if (APlayerMultiController* PMC = Cast<APlayerMultiController>(GetWorld()->GetFirstPlayerController()))
			{
				PMC->OnConfirm.AddDynamic(this, &UNewspaperWidget::OnPromptPressConfirm);
			}
		}
		
		if (IsAnyAnimationPlaying())
			StopAllAnimations();
		// For safety reasons, initialize here too
		ArticleIndex = 0;
		PlayAnimation(NewspaperAppear);
		break;

	case ENewspaperState::OnScreen_TransitionIn:
		if (IsAnyAnimationPlaying())
			StopAllAnimations();

		OnArticleSpawn.ExecuteIfBound();

		PlayAnimation(NewspaperWait_Loop, 0, 0);
		break;

	case ENewspaperState::OnScreen_TransitionOut:
		if (IsAnyAnimationPlaying())
			StopAllAnimations();

		NewsArticleWidget->DeactivateArticle();

		PlayAnimation(NewspaperWait_Loop, 0, 0);
		break;

	case ENewspaperState::Disappearing:
		if (IsAnyAnimationPlaying())
			StopAllAnimations();
		////Hjalmar 
		//auto* PlayerChar = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		//PlayerChar->DisplayNote();
		////Hjalmar
		PlayAnimation(NewspaperDisappear);
		break;

	case ENewspaperState::Inactive:
		if (PrevNewspaperState == ENewspaperState::Uninitialized)
			return;

		if (APlayerMultiController* PMC = Cast<APlayerMultiController>(GetWorld()->GetFirstPlayerController()))
		{
			PMC->OnConfirm.RemoveDynamic(this, &UNewspaperWidget::OnPromptPressConfirm);
		}

		// Contrary to 'Appearing', this should always execute,
		// since uninitializers will be assigned to this delegate.
		OnNewspaperClosed.ExecuteIfBound();

		if (bRemoveSelfOnInactive)
		{
			UE_LOG(LogTemp, Log, TEXT("REemoval"));
			RemoveFromViewport();
		}
		else
		{
			if (IsAnyAnimationPlaying())
				StopAllAnimations();

			PlayAnimation(NewspaperInactiveInvisible_Loop);
		}

		break;
	}
}

// Newspaper is popping up...
void UNewspaperWidget::Tick_Appearing(const FGeometry& MyGeometry, float InDeltaTime)
{
	// This animation is called at the beginning of the state.
	// Once it reaches the end, IsAnimationPlaying will return false.
	if (!NewspaperAppear || !IsAnimationPlaying(NewspaperAppear))
		NewspaperState = ENewspaperState::OnScreen_TransitionIn;
}

// Article widget is fading in...
void UNewspaperWidget::Tick_OnScreen_TransitionIn(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (ArticleIndex >= 0 && ArticleIndex < NewsArticles.Num())
	{
		UE_LOG(LogTemp, Log, TEXT("%i"), ArticleIndex);
		if (!NewsArticleWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("Nope :dd("));
			return;
		}

		if (NewsArticleWidget->ActivateArticle(NewsArticles[ArticleIndex]))
		{
			NewspaperState = ENewspaperState::OnScreen;
		}
	}
	// Just close the thing if there's no headlines. Don't even bother, it's all fake news
	else
	{
		NewspaperState = ENewspaperState::Disappearing;
	}
}

// Waiting for input...
void UNewspaperWidget::Tick_OnScreen(const FGeometry& MyGeometry, float InDeltaTime)
{
	// The reason we're doing this instead of putting it
	// directly in the input delegate is to make sure that
	// the state change always happens before OnStateChanged()
	// is called. We could have a state buffer that gets
	// managed at the end of the tick, which would prevent
	// large workarounds like this, but it didn't really happen...
	//if (bConfirmInputBuffer)
	//{
	//	NewspaperState = ENewspaperState::OnScreen_TransitionOut;
	//	bConfirmInputBuffer = false;
	//}
}

// Article widget is fading out...
void UNewspaperWidget::Tick_OnScreen_TransitionOut(const FGeometry& MyGeometry, float InDeltaTime)
{
	EArticleState ArticleState = NewsArticleWidget->GetArticleState();

	// Make the decision once the article state has fully faded out (is inactive).
	if (ArticleState == EArticleState::Inactive)
	{
		ArticleIndex++;

		// If we're out of range, that means we're done.
		if (ArticleIndex >= NewsArticles.Num())
		{
			NewspaperState = ENewspaperState::Disappearing;
		}
		// Loop back around otherwise.
		else
		{
			NewspaperState = ENewspaperState::OnScreen_TransitionIn;
		}
	}
}

// Newspaper disappears. End of cycle.
void UNewspaperWidget::Tick_Disappearing(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (!NewspaperDisappear || !IsAnimationPlaying(NewspaperDisappear))
		NewspaperState = ENewspaperState::Inactive;
}

void UNewspaperWidget::OnPromptPressConfirm()
{
	if (Check < 1.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("Failed confirm"));
		return;
	}
	Check = 0.0f;
	UE_LOG(LogTemp, Log, TEXT("Prompt before switch"));
	switch (NewspaperState)
	{
	case ENewspaperState::OnScreen:

		UE_LOG(LogTemp, Log, TEXT("Continue in switch"));
		NewspaperState = ENewspaperState::OnScreen_TransitionOut;
		break;

	default:
		UE_LOG(LogTemp, Log, TEXT("Default in switch"));
		break;
	}
}
