#include "Widgets/SGlobalDataEditor.h"
#include "ModuleManager.h"
#include "PropertyEditorModule.h"
#include "GlobalDataEditorMetadata.h"
#include "Canyon/Private/Misc/CanyonGlobalData.h"

void SGlobalDataEditor::Construct(const FArguments& Args)
{
	auto &PropertyModule{ FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor")) };
			
	m_pTargetDataObject = NewObject<UGlobalDataEditorMetadata>();
	if(!m_pTargetDataObject)
	{
		return;
	}	
	
	FDetailsViewArgs ViewArgs
	{
		false,
		false,
		true,
		FDetailsViewArgs::ActorsUseNameArea,
		false,
		this,
		false,
		TEXT("GlobalDataEditorView")
	};
	auto DetailView{ PropertyModule.CreateDetailView(ViewArgs) };


	DetailView->SetObject(m_pTargetDataObject, false);
	ChildSlot[DetailView];

	
}

void SGlobalDataEditor::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
}

void SGlobalDataEditor::NotifyPreChange(UProperty* PropertyAboutToChange)
{
	if(!m_pTargetDataObject)
	{
		return;
	}

	m_pTargetDataObject->PreEditChange(PropertyAboutToChange);
	

}

void SGlobalDataEditor::NotifyPreChange(FEditPropertyChain* PropertyAboutToChange)
{
	m_pCurrentPropertyChain = PropertyAboutToChange;

	if (!m_pTargetDataObject)
	{
		return;
	}

	m_pTargetDataObject->PreEditChange(PropertyAboutToChange->GetActiveNode()->GetValue());


}

void SGlobalDataEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,	UProperty* PropertyThatChanged)
{
	if (!m_pTargetDataObject)
	{
		return;
	}

	if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		FPropertyChangedEvent ChangedEvent{ PropertyChangedEvent };

		if (PropertyThatChanged->IsA<UStructProperty>())
		{
			FPropertyChangedChainEvent Event{ *m_pCurrentPropertyChain, ChangedEvent };
			m_pTargetDataObject->PostEditChangeChainProperty(Event);

			return;
		}		
		m_pTargetDataObject->PostEditChangeProperty(ChangedEvent);		
	}


}


void SGlobalDataEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,	FEditPropertyChain* PropertyThatChanged)
{
	if (!m_pTargetDataObject)
	{
		return;
	}

	if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		FPropertyChangedEvent ChangedEvent{ PropertyChangedEvent };
		auto *pChangedProperty{ PropertyThatChanged->GetActiveNode()->GetValue() };

		if (pChangedProperty->IsA<UStructProperty>())
		{
			FPropertyChangedChainEvent Event{ *PropertyThatChanged, ChangedEvent };
			m_pTargetDataObject->PostEditChangeChainProperty(Event);

			return;
		}
		m_pTargetDataObject->PostEditChangeProperty(ChangedEvent);
	}


}
