// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.

// AUTHOR: Justus H�rberg

#include "UI/Subtitles/DialogWidget.h"

#include "Animation/UMGSequencePlayer.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "CustomerAI/BaseCustomer.h"
#include "GameSystem/GM_AdventurerInformationComponent.h"
#include "GameSystem/GM_BargainComponent.h"
#include "GameSystem/GP3GameMode.h"
#include "GameSystem/PlayerMultiController.h"
#include "Kismet/GameplayStatics.h"


void UDialogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsActive = false;
	
	//This is commented out so that we don't overlap the interact input <--(no longer relevant)
	
	//APlayerController* PlayerControl = GetWorld()->GetFirstPlayerController();
	//if (PlayerControl && PlayerControl->InputComponent)
	//{
	//	//FInputActionBinding Bind = FInputActionBinding("Interact", IE_Pressed);
	//	//FInputActionUnifiedDelegate DelegateAction = FInputActionUnifiedDelegate();
	//	//DelegateAction.
	//	//Bind.ActionDelegate = DelegateAction;
	//	//ConfirmBinding = PlayerControl->InputComponent->AddActionBinding(Bind);
	//	ConfirmBinding = PlayerControl->InputComponent->BindAction("Confirm", IE_Pressed, this, &UDialogWidget::OnPromptPressConfirm);
	//	ConfirmBinding.bConsumeInput = false;
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Log, TEXT("DialogWidget: Input component could not be found, dialog boxes will not be closable."));
	//}

	TempSeqPlayer = PlayAnimation(TextBoxInactive_Loop, 0, 0);
}

void UDialogWidget::BeginDestroy()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (World->IsGameWorld())
		{
			if (APlayerMultiController* PMC = Cast<APlayerMultiController>(GetWorld()->GetFirstPlayerController()))
			{
				PMC->OnConfirm.RemoveDynamic(this, &UDialogWidget::OnPromptPressConfirm);
			}

			// For safety measures, we call this delegate even on outside 
			// destruction, to prevent any potential softlocks related to 
			// this. Not necessary if already inactive, though.
			if (CurrentDialogBoxState != EDialogBoxState::Inactive)
			{
				OnDialogBoxFinished.ExecuteIfBound(0);
			}
		}
	}

	Super::BeginDestroy();
}

void UDialogWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsActive)
		return;

	switch (CurrentDialogBoxState)
	{
	case EDialogBoxState::Inactive:
		break;

	case EDialogBoxState::Opening:
		Tick_Opening(MyGeometry, InDeltaTime);
		break;

	case EDialogBoxState::Typing:
		Tick_Typing(MyGeometry, InDeltaTime);
		break;

	case EDialogBoxState::WaitingForInput:
		Tick_WaitingForInput(MyGeometry, InDeltaTime);
		break;

	case EDialogBoxState::Closing:
		Tick_Closing(MyGeometry, InDeltaTime);
		break;
	}
}

void UDialogWidget::InitializeInputComponent()
{
	Super::InitializeInputComponent();
}

void UDialogWidget::InitiateTextbox(TArray<FDialogBox> DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FStringFormatArg> Args)
{

	if (!bIsActive)
	{
		LoadedDialogBoxes = DialogToDisplay;
		bIsActive = true;
		bIsSkipping = false;
		MessageIndex = 0;
		MessageCursorPosition = 0;
		CurrentText.Empty();
		Arguments = Args;
		if(Arguments.Num() > 0)
		{
			FString TextName = Arguments[0].StringValue;
			CustomerName = FText::AsCultureInvariant(TextName);
		}
		else
		{
			//CustomerName = FText::GetEmpty();
		}


		if (TempSeqPlayer)
			TempSeqPlayer->Stop();

		TempSeqPlayer = PlayAnimation(TextBoxInactive_Loop, 0, 0);

		OnDialogBoxFinished = DialogBoxFinishedDelegate;

		TextboxActivate();
	}
}

void UDialogWidget::InitiateTextbox_BP(TArray<FDialogBox> DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FString> Args)
{
	if (!bIsActive)
	{
		LoadedDialogBoxes = DialogToDisplay;
		bIsActive = true;
		bIsSkipping = false;
		MessageIndex = 0;
		MessageCursorPosition = 0;
		CurrentText.Empty();
		Arguments.Reset();
		for (const FString Arg : Args)
		{
			Arguments.Add(Arg);
		}

		TempSeqPlayer = PlayAnimation(TextBoxInactive_Loop, 0, 0);

		OnDialogBoxFinished = DialogBoxFinishedDelegate;

		TextboxActivate();
	}
}

void UDialogWidget::InitiateTextbox_BP_Asset(UDialogAsset* DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FString> Args)
{
	TArray<FDialogBox> DialogInAsset;

	if (DialogToDisplay)
	{
		if (DialogToDisplay->DialogBoxes.Num() > 0)
		{
			DialogInAsset = DialogToDisplay->DialogBoxes;
		}
		else
		{
			// Empty means error
			DialogInAsset.Add(FDialogBox::GetErrorDialog(EDialogErrorReason::DialogAssetEmpty));
		}
	}
	else
	{
		// Null means error
		DialogInAsset.Add(FDialogBox::GetErrorDialog(EDialogErrorReason::DialogAssetNull));
	}

	InitiateTextbox_BP(DialogInAsset, DialogBoxFinishedDelegate, Args);
}

void UDialogWidget::InitiateTextbox(UDialogAsset* DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FStringFormatArg> Args)
{
	TArray<FDialogBox> DialogInAsset;

	if (DialogToDisplay)
	{
		if (DialogToDisplay->DialogBoxes.Num() > 0)
		{
			DialogInAsset = DialogToDisplay->DialogBoxes;
		}
		else
		{
			// Empty means error
			DialogInAsset.Add(FDialogBox::GetErrorDialog(EDialogErrorReason::DialogAssetEmpty));
		}
	}
	else
	{
		// Null means error
		DialogInAsset.Add(FDialogBox::GetErrorDialog(EDialogErrorReason::DialogAssetNull));
	}

	InitiateTextbox(DialogInAsset, DialogBoxFinishedDelegate, Args);
}

//void UDialogWidget::InitiateTextbox(TSubclassOf<UDialogAsset> DialogToDisplay, TArray<FString> Args)
//{
//	if (!bIsActive)
//	{
//		DialogAsset = DialogToDisplay;
//		bIsActive = true;
//		MessageIndex = 0;
//		CurrentText = FString();
//		Arguments = Args;
//		TextboxActivate();
//	}
//}

FDialogBox UDialogWidget::LoadAndInitializeText(const int Index) const
{
	if (Index >= 0 && Index <= LoadedDialogBoxes.Num() - 1)
	{
		return LoadedDialogBoxes[Index];
	}

	// Return error
	return FDialogBox::GetErrorDialog(EDialogErrorReason::IndexOutOfRange);
}

void UDialogWidget::TextboxActivate()
{
	auto* GM = Cast<AGP3GameMode>(UGameplayStatics::GetGameMode(this));

	FText Name;
	if (GM->GetBargainComponent()->CurrentCustomer)
	{
		Name = FText::FromString(GM->GetBargainComponent()->CurrentCustomer->NameOfCustomer);
	}
	else
	{
		Name = FText::FromString(TEXT("Undefined"));
	}
	
	CustomerName = Name;
	if (APlayerMultiController* PMC = Cast<APlayerMultiController>(GetWorld()->GetFirstPlayerController()))
	{
		PMC->OnConfirm.AddDynamic(this, &UDialogWidget::OnPromptPressConfirm);
	}
	ChangeDialogBoxState(EDialogBoxState::Opening);
}

void UDialogWidget::TextboxDeactivate()
{
	if (bIsActive)
	{
		bIsActive = false;
		if (APlayerMultiController* PMC = Cast<APlayerMultiController>(GetWorld()->GetFirstPlayerController()))
		{
			PMC->OnConfirm.RemoveDynamic(this, &UDialogWidget::OnPromptPressConfirm);
		}
		ChangeDialogBoxState(EDialogBoxState::Inactive);

		if (TempSeqPlayer)
			TempSeqPlayer->Stop();
		
		TempSeqPlayer = PlayAnimation(TextBoxInactive_Loop);
		if (NameTextBlock)
		{
			NameTextBlock->SetText(FText::GetEmpty());
		}

		if (OnDialogBoxFinished.IsBound())
		{
			OnDialogBoxFinished.Execute(0);
			OnDialogBoxFinished.Clear();
		}
	}
	
}

void UDialogWidget::ChangeDialogBoxState(EDialogBoxState NewState)
{
	CurrentDialogBoxState = NewState;
	switch (CurrentDialogBoxState)
	{
	case EDialogBoxState::Inactive:
		break;

	case EDialogBoxState::Opening:
		StateEnter_Opening();
		break;

	case EDialogBoxState::Typing:
		StateEnter_Typing();
		break;

	case EDialogBoxState::WaitingForInput:
		StateEnter_WaitingForInput();
		break;

	case EDialogBoxState::Closing:
		StateEnter_Closing();
		break;
	}
}

#pragma region Opening
void UDialogWidget::StateEnter_Opening()
{
	if (TextBoxAppear)
	{
		if (TempSeqPlayer)
			TempSeqPlayer->Stop();
		
		TempSeqPlayer = PlayAnimation(TextBoxAppear);
	}
}

void UDialogWidget::Tick_Opening(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (TempSeqPlayer)
	{
		// It's gonna be playing from the point the state is entered.
		if (!IsAnimationPlaying(TextBoxAppear))
		{
			ChangeDialogBoxState(EDialogBoxState::Typing);
		}
	}
}
#pragma endregion 

#pragma region Typing
void UDialogWidget::StateEnter_Typing()
{
	FDialogBox CurrentDialogBox = LoadAndInitializeText(MessageIndex);
	
	bIsSkippable = CurrentDialogBox.bSkippable;
	
	bIsSkipping = false;

	// Convert to char array to fit in the arguments
	// (Is this garbage collected...?)
	const TCHAR* MessageToChr = ToCStr(CurrentDialogBox.Message.ToString());
	MessageString = FString::Format(MessageToChr, Arguments);
	
	CurrentText.Empty();
	
	if (DialogTextBlock)
	{
		DialogTextBlock->SetText(FText::GetEmpty());
	}
	if(NameTextBlock)
	{
		NameTextBlock->SetText(CustomerName);
	}

	MessageCursorPosition = 0;
	MessageTimer = TextSpeed;
}

void UDialogWidget::Tick_Typing(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (MessageTimer <= 0)
	{
		// Get that char and add to the text.
		// Not certain that this is gonna work for multibyte characters... But maybe? idk
		FString Char;
		Char += MessageString[MessageCursorPosition];
		
		CurrentText += Char;
		MessageCursorPosition++;
		
		MessageTimer = bIsSkipping ? TextSpeedFast : TextSpeed;

		DialogTextBlock->SetText(FText::FromString(CurrentText));

		// If we're done printing the message, wait for input.
		if (MessageCursorPosition >= MessageString.Len())
		{
			ChangeDialogBoxState(EDialogBoxState::WaitingForInput);
		}
	}
	else
	{
		MessageTimer -= InDeltaTime;
	}
}
#pragma endregion 

#pragma region WaitingForInput
void UDialogWidget::StateEnter_WaitingForInput()
{
	TempSeqPlayer = PlayAnimation(TextBoxWait_Loop, 0, 0);
}

void UDialogWidget::Tick_WaitingForInput(const FGeometry& MyGeometry, float InDeltaTime)
{

}
#pragma endregion 

#pragma region Closing
void UDialogWidget::StateEnter_Closing()
{
	if (TempSeqPlayer)
		TempSeqPlayer->Stop();
	
	TempSeqPlayer = PlayAnimation(TextBoxDisappear);

	CurrentText.Empty();

	if (DialogTextBlock)
	{
		DialogTextBlock->SetText(FText::GetEmpty());
	}
}

void UDialogWidget::Tick_Closing(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (TempSeqPlayer)
	{
		if (!IsAnimationPlaying(TextBoxDisappear))
		{
			TextboxDeactivate();
		}
	}
}
#pragma endregion 

void UDialogWidget::OnPromptPressConfirm()
{
	UE_LOG(LogTemp, Log, TEXT("Prompt"));
	if (!bIsActive)
		return;
	switch (CurrentDialogBoxState)
	{
	default:
		break;
		
	case EDialogBoxState::Typing:
		bIsSkipping = true;
		break;

	case EDialogBoxState::WaitingForInput:
		PromptExecute();
		break;
	}
}

void UDialogWidget::PromptExecute()
{
	if (GetCurrentDialogBoxState() != EDialogBoxState::WaitingForInput)
		return;

	// If we haven't read all of the dialog boxes yet, we will
	// jump to the next one in index and repeat the typing process.
	
	if (!IsOnFinalDialogBox())
	{
		MessageIndex++;

		ChangeDialogBoxState(EDialogBoxState::Typing);

		return;
	}

	// If for whatever reason the above fails,
	// just exit out of the textbox.
	
	ChangeDialogBoxState(EDialogBoxState::Closing);
}

bool UDialogWidget::IsOnFinalDialogBox()
{
	return MessageIndex >= LoadedDialogBoxes.Num() - 1;
}
