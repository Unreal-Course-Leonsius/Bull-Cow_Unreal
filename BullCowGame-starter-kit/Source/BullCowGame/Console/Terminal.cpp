// Fill out your copyright notice in the Description page of Project Settings.


#include "Terminal.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/Application/SlateApplication.h"

#include "Cartridge.h"

constexpr TCHAR GPrompt[4] = TEXT("$> ");

// Called when the game starts
void UTerminal::BeginPlay()
{
	Super::BeginPlay();
	UpdateText();
}

void UTerminal::ActivateTerminal()
{
	// If Press any Key calls Delegate function
    FInputKeyBinding PressedBinding(EKeys::AnyKey, EInputEvent::IE_Pressed);
    PressedBinding.KeyDelegate.BindDelegate(this, &UTerminal::OnKeyDown);

    FInputKeyBinding RepeatBinding(EKeys::AnyKey, EInputEvent::IE_Repeat);
    RepeatBinding.KeyDelegate.BindDelegate(this, &UTerminal::OnKeyDown);

	if (GetOwner()->InputComponent == nullptr) return;

	// Put this PressedBinding into Owner's InputComponent <-(which may be belongs to PlayerController)
    PressedBindingIndex = GetOwner()->InputComponent->KeyBindings.Emplace(MoveTemp(PressedBinding));
    RepeatBindingIndex = GetOwner()->InputComponent->KeyBindings.Emplace(MoveTemp(RepeatBinding));
}

void UTerminal::DeactivateTerminal() const
{
	if (GetOwner()->InputComponent == nullptr) return;
	
	// Must do in this order as RepeatBindingIndex > PressedBindingIndex so would change when first is removed
	GetOwner()->InputComponent->KeyBindings.RemoveAt(RepeatBindingIndex);
	GetOwner()->InputComponent->KeyBindings.RemoveAt(PressedBindingIndex);
}

void UTerminal::PrintLine(const FString& Line)
{
	FString Input = Line;
	FString Left, Right;

	// Split the Input String if it include '\n' symbol and split it on that symbol
	while (Input.Split(TEXT("\n"), &Left, &Right))
	{
		UE_LOG(LogTemp, Warning, TEXT("Split is true"));
		Buffer.Emplace(Left); // Left from '\n' symbol
		Input = Right;        // Right from '\n' symbol
	} 

	Buffer.Emplace(Input); // put Right part into Buffer massive
	UpdateText();
}

void UTerminal::ClearScreen()
{
	Buffer.Empty();
	UpdateText();
}

FString UTerminal::GetScreenText() const
{
	TArray<FString> FullTerminal = Buffer;
	FullTerminal.Add(GPrompt + InputLine);

	// WrapLines
	TArray<FString> WrappedLines(WrapLines(FullTerminal));
	Truncate(WrappedLines);

	return JoinWithNewline(WrappedLines);
}

TArray<FString> UTerminal::WrapLines(const TArray<FString>& Lines) const
{
	//UE_LOG(LogTemp, Warning,TEXT("WrapLines Line Length = %i"), Lines.Num());
	TArray<FString> WrappedLines;
	for (auto &&Line : Lines)
	{

		FString CurrentLine = Line;
		//UE_LOG(LogTemp, Warning,TEXT("CurrentLine = %s"), *CurrentLine);
		do
		{
			WrappedLines.Add(CurrentLine.Left(MaxColumns));
			CurrentLine = CurrentLine.RightChop(MaxColumns);
		}
		while (CurrentLine.Len() > 0);
	}
	return WrappedLines;
}

void UTerminal::Truncate(TArray<FString>& Lines) const
{
	while (Lines.Num() > MaxLines)
	{
		Lines.RemoveAt(0);
	}
}

FString UTerminal::JoinWithNewline(const TArray<FString>& Lines) const
{
	// Convert String massive to one String
	FString Result;
	for (auto &&Line : Lines)
	{
		// When press Enter key here comes two Elements 1) string 2) "$> " + InputLine.empty
		// foreach loop split it two parts and both of them add "<br>" outcome this we get
		// 1 string +"<br>" = Log string and breakdown next line
		// 2 "$> " + InputLine.empty = New line start with $> and we can put other word 
		Result += Line + TEXT(" <br>");
	}

	return Result;
}

void UTerminal::OnKeyDown(FKey Key)
{
	if (Key == EKeys::Enter)
	{
		AcceptInputLine();
	}

	if (Key == EKeys::BackSpace)
	{
		Backspace();
	}

    const FString KeyString = GetKeyString(Key);
    const FModifierKeysState KeyState = FSlateApplication::Get().GetModifierKeys();
	if (KeyState.IsShiftDown() || KeyState.AreCapsLocked())
	{
		InputLine += KeyString.ToUpper();
		if(KeyString != "")
		{
			FillWordMap(KeyString.ToUpper());
		}
	}
	else
	{
		InputLine += KeyString.ToLower();
		if(KeyString != "")
		{
			FillWordMap(KeyString.ToLower());
		}
		
		//WordMap.Add(charaqter, false);
	}


	UpdateText();
}


void UTerminal::AcceptInputLine()
{
	Buffer.Emplace(GPrompt + InputLine);
	auto Cartridge = GetOwner()->FindComponentByClass<UCartridge>();
	if (Cartridge != nullptr)
	{
		Cartridge->OnInput(InputLine);
	}
	InputLine = TEXT("");
	WordMap.Empty();
}

void UTerminal::Backspace()
{
	if (InputLine.Len() > 0)
	{
		InputLine.RemoveAt(InputLine.Len()-1);
	}
}

FString UTerminal::GetKeyString(FKey Key) const
{
	const uint32* KeyCode = nullptr;
	const uint32* CharCode = nullptr;
	FInputKeyManager::Get().GetCodesFromKey(Key, KeyCode, CharCode);
	if (CharCode != nullptr)
	{
		ANSICHAR Char[2] = {static_cast<ANSICHAR>(*CharCode), '\0'};
		return ANSI_TO_TCHAR(Char);
	}

	return TEXT("");
}

void UTerminal::UpdateText()
{
	TextUpdated.Broadcast(GetScreenText());
}

TSet<FString>* UTerminal::GetWordMap()
{
	return &WordMap;
}

void UTerminal::FillWordMap(FString Letter)
{
	WordMap.Add(Letter);
	//UE_LOG(LogTemp,Warning, TEXT("WordMap size = %i"), WordMap.Num());
}