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
