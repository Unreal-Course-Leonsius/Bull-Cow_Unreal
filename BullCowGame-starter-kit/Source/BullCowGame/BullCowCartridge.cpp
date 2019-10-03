// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
#include "Map.h"
#include "Console/Terminal.h"
#include "HiddenWordList.h"


void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();

    SetupGame();

}

void UBullCowCartridge::SetupGame()
{
    // Welcoming The Player
    PrintLine(TEXT("Welcome to Bull Cows!"));
    
    HiddenWord = Word::GetIsogramWord();
    Lives = HiddenWord.Len();
    bGameOver = false;
    //UE_LOG(LogTemp, Warning, TEXT("HiddenWord = %s"), *HiddenWord);
    
    PrintLine(TEXT("Guess the %i letter word!"), HiddenWord.Len());
    PrintLine(TEXT("You have %i lives."), Lives);
    PrintLine(TEXT("Type in your guess and \npress enter to continue...")); // Prompt Player For Guess
}

void UBullCowCartridge::OnInput(const FString& Input) // When the player hits enter
{ 
    if (bGameOver)
    {
        ClearScreen();
        SetupGame();
    }
    else // Checking PlayerGuess
    {
        ClearScreen();
        ProcessGuess(Input);
    }
}

void UBullCowCartridge::ProcessGuess(FString Guess)
{
    if (Guess == HiddenWord)
    {
        PrintLine(TEXT("You have Won!"));
        EndGame();
        return;
    }

    if (Lives <= 0)
    {
        ClearScreen();
        PrintLine(TEXT("You have no lives left!"));
        PrintLine(TEXT("The hidden word was: %s"), *HiddenWord);
        EndGame();
        return;
    }

    EGuessStatus Status = CheckGuessValidity(Guess);
    FBullCowCount BullsCows;
    switch(Status)
    {
        case EGuessStatus::Wrong_Length :
            PrintLine(TEXT("Word lenght is not correct.\nHidden Word length is %i"), HiddenWord.Len());
            PrintLine(TEXT("Lost a life!"));
            --Lives;
            PrintLine(TEXT("Guess again, you have %i lives left"), Lives);
            break;
        case EGuessStatus::NOT_Isogram :
            PrintLine(TEXT("Word is not Isogram.\nIsogram is a word that\ndon't contain the same letters"));
            PrintLine(TEXT("Lost a life!"));
            --Lives;
            PrintLine(TEXT("Guess again, you have %i lives left"), Lives);
            break;
        case EGuessStatus::NOT_Lowercase :
            PrintLine(TEXT("Word must be include lower letters"));
            PrintLine(TEXT("Lost a life!"));
            --Lives;
            PrintLine(TEXT("Guess again, you have %i lives left"), Lives);
            break;
        case EGuessStatus::OK :
            BullsCows = CountBullsCows(Guess);
            PrintLine(TEXT("Your Bulls: %i\nYour Cows: %i"), BullsCows.Bulls, BullsCows.Cows);
            PrintLine(TEXT("Lost a life!"));
            --Lives;
            PrintLine(TEXT("Guess again, you have %i lives left"), Lives);
            break;
        default :
            break;
    }

    return;
}

/// Check Guess Word Validity
EGuessStatus UBullCowCartridge::CheckGuessValidity(FString Guess)
{
    UE_LOG(LogTemp, Warning, TEXT("CheckGuessValidity -- LengthCheck"));
    if(Guess.Len() != HiddenWord.Len())
    {
        return EGuessStatus::Wrong_Length;
    }

    if(!IsLowerCase(Guess))
    {
        return EGuessStatus::NOT_Lowercase;
    }

    UE_LOG(LogTemp, Warning, TEXT("CheckGuessValidity -- IsogramCheck"));
    if(!IsIsogram(Guess))
    {
        return EGuessStatus::NOT_Isogram;
    }

    return EGuessStatus::OK;
}


bool UBullCowCartridge::IsIsogram(FString Guess) const
{
    if(Guess.Len() <= 1) return true;

    TSet<FString>* LetterSeen = GetTerminal()->GetWordMap();
    if(LetterSeen->Num() != Guess.Len()) return false;
    
    return true;
}

bool UBullCowCartridge::IsLowerCase(FString Guess) const
{
    for (auto letter : Guess)
    {
		if (letter != tolower(letter)) return false;
	}
	return true;
}

FBullCowCount UBullCowCartridge::CountBullsCows(FString Guess)
{
    
	int32 WordLength = HiddenWord.Len(); // assuming same length as guess
	FBullCowCount BullCowCount;
	for (int32 MHVChar = 0; MHVChar < WordLength; MHVChar++)
    {
		// compare letters against the hidden word

		for (int32 GChar = 0; GChar < WordLength; GChar++)
        {
			// if they match then
			if (Guess[GChar] == HiddenWord[MHVChar])
            {
				if (MHVChar == GChar) BullCowCount.Bulls++; // if they're in the same place // incriment bulls
				else BullCowCount.Cows++; // must be a cow

			}

		}
	}

    return BullCowCount;
}


void UBullCowCartridge::EndGame()
{
    bGameOver = true;
    PrintLine(TEXT("\nPress enter to play again."));
}