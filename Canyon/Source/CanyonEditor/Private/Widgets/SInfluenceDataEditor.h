#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "NotifyHook.h"

class SInfluenceDataEditor : public SCompoundWidget, public FNotifyHook
{
public:
	SLATE_BEGIN_ARGS(SInfluenceDataEditor)
	{		
	}
	SLATE_END_ARGS()

	virtual ~SInfluenceDataEditor() = default;

	void Construct(const struct FArguments &Args);
	
	void RefreshObjectData();
	
	virtual void NotifyPreChange(UProperty* PropertyAboutToChange) override;

	virtual void NotifyPreChange(FEditPropertyChain* PropertyAboutToChange) override;

	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged) override;

	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FEditPropertyChain* PropertyThatChanged) override;
	

private:
	class UInfluenceDataObject *m_pEditorDataObject;

	FEditPropertyChain *m_pCurrentPropertyChain;

	   
};
