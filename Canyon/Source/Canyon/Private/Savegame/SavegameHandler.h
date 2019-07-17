#pragma once
#include "CoreMinimal.h"
#include "UObject.h"
#include "SavegameHandler.generated.h"

UCLASS()
class USavegameHandler : public UObject
{
	GENERATED_BODY()

public:
	void SaveGame(bool bIsAutosave = false, int32 SlotNumber = 0) const;

	void LoadNewestSavegame(bool bIncludeAutosaves = true, int32 SlotNumber = 0) const;


private:
	FString BuildSavegameFilepath(bool bIsAutosave, int32 SlotNumber) const;

	FString GetSavegameBasePath() const;

	void AddSavegameFileExtensionToPath(FString &Path) const;

	bool GetNewestSavegameFilepathForSlot(FString &out_Path, bool bIncludeAutosaves, int32 SlotNumber = 0) const;

	FString GenerateSavegameName(bool bIsAutoSave) const;


	static bool SaveToFile(const TArray<uint8> &Data, const FString &Path);

	static void AddSavegameSlotToPath(FString &Path, int32 SavegameSlot);

	


	FString m_SavegamePathCustomPart{ TEXT("Studio403/Helios/") };
	FString m_SavegameExtension{ TEXT("nbbsav") };
	FString m_AutosaveToken{ TEXT("auto") };

};

