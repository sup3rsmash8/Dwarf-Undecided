// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.


#include "UI/Newspaper/ArticleData.h"

FText FArticleData::GetHeadLine() const
{
	int Rand = FMath::RandRange(0, Headlines.Num() - 1);
	//FStringFormatArg Argument = FString::Format(ToCStr(Headlines[Rand]), CharacterInfo);
	//return FText::FromString(Argument.StringValue);
	return FText::FromString(FString::Format(ToCStr(Headlines[Rand]), CharacterInfo));
}

FText FArticleData::GetComment() const
{
	int Rand = FMath::RandRange(0, Comments.Num() - 1);
	//FStringFormatArg Argument = FString::Format(ToCStr(Comments[Rand]), CharacterInfo);
	return FText::FromString(FString::Format(ToCStr(Comments[Rand]), CharacterInfo));
}

FText FArticleData::GetBody() const
{
	int Rand = FMath::RandRange(0, BodyTexts.Num() - 1);
	//FStringFormatArg Argument = FString::Format(ToCStr(BodyTexts[Rand]), CharacterInfo);
	//return FText::FromString(Argument.StringValue);
	return FText::FromString(FString::Format(ToCStr(BodyTexts[Rand]), CharacterInfo));
}

UTexture2D* FArticleData::GetPhoto() const
{
	int Rand = FMath::RandRange(0, NewsPhotos.Num() - 1);
	return NewsPhotos[Rand];
}

void FArticleData::AssignCharacterInfo(TArray<FStringFormatArg> Info)
{
	CharacterInfo = Info;
}