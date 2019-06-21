#pragma once
#include "CoreMinimal.h"
#include "CategoryData.generated.h"

USTRUCT()
struct FCategoryData
{
	GENERATED_BODY()

public:
	FCategoryData() = default;

	FCategoryData(const FString &CategoryName, int32 ChargeCount = 0)
	{		 
		m_CategoryName = CategoryName;
		m_ChargeCount = ChargeCount;


	}

	FString m_CategoryName;
	int32 m_ChargeCount;


};
