#include "InfluenceDataObject.h"
#include "ConstructorHelpers.h"
#include "Engine/UserDefinedEnum.h"
#include "Canyon/Public/CanyonHelpers.h"
#include "FileManagerGeneric.h"
#include "BufferArchive.h"
#include "FileHelper.h"
#include "Paths.h"
#include "MemoryReader.h"

//Local custom archive class that can handle soft object paths
class FCustomArchive : public FBufferArchive
{
public:
	virtual FArchive &operator<<(FSoftObjectPath &Path) override
	{		
		Path.SerializePath(*this);

		return *this;


	}


};

FArchive& operator<<(FArchive& Lhs, FInfluenceColData& Rhs)
{
	Lhs << Rhs.m_InfluenceMapping;
	Lhs << Rhs.m_InfluenceRadius;
	
	FSoftObjectPath AsPath{ Rhs.m_UiWidgetClass.ToSoftObjectPath() };
	AsPath.SerializePath(Lhs);
	
	if(Lhs.IsLoading())
	{
		Rhs.m_UiWidgetClass = TSoftClassPtr<UUserWidget>{ AsPath };	
	}

	Lhs << Rhs.m_BasePointRequirement;
		
	return Lhs;


}

FInfluenceColData::FInfluenceColData(const TArray<FString>& aKeyArray) :
	m_InfluenceRadius{ 1 },
	m_UiWidgetClass{ nullptr }
{
	for(auto &&Key :  aKeyArray)
	{
		m_InfluenceMapping.Add(Key, 0);
	}


}

void FInfluenceColData::RemoveInfluenceKey(const FString& Key)
{
	m_InfluenceMapping.Remove(Key);


}

void FInfluenceColData::AddInfluenceKey(const FString& Key)
{
	m_InfluenceMapping.Add(Key, 0);


}

int32& FInfluenceColData::operator[](const FString& Key)
{
	return m_InfluenceMapping[Key];


}


UInfluenceDataObject* UInfluenceDataObject::CreateFromFile()
{
	auto *pObj{ NewObject<UInfluenceDataObject>() };

	pObj->InitializeData();

	return pObj;

	
}

void UInfluenceDataObject::InitializeData()
{	
	//try to load a config from file
	LoadFromFile();

	//synchronize the loaded data with any editor changes
		//load the influence category enum
	TSoftObjectPtr<UUserDefinedEnum> pInfluenceEnumAsset
	{
		FSoftObjectPath{TEXT("UserDefinedEnum'/Game/Placeables/InfluenceTypes.InfluenceTypes'")}
	};		
	auto *pEnum = SafeLoadObjectPtr(pInfluenceEnumAsset);	
	auto EnumCount{ pEnum->NumEnums() };

		//generate the new key set
	TArray<FString> aTemplateKeys{};
	
	for(int32 EnumIndex{ 0 }; EnumIndex < EnumCount; ++EnumIndex)
	{		
		auto Key{ pEnum->GetDisplayNameTextByIndex(EnumIndex).ToString() };

		if(Key == TEXT("None") || Key == TEXT("Influence Types MAX"))
		{
			continue;
		}
		aTemplateKeys.Add(Key);

	}

		//enforce the new key set
	OverwriteKeySet(aTemplateKeys);


}

void UInfluenceDataObject::SaveToFile()
{	
	FCustomArchive Ar{};
	Archive(Ar);
	   
	FFileHelper::SaveArrayToFile(Ar, *(FPaths::ProjectContentDir() + TEXT("Resc/InfluenceData.dat")));


}

void UInfluenceDataObject::LoadFromFile()
{
	TArray<uint8> aBytes;
	FFileHelper::LoadFileToArray(aBytes, *(FPaths::ProjectContentDir() + TEXT("Resc/InfluenceData.dat")));
	
	FMemoryReader Reader{aBytes};
	Reader.Seek(0);

	Archive(Reader);


}

FInfluenceColData& UInfluenceDataObject::operator[](const FString& Key)
{
	return m_InfluenceRowDataMap[Key];


}


//Private---------------------

void UInfluenceDataObject::Archive(FArchive& Archive)
{
	Archive << m_InfluenceRowDataMap;


}

void UInfluenceDataObject::OverwriteKeySet(const TArray<FString> &aNewKeyTemplate)
{
	//keys from the deserialized data that are currently not found
	//in the template key set
	TArray<FString> aUnfoundCurrentKeys;
	m_InfluenceRowDataMap.GenerateKeyArray(aUnfoundCurrentKeys);
	
	//Make sure every key from the template is present
	for(auto &&TemplateKey : aNewKeyTemplate)
	{
		auto *pElem{ m_InfluenceRowDataMap.Find(TemplateKey) };
		
		//add nonexistent entries...
		if(!pElem)
		{
			//...to the matrix column
			for(auto &&RowPair : m_InfluenceRowDataMap)
			{
				//row value == column data
				RowPair.Value.AddInfluenceKey(TemplateKey);

			}

			//...to the matrix rows
			m_InfluenceRowDataMap.Add(TemplateKey, FInfluenceColData{aNewKeyTemplate} );
		}

		//remove found keys so we can detect "deprecated" enums later
		aUnfoundCurrentKeys.RemoveAll([&KeyToRemove = TemplateKey](const FString &Other)
		{
			return Other == KeyToRemove;


		});

	}

	//remove keys from the objects map that did not occur in the template map, ie the editor enum
	for(auto &&DeprecatedKey : aUnfoundCurrentKeys)
	{
		m_InfluenceRowDataMap.Remove(DeprecatedKey);

		for(auto &&Pair : m_InfluenceRowDataMap)
		{
			Pair.Value.m_InfluenceMapping.Remove(DeprecatedKey);

		}

	}


}
