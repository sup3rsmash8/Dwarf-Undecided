// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.
// AUTHOR: Justus Hörberg

#include "UI/InfoNote/InfoNoteData.h"

FInfoNoteData::FInfoNoteData()
{
	NoteText = FText::GetEmpty();
}

FInfoNoteData::FInfoNoteData(FText Text)
{
	NoteText = Text;
}
