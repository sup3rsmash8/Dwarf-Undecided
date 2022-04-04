// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.
// Author: Justus Hörberg
// Secondary Author: Hjalmar Andersson

#include "UI/HUD/GameHUD.h"
#include "UI/ItemDisplayWidget.h"
#include "UI/GoldCarryWidget.h"
#include "GameSystem/PlayerMultiController.h"
#include "GameSystem/GP_GameInstance.h"
#include "Player/PlayerCharacter.h"
//#include "Kismet/GameplayStatics.h"

AGameHUD::AGameHUD()
{
	//ItemDisplayWidget = CreateDefaultSubobject<UItemDisplayWidgetComponent>(TEXT("Item Display Widget"));
	//GoldWidget = CreateDefaultSubobject<UGoldWidgetComponent>(TEXT("Gold Widget"));
}

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	UGP_GameInstance* GInst = GetWorld()->GetGameInstance<UGP_GameInstance>();

	if (GInst)
	{
		// Spawn item widget and project to viewport
		if (ItemDisplayWidget)
		{
			UItemDisplayWidget* CreateItemWidget = Cast<UItemDisplayWidget>(UUserWidget::CreateWidgetInstance(*GInst, ItemDisplayWidget, FName(TEXT("Item Display"))));
			
			if (CreateItemWidget)
			{
				ItemDisplayWidget_Inst = CreateItemWidget;
				ItemDisplayWidget_Inst->SetOwningPlayer(PlayerOwner);
				ItemDisplayWidget_Inst->AddToViewport();
			}
		}

		// Spawn gold widget and project to viewport
		if (GoldCarryWidget)
		{
			UGoldCarryWidget* CreateGoldWidget = Cast<UGoldCarryWidget>(UUserWidget::CreateWidgetInstance(*GInst, GoldCarryWidget, FName(TEXT("Gold Display"))));


			if (CreateGoldWidget)
			{
				GoldCarryWidget_Inst = CreateGoldWidget;
				GoldCarryWidget_Inst->SetOwningPlayer(PlayerOwner);
				GoldCarryWidget_Inst->AddToViewport();
			}
		}

		// Spawn dialog widget and project to viewport
		if (DialogWidget)
		{
			UDialogWidget* CreateDialogWidget = Cast<UDialogWidget>(UUserWidget::CreateWidgetInstance(*GInst, DialogWidget, FName(TEXT("Dialog Box"))));

			if (CreateDialogWidget)
			{
				DialogWidget_Inst = CreateDialogWidget;
				DialogWidget_Inst-> SetOwningPlayer(PlayerOwner);
				DialogWidget_Inst->AddToViewport();
				DialogWidget_Inst->SetDesiredSizeInViewport(FVector2D(1920.f, 1080.f));
			}
		}

		// Spawn newspaper widget and project to viewport
		if (NewspaperWidget)
		{
			UNewspaperWidget* CreateNewspaperWidget = Cast<UNewspaperWidget>(UUserWidget::CreateWidgetInstance(*GInst, NewspaperWidget, FName(TEXT("Newspaper"))));

			if (CreateNewspaperWidget)
			{
				NewspaperWidget_Inst = CreateNewspaperWidget;
				NewspaperWidget_Inst->AddToViewport();
				NewspaperWidget_Inst->SetDesiredSizeInViewport(FVector2D(1920.f, 1080.f));
			}
		}

		// Spawn note widget and project to viewport
		if (InfoNoteWidget)
		{
			UInfoNoteWidget* CreateInfoNoteWidget = Cast<UInfoNoteWidget>(UUserWidget::CreateWidgetInstance(*GInst, InfoNoteWidget, FName(TEXT("Post it-lapp"))));

			if (CreateInfoNoteWidget)
			{
				InfoNoteWidget_Inst = CreateInfoNoteWidget;
				InfoNoteWidget_Inst->AddToViewport();
				InfoNoteWidget_Inst->SetDesiredSizeInViewport(FVector2D(1920.f, 1080.f));
			}
		}
	}

	// Assign tasklist functionality
	auto* PMC = Cast<APlayerMultiController>(PlayerOwner);
	if (PMC)
	{
		UE_LOG(LogTemp, Log, TEXT("sssssss"));
		PMC->OnOpenTaskList.AddDynamic(this, &AGameHUD::OnTaskListDisplay);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Dynamamam"));
	}
}

void AGameHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AGameHUD::BeginDestroy()
{
	Super::BeginDestroy();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (!World->IsGameWorld())
	{
		return;
	}

	auto* PMC = Cast<APlayerMultiController>(PlayerOwner);
	if (PMC)
	{
		PMC->OnOpenTaskList.RemoveDynamic(this, &AGameHUD::OnTaskListDisplay);
	}
}

UItemDisplayWidget* AGameHUD::GetItemDisplayWidget() const
{
	return ItemDisplayWidget_Inst;
}

UGoldCarryWidget* AGameHUD::GetGoldCarryWidget() const
{
	return GoldCarryWidget_Inst;
}

UDialogWidget* AGameHUD::GetDialogWidget() const
{
	return DialogWidget_Inst;
}

UNewspaperWidget* AGameHUD::GetNewspaperWidget() const 
{
	return NewspaperWidget_Inst;
}

UInfoNoteWidget* AGameHUD::GetInfoNoteWidget() const
{
	return InfoNoteWidget_Inst;
}

void AGameHUD::InitiateTextbox(UDialogAsset* DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FStringFormatArg> Args)
{
	if (DialogWidget_Inst)
	{
		UE_LOG(LogTemp, Log, TEXT("KKKEKEKEn"));
		//DialogWidget_Inst->siz
		DialogWidget_Inst->InitiateTextbox(DialogToDisplay, DialogBoxFinishedDelegate, Args);
	}
}

void AGameHUD::InitiateTextbox(TArray<FDialogBox> DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FStringFormatArg> Args)
{
	if (DialogWidget_Inst)
	{
		DialogWidget_Inst->InitiateTextbox(DialogToDisplay, DialogBoxFinishedDelegate, Args);
	}
}

void AGameHUD::InitiateTextbox_Asset(UDialogAsset* DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FString> Args)
{
	if (DialogWidget_Inst)
	{
		DialogWidget_Inst->InitiateTextbox_BP_Asset(DialogToDisplay, DialogBoxFinishedDelegate, Args);
	}
}

void AGameHUD::InitiateTextbox_Array(TArray<FDialogBox> DialogToDisplay, const FOnDialogBoxEvent& DialogBoxFinishedDelegate, TArray<FString> Args)
{
	if (DialogWidget_Inst)
	{
		DialogWidget_Inst->InitiateTextbox_BP(DialogToDisplay, DialogBoxFinishedDelegate, Args);
	}
}

bool AGameHUD::DisplayNewspaper(TArray<FArticleData> Articles, const FOnNewspaperEvent& OnNewspaperOpenDelegate, const FOnNewspaperEvent& OnNewspaperClosedDelegate, const FOnNewspaperEvent& OnArticleSpawn)
{
	if (!NewspaperWidget_Inst || !NewspaperWidget_Inst->IsInactive())
		return false;

	//UGameplayStatics::GetPlayerController(GetWorld(), 0)->
	UE_LOG(LogTemp, Log, TEXT("YO YO YO"));

	return NewspaperWidget_Inst->GenerateAndDisplayNewspaper(Articles, OnNewspaperOpenDelegate, OnNewspaperClosedDelegate, OnArticleSpawn);
}

bool AGameHUD::DisplayNote(TArray<FInfoNoteData> NoteInfo, const FOnNoteEvent& OnNoteOpenedDelegate, const FOnNoteEvent& OnNoteClosedDelegate)
{
	if (!InfoNoteWidget_Inst || !InfoNoteWidget_Inst->IsInactive())
		return false;

	UE_LOG(LogTemp, Log, TEXT("YO YO YOu"));

	return InfoNoteWidget_Inst->DisplayNote(NoteInfo, OnNoteOpenedDelegate, OnNoteClosedDelegate);
}

bool AGameHUD::IsTextboxAlreadyActive() const
{
	if (DialogWidget_Inst)
	{
		return DialogWidget_Inst->IsCurrentlyActive();
	}

	return false;
}

void AGameHUD::RemoveNewspaperOnInactive()
{
	//UE_LOG(LogTemp, Log, TEXT("Removin"));

	//if (NewspaperWidget_Inst)
	//	NewspaperWidget_Inst->RemoveFromViewport();
}

void AGameHUD::OnTaskListDisplay()
{
	if (!PlayerOwner || !TaskNoteData)
		return;

	UE_LOG(LogTemp, Log, TEXT("Pass1"));

	auto* PlayerChar = Cast<APlayerCharacter>(PlayerOwner->GetPawn());
	if (!PlayerChar || PlayerChar->IsRestricted())
		return;

	UE_LOG(LogTemp, Log, TEXT("Pass2"));

	if (!PlayerChar->IsRestricted())
	{
		PlayerChar->RestrictMovementInput(true);

		TArray<FInfoNoteData> NoteData;
		TaskNoteData->GetInfoNotes(NoteData);

		//HJ
		PlayerChar->DisplayNote(true);
		//HJ
		FOnNoteEvent Open;

		FOnNoteEvent Close;
		Close.BindDynamic(this, &AGameHUD::UnrestrictPlayerMovement);

		if (Close.IsBound())
		UE_LOG(LogTemp, Log, TEXT("Pass3"));

		DisplayNote(NoteData, Open, Close);
	}
}

void AGameHUD::UnrestrictPlayerMovement()
{
	if (!PlayerOwner)
		return;

	UE_LOG(LogTemp, Log, TEXT("Pass4"));

	auto* PlayerChar = Cast<APlayerCharacter>(PlayerOwner->GetPawn());
	if (!PlayerChar)
		return;
	//HJ
	PlayerChar->DisplayNote(false);
	//HJ
	UE_LOG(LogTemp, Log, TEXT("Pass5"));

	PlayerChar->RestrictMovementInput(false);
}


UNewspaperWidget* AGameHUD::GetNewsPaper() const
{
	return NewspaperWidget_Inst;
}