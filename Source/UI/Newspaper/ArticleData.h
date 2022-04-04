// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/StringFormatArg.h"
#include "ArticleData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FArticleData
{
	GENERATED_BODY()
public:
	/* The headline to display in the article. */
		//UPROPERTY(EditAnywhere, meta = (MultiLine = "true"))
		//FText ArticleHeadline = FText::GetEmpty();

	/* Headline quote from article subject. */
	//UPROPERTY(EditAnywhere, meta = (MultiLine = "true"))
	//	FText ArticleComment = FText::GetEmpty();

	/* The main text in the article. */
	//UPROPERTY(EditAnywhere, meta = (MultiLine = "true"))
	//	FText ArticleStory = FText::GetEmpty();

private:
	UPROPERTY(EditAnywhere, meta = (MultiLine = "true"))
	TArray<FString> Headlines;
	UPROPERTY(EditAnywhere, meta = (MultiLine = "true"))
	TArray<FString> Comments;
	UPROPERTY(EditAnywhere, meta = (MultiLine = "true"))
	TArray<FString> BodyTexts;
	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> NewsPhotos;

	TArray<FStringFormatArg> CharacterInfo;

public:
	FText GetHeadLine() const;
	FText GetComment() const;
	FText GetBody() const;
	UTexture2D* GetPhoto() const;
	void AssignCharacterInfo(TArray<FStringFormatArg> Info);

	/* If the article should feature a photo, include it in here. Leave null (blank) if not. */
	//UPROPERTY(EditAnywhere)
	//	UTexture2D* ArticlePhoto = nullptr;
};