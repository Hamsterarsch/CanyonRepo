#include "Savegame/SavegameHandler.h"
#include "BufferArchive.h"
#include "MemoryReader.h"
#include "Savegame/NamedBinaryFormatter.h"
#include "FileHelper.h"
#include "Paths.h"

void USavegameHandler::SaveGame(const bool bIsAutosave, const int32 SlotNumber) const
{
	return;
	auto SavegameFilename{ BuildSavegameFilepath(bIsAutosave, SlotNumber) };


	FBufferArchive WriterArchive{ true };
	
	{
		Canyon::CNamedBinaryFormatter WriterFormatter{ WriterArchive };

		int32 Major{ 0 };
		int32 Minor{ 1 };
		WriterFormatter.AddField(TEXT("MajorVersion"), Major);
		WriterFormatter.AddField(TEXT("MinorVersion"), Minor);

		WriterFormatter.EnterScope(TEXT("LevelData"));

		TArray<FVector> aPos{ {1,0,0}, {0,1,0}, {0,0,1} };
		WriterFormatter.AddField(TEXT("Positions"), aPos);
	}

	SaveToFile(WriterArchive, SavegameFilename);

	LoadNewestSavegame(false, 0);
	



	   
}

void USavegameHandler::LoadNewestSavegame(const bool bIncludeAutosaves, const int32 SlotNumber) const
{
	FString SavegameFilepath{};
	GetNewestSavegameFilepathForSlot(SavegameFilepath, bIncludeAutosaves, SlotNumber);

	TArray<uint8> aLoadedData;
	FFileHelper::LoadFileToArray(aLoadedData, *SavegameFilepath);

	FMemoryReader ReaderArchive{ aLoadedData, true };
	Canyon::CNamedBinaryFormatter ReaderFormatter{ ReaderArchive };
			
	auto Minor{ ReaderFormatter.ReadField<int32>(TEXT("MinorVersion")) };
	auto Major{ ReaderFormatter.ReadField<int32>(TEXT("MajorVersion")) };
	auto aPos{ ReaderFormatter.ReadField<TArray<FVector>>(TEXT("LevelData.Positions")) };

	int32 Breaker{ 2 };

}

#pragma region PathGeneration

FString USavegameHandler::BuildSavegameFilepath(const bool bIsAutosave, const int32 SlotNumber) const
{
	auto SavegamePath{ GetSavegameBasePath() };

	AddSavegameSlotToPath(SavegamePath, SlotNumber);

	const auto SavegameName{ GenerateSavegameName(bIsAutosave) };

	SavegamePath.PathAppend(*SavegameName, SavegameName.Len());

	AddSavegameFileExtensionToPath(SavegamePath);

	return SavegamePath;


}

FString USavegameHandler::GetSavegameBasePath() const
{
	FString OutPath{ FPlatformProcess::UserDir() };
	
	OutPath.PathAppend(*m_SavegamePathCustomPart, m_SavegamePathCustomPart.Len());
	
	FString SavePart{ TEXT("Saves") };
	OutPath.PathAppend(*SavePart, SavePart.Len());

	return OutPath;


}

void USavegameHandler::AddSavegameFileExtensionToPath(FString& Path) const
{
	Path.Append(".");
	Path.Append(m_SavegameExtension);


}

bool USavegameHandler::GetNewestSavegameFilepathForSlot(FString& out_Path, const bool bIncludeAutosaves, const int32 SlotNumber) const
{
	auto &FileManager{ IFileManager::Get() };

	auto SavegameSlotPath{ GetSavegameBasePath() };
	AddSavegameSlotToPath(SavegameSlotPath, SlotNumber);
	
	TArray<FString> aFoundSavegameFiles;
	FileManager.FindFiles(aFoundSavegameFiles, *SavegameSlotPath, *m_SavegameExtension);

	if(aFoundSavegameFiles.Num() == 0)
	{
		return false;
	}
		
	TArray<TPair<FDateTime, int32>> aTimestampFileindex{};

	for(int32 SavegameFileIndex{ 0 }; SavegameFileIndex < aFoundSavegameFiles.Num(); ++SavegameFileIndex)
	{
		//don't include autosaves if not wanted
		auto Filepath{  SavegameSlotPath + '/' + aFoundSavegameFiles[SavegameFileIndex] };
				
		const auto SubstrIndex{ Filepath.Find(*m_AutosaveToken, ESearchCase::CaseSensitive, ESearchDir::FromEnd) };
		const auto LastFolderTokenIndex{ Filepath.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd) };

		//if Autosave token occurs in the filename and not in the folder name
		if(SubstrIndex > LastFolderTokenIndex && !bIncludeAutosaves)
		{
			continue;
		}

		aTimestampFileindex.Add
		(
			TPair<FDateTime, int32>
			{ 
				FileManager.GetTimeStamp(*Filepath),
				SavegameFileIndex 
			}
		);

	}

	aTimestampFileindex.Sort([](const TPair<FDateTime, int32> &Left, const TPair<FDateTime, int32> &Right)
	{
		return Left.Key > Right.Key;
	});

	out_Path = SavegameSlotPath + '/' + aFoundSavegameFiles[ aTimestampFileindex[0].Value ];
	return true;


}

FString USavegameHandler::GenerateSavegameName(const bool bIsAutoSave) const
{
	FString OutName{};

	if(bIsAutoSave)
	{
		OutName.Append(m_AutosaveToken);
	}

	OutName.Append(FDateTime::UtcNow().ToString());
	OutName.ReplaceCharInline('.', '_');
	OutName.ReplaceCharInline('-', '_');
	
	return OutName;


}


//statics----------------------------

void USavegameHandler::AddSavegameSlotToPath(FString &Path, int32 SavegameSlot)
{
	auto SlotPart{ FString::FromInt(SavegameSlot) };
	Path.PathAppend(*SlotPart, SlotPart.Len());


}

#pragma endregion

bool USavegameHandler::SaveToFile(const TArray<uint8>& Data, const FString& Path)
{
	return FFileHelper::SaveArrayToFile(Data, *Path, &IFileManager::Get(), EFileWrite::FILEWRITE_NoReplaceExisting);
	
	
}
