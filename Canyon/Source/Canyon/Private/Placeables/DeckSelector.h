#pragma once
#include "CoreMinimal.h"
#include <set>
#include "WidgetBase/PrettyWidget.h"
#include "DeckSelector.generated.h"

USTRUCT()
struct FDeckData
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TMap<FString, int32> m_ChargeMapping;

	UPROPERTY()
		TSubclassOf<UPrettyWidget> m_DeckWidgetClass;


};

USTRUCT()
struct FDeckPathWithMetadata
{
	GENERATED_BODY()

public:
	//uht cant deal with uniform member init list in headers/for structs (?)
	FDeckPathWithMetadata() :
		Path(),
		RelativeProb( 1 ),
		ReqGeneration( -1 )
	{		
	}
		

	FDeckPathWithMetadata(FSoftObjectPath &&mov_Path, int32 RelativeProbability, int32 RequiredGeneration) :
		Path( std::move(mov_Path) ),
		RelativeProb( RelativeProbability ),
		ReqGeneration( RequiredGeneration )
	{		
	}

	UPROPERTY()
		FSoftObjectPath Path;

	UPROPERTY()
		int32 RelativeProb;

	UPROPERTY()
		int32 ReqGeneration;
	

};

UCLASS(Blueprintable, BlueprintType)
class UDeckSelector : public UObject
{
	GENERATED_BODY()

public:
	static UDeckSelector *Construct(UClass *pClass = nullptr);


	UDeckSelector();

	TArray<FDeckData> GetDeckData(int32 ForAmount = 2);

	//Generates deck data without a deck template
	//and filler charges only. Will be empty if no filler charges are permitted.
	FDeckData GetEndlessDeckData();

	inline int32 GetDeckGeneration() const { return m_DeckGeneration; }

	inline void IncreaseDeckGeneration() { ++m_DeckGeneration; }
	

protected:
	UPROPERTY(EditAnywhere)
		class UInfluenceFloatMapDAL *m_pPlaceableMaxPerMap;

	UPROPERTY(EditAnywhere)
		class UInfluenceFloatMapDAL *m_pFillerChargesProbForEndless;
	
	UPROPERTY(EditAnywhere)
		class UCurveFloat *m_pFillerBuildingAmountSource;


	TSet<int32, DefaultKeyFuncs<int32, true>> m_EndlessFillerProbSampleSet;


private:
	void Init();

	void AddFillerChargesToDeckData(int32 FillerChargeCount, FDeckData &DeckData, class UDeckDatabaseNative *pDeckTemplate = nullptr);

	void AddFillerChargesForEndless(int32 FillerChargeCount, FDeckData &DeckData);

	void AddFillerChargesForDeck(int32 FillerChargeCount, FDeckData &DeckData, class UDeckDatabaseNative *pDeckTemplate);

	void AddFillerCharges
	(
		int32 FillerChargeCount, 
		FDeckData &DeckData, 
		const TArray<FString> &aFillerCat, 
		const TArray<int32> &aFillerProbs, 
		TSet<int32, DefaultKeyFuncs<int32, true>> &FillerProbSampleSet
	);

	//Regenerates the sample set used to choose an index from
	//the valid decks array.
	void RegenerateValidDeckData();

	FDeckData GetDeckDataFromValidDeckAt(int32 Index);

	void AddToIssuedChargesForCategory(const FString &Category, int32 ChargeCount);
	
	TArray<TTuple<FSoftObjectPath, class UDeckDatabaseNative *>> FetchAllDecks() const;
	

	UPROPERTY()
		TArray<FDeckPathWithMetadata> m_aDecksValid;
		
	TSet<int32, DefaultKeyFuncs<int32, true>> m_ValidDeckIndexSampleSourceSet;
	
	UPROPERTY()
		TArray<FDeckPathWithMetadata> m_aDecksInvalid;

	UPROPERTY()
		TMap<FString, int32> m_IssuedChargesMap;
	
	UPROPERTY()
		int32 m_DeckGeneration;

	//cache
	TArray<FString> m_aEndlessFillerCats;

	TArray<int32> m_aEndlessFillerProbs;


};