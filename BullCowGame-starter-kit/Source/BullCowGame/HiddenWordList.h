#include "CoreMinimal.h"
#include "Map.h"

const TArray<FString> HiddenWords = 
{
    TEXT("attention"),
    TEXT("attorney"),
    TEXT("audience"),
    TEXT("author"),
    TEXT("authority"),
    TEXT("available"),
    TEXT("avoid"),
    TEXT("beat"),
    TEXT("beautiful"),
    TEXT("because"),
    TEXT("become"),
    TEXT("bed"),
    TEXT("before"),
    TEXT("begin"),
    TEXT("behavior"),
    TEXT("behind"),
    TEXT("believe"),
    TEXT("benefit"),
    TEXT("best"),
    TEXT("better"),
    TEXT("between"),
    TEXT("beyond"),
    TEXT("center"),
    TEXT("central"),
    TEXT("century"),
    TEXT("certain"),
    TEXT("certainly"),
    TEXT("chair"),
    TEXT("chance"),
    TEXT("change"),
    TEXT("charge"),
    TEXT("check"),
    TEXT("child"),
    TEXT("choice"),
    TEXT("choose"),
    TEXT("commercial"),
    TEXT("common"),
    TEXT("community"),
    TEXT("company"),
    TEXT("compare"),
    TEXT("computer"),
    TEXT("concern"),
    TEXT("condition"),
    TEXT("conference"),
    TEXT("great"),
    TEXT("ground"),
    TEXT("group"),
    TEXT("grow"),
    TEXT("growth")
};

class Word
{
    public:

    static FString GetIsogramWord()
    {
        bool success;
        FString Word;
        TSet<TCHAR> Isogram;
        do
        {
            
            int32 WordIndex = FMath::RandRange(0, HiddenWords.Num()-1);
          
            Word = HiddenWords[WordIndex];
            success = true;

            for(auto Letter : Word)
            {
                Isogram.Add(Letter);
            }

            if(Isogram.Num() == Word.Len())
            {
                success = true;
            }
            else success = false;

            Isogram.Empty();
            
        } while (!success);

        return Word;
        
    }


};