// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.


#include "UI/InfoNote/InfoNoteWidget.h"
#include "GameFramework/PlayerState.h"
#include "GameSystem/PlayerMultiController.h"

void UInfoNoteWidget::NativeConstruct()
{
	Super::NativeConstruct();

	NoteState = ENoteState::Inactive;
}

void UInfoNoteWidget::BeginDestroy()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (World->IsGameWorld())
		{
			if (APlayerMultiController* PMC = Cast<APlayerMultiController>(GetWorld()->GetFirstPlayerController()))
			{
				PMC->OnOpenTaskList.RemoveDynamic(this, &UInfoNoteWidget::OnPromptPressConfirm);
			}

			// For safety measures, we call this delegate even on outside 
			// destruction, to prevent any potential softlocks related to 
			// this. Not necessary if already inactive, though.
			if (NoteState != ENoteState::Inactive)
			{
				OnNoteClosed.ExecuteIfBound();
			}
		}
	}

	Super::BeginDestroy();
}

void UInfoNoteWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// DEBUG
	//APlayerController* Ctrl = GetWorld()->GetFirstPlayerController();
	//if (Ctrl->WasInputKeyJustPressed(EKeys::LeftMouseButton))
	//{
	//	OnPromptPressConfirm();
	//}
	// NO MORE DEBUG

	// Execute state-initializing code if the article state has changed.
	if (NoteState != PrevNoteState)
	{
		OnStateChanged(NoteState);
		PrevNoteState = NoteState;
	}

	switch (NoteState)
	{
	default:
	case ENoteState::Inactive:
		break;

	case ENoteState::Appearing:
		Tick_Appearing(MyGeometry, InDeltaTime);
		break;

	case ENoteState::TransitionIn:
		Tick_TransitionIn(MyGeometry, InDeltaTime);
		break;

	case ENoteState::Waiting:
		Tick_Waiting(MyGeometry, InDeltaTime);
		break;

	case ENoteState::TransitionOut:
		Tick_TransitionOut(MyGeometry, InDeltaTime);
		break;

	case ENoteState::Disappearing:
		Tick_Disappearing(MyGeometry, InDeltaTime);
		break;
	}
}

void UInfoNoteWidget::OnStateChanged(const ENoteState NewState)
{
	// As of now, all states involve StopAllAnimations(). 
	// This boilerplate mostly exists out of consistency because not
	// every case has to imply that a new animation is going to play.

	switch (NewState)
	{
	case ENoteState::Inactive:
		if (PrevNoteState != ENoteState::Uninitialized)
		{
			Deactivate();
		}

		// We don't want overlapping animations, so we just stop everything else J.I.C
		if (IsAnyAnimationPlaying())
			StopAllAnimations();

		PlayAnimation(NoteInvisibleInactive_Loop);
		break;

	case ENoteState::Appearing:
		if (IsAnyAnimationPlaying())
			StopAllAnimations();



		PlayAnimation(NoteAppear);
		break;

	case ENoteState::TransitionIn:
		if (IsAnyAnimationPlaying())
			StopAllAnimations();

		UE_LOG(LogTemp, Log, TEXT("%s, %i"), *InfoNotes[NoteIndex].NoteText.ToString(), NoteIndex);
		if (NoteIndex < InfoNotes.Num())
			SetNoteText(InfoNotes[NoteIndex]);

		PlayAnimation(NoteTransitionIn);
		break;

	case ENoteState::Waiting:
		if (IsAnyAnimationPlaying())
			StopAllAnimations();

		PlayAnimation(NoteWait_Loop);
		break;

	case ENoteState::TransitionOut:
		if (IsAnyAnimationPlaying())
			StopAllAnimations();

		PlayAnimation(NoteTransitionOut);
		break;


	case ENoteState::Disappearing:
		if (IsAnyAnimationPlaying())
			StopAllAnimations();

		PlayAnimation(NoteDisappear);
		break;
	}
}

void UInfoNoteWidget::Tick_Appearing(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (!NoteAppear || !IsAnimationPlaying(NoteAppear))
		NoteState = ENoteState::TransitionIn;
}

void UInfoNoteWidget::Tick_TransitionIn(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (!NoteTransitionIn || !IsAnimationPlaying(NoteTransitionIn))
		NoteState = ENoteState::Waiting;
}

void UInfoNoteWidget::Tick_Waiting(const FGeometry& MyGeometry, float InDeltaTime)
{
}

void UInfoNoteWidget::Tick_TransitionOut(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (!NoteTransitionOut || !IsAnimationPlaying(NoteTransitionOut))
	{
		NoteIndex++;

		ENoteState NextState;

		if (NoteIndex >= InfoNotes.Num())
		{
			NextState = ENoteState::Disappearing;
		}
		else
		{
			NextState = ENoteState::TransitionIn;
		}

		NoteState = NextState;
	}
}

void UInfoNoteWidget::Tick_Disappearing(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (!NoteDisappear || !IsAnimationPlaying(NoteDisappear))
	{
		NoteState = ENoteState::Inactive;
	}
}

bool UInfoNoteWidget::DisplayNote(TArray<FInfoNoteData> NoteInfo, const FOnNoteEvent& OnNoteOpenedDelegate, const FOnNoteEvent& OnNoteClosedDelegate)
{
	bool bIsTrue = NoteState == ENoteState::Inactive && NoteInfo.Num() > 0;

	if (bIsTrue)
	{
		InfoNotes = NoteInfo;

		// Bind open delegate. Clear otherwise, in case of an invalid bond
		if (OnNoteOpenedDelegate.IsBound())
			OnNoteOpened = OnNoteOpenedDelegate;
		else
			OnNoteOpened.Clear();

		// Bind closed delegate. Clear otherwise, in case of an invalid bond
		if (OnNoteClosedDelegate.IsBound())
			OnNoteClosed = OnNoteClosedDelegate;
		else
			OnNoteClosed.Clear();

		Activate();
	}

	return bIsTrue;
}

bool UInfoNoteWidget::UndisplayNote()
{
	bool bIsTrue = NoteState == ENoteState::Waiting;

	if (bIsTrue)
	{
		NoteState = ENoteState::TransitionOut;
	}

	return bIsTrue;
}

void UInfoNoteWidget::Activate()
{
	UE_LOG(LogTemp, Log, TEXT("einstein"));
	if (!IsInViewport())
		AddToViewport();

	if (APlayerMultiController* PMC = Cast<APlayerMultiController>(GetWorld()->GetFirstPlayerController()))
	{
		PMC->OnOpenTaskList.AddDynamic(this, &UInfoNoteWidget::OnPromptPressConfirm);
	}

	if (OnNoteOpened.IsBound())
		OnNoteOpened.Execute();

	NoteState = ENoteState::Appearing;
	NoteIndex = 0;
}

void UInfoNoteWidget::Deactivate()
{
	if (APlayerMultiController* PMC = Cast<APlayerMultiController>(GetWorld()->GetFirstPlayerController()))
	{
		PMC->OnOpenTaskList.RemoveDynamic(this, &UInfoNoteWidget::OnPromptPressConfirm);
	}

	if (OnNoteClosed.IsBound())
		OnNoteClosed.Execute();

	NoteState = ENoteState::Inactive;
	NoteIndex = 0;

	if (IsInViewport())
		RemoveFromViewport();
}

void UInfoNoteWidget::SetNoteText(FInfoNoteData NoteData)
{
	FText Text = NoteData.NoteText;
	NoteText->SetText(Text);
}

void UInfoNoteWidget::OnPromptPressConfirm()
{
	switch (NoteState)
	{
	default:
		break;

	case ENoteState::Waiting:
		NoteState = ENoteState::TransitionOut;
		break;
	}
}

bool UInfoNoteWidget::IsInTransition() const
{
	return NoteState == ENoteState::TransitionIn || NoteState == ENoteState::TransitionOut;
}

bool UInfoNoteWidget::IsInactive() const
{
	return NoteState == ENoteState::Inactive;
}
