#pragma once

#include "CoreMinimal.h"
#include "UserWidget.h"
#include "GlobalDataEditorMetadata.generated.h"

USTRUCT()
struct FInfluenceMetadata
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		TMap<FString, int32> m_InfluenceMapping;

	UPROPERTY(EditAnywhere)
		float m_InfluenceRadius;

	UPROPERTY(EditAnywhere)
		TSoftClassPtr<UUserWidget> m_UiWidgetClass;

};

UCLASS()
class UGlobalDataEditorMetadata : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
		TMap < FString, FInfluenceMetadata > m_InfluenceMapping;
	

};

