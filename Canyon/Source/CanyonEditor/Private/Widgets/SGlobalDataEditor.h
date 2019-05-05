#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "NotifyHook.h"

class SGlobalDataEditor : public SCompoundWidget, public FNotifyHook
{
public:
	SLATE_BEGIN_ARGS(SGlobalDataEditor)
	{		
	}
	SLATE_END_ARGS()

	virtual ~SGlobalDataEditor() = default;

	void Construct(const struct FArguments &Args);
	
	void OnFinishedChangingProperties(const FPropertyChangedEvent &PropertyChangedEvent);

	virtual void NotifyPreChange(UProperty* PropertyAboutToChange) override;

	virtual void NotifyPreChange(FEditPropertyChain* PropertyAboutToChange) override;

	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged) override;

	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FEditPropertyChain* PropertyThatChanged) override;

	UObject *GetTargetDco() const { return m_pTargetDCO; }


private:
	UObject *m_pTargetDCO;

	FEditPropertyChain *m_pCurrentPropertyChain;

	TMap < FString, TMap<FString, int32> > m_InfluenceMapping;
	TMap < FString, TSoftClassPtr<class UUserWidget> > m_UiMapping;


};
