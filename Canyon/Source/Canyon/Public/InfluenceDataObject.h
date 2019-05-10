#pragma once

#include "CoreMinimal.h"
#include "UserWidget.h"
#include "InfluenceDataObject.generated.h"

USTRUCT()
struct CANYON_API FInfluenceColData
{
	GENERATED_BODY()

public:
	FInfluenceColData() = default;

	FInfluenceColData(const TArray<FString> &aKeyArray);

	void RemoveInfluenceKey(const FString &Key);

	void AddInfluenceKey(const FString &Key);

	//returns the influence associated with the key
	int32 &operator[](const FString &Key);


	UPROPERTY(EditAnywhere)
		TMap<FString, int32> m_InfluenceMapping;

	UPROPERTY(EditAnywhere)
		float m_InfluenceRadius;

	UPROPERTY(EditAnywhere)
		TSoftClassPtr<UUserWidget> m_UiWidgetClass;
		

};

//Influence data serialization (required as global)
FArchive& operator<<(FArchive& Lhs, FInfluenceColData& Rhs);


UCLASS()
class CANYON_API UInfluenceDataObject : public UObject
{
	GENERATED_BODY()

public:
	static UInfluenceDataObject *CreateFromFile();

	//Loads saved data from file and merges it with any new editor changes
	void InitializeData();
	   
	//saves data member content to file
	void SaveToFile();

	//loads data member content from file
	void LoadFromFile();

	//returns the column data associated with the key
	FInfluenceColData &operator[](const FString &Key);


	using InfluenceGroupDataType = TMap < FString, FInfluenceColData >;

	UPROPERTY(EditDefaultsOnly)
		TMap < FString, FInfluenceColData > m_InfluenceRowDataMap;
	

private:
	//Member serialization
	void Archive(FArchive &Archive);

	//Updates the matrix rows and columns to match the input template
	void OverwriteKeySet(const TArray<FString> &aNewKeyTemplate);

	
};

