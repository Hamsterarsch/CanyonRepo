#pragma once

#include "CoreMinimal.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"

#pragma region ComponentSearchDCO
template<class ComponentClass = UActorComponent>
TArray<const USCS_Node *> GetScsDataNodesForType(const UClass *pClassToSearch)
{
	return GetScsDataNodesForType<ComponentClass>(pClassToSearch, ComponentClass::StaticClass());


}

template
<
	class SearchedClass = AActor,
	class ComponentClass = UActorComponent,
	UClass *pClassToSearch = SearchedClass::StaticClass(),
	UClass *pCompClass = ComponentClass::StaticClass() 
>
TArray<const USCS_Node *> GetScsDataNodesForType()
{
	return GetScsDataNodesForType<ComponentClass>(pClassToSearch, pCompClass);

	
}

template<class ComponentClass = UActorComponent>
TArray<const USCS_Node *> GetScsDataNodesForType(const UClass *pClassToSearch, const UClass *pCompClass)
{
	if (!pClassToSearch || !pCompClass)
	{
		return {};
	}

	auto *pBpClass{ Cast<UBlueprintGeneratedClass>(pClassToSearch) };
	auto aNodes{ pBpClass->SimpleConstructionScript->GetAllNodesConst() };

	TArray<const USCS_Node *> aFoundNodes{};
	for (auto &&pNode : aNodes)
	{
		if (pNode->ComponentTemplate->IsA<ComponentClass>())
		{
			aFoundNodes.Emplace(pNode);
		}
		
	}
	
	return aFoundNodes;


}

inline void GetScsDataNodesForType(...)
{
	static_assert(true, "Component class is not derived from uobject.");
}
#pragma endregion

inline int32 GetRandomIndex(int32 ArrSize)
{
	return FMath::RoundToInt((static_cast<float>(FMath::Rand()) / RAND_MAX) * (ArrSize - 1));


}

inline int32 GetRandomIndexSeeded(int32 ArrSize)
{
	//in constrast to FMath::Rand, FMath::SRand returns results in the range from 0 - 0.999999...
	return FMath::RoundToInt(FMath::SRand() * (ArrSize - 1));


}



template<class T>
UClass *SafeLoadClassPtr(const TSoftClassPtr<T> &ClassPtr)
{
	UClass *pClass = nullptr;
	if ((pClass = ClassPtr.Get()) == nullptr)
	{
		pClass = ClassPtr.LoadSynchronous();

	}

	return ensureMsgf(pClass, TEXT("Could not load class ptr from soft obj ptr")) ? pClass : nullptr;


}

template<class T>
T *SafeLoadObjectPtr(const TSoftObjectPtr<T> &ObjectPtr)
{
	UObject *pObject = nullptr;
	if ((pObject = ObjectPtr.Get()) == nullptr)
	{
		pObject = ObjectPtr.LoadSynchronous();

	}

	return ensureMsgf(pObject, TEXT("Could not load class ptr from soft obj ptr")) ? Cast<T>(pObject) : nullptr;


}

