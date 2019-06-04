#include "Widgets/SInfluenceDataEditor.h"
#include "InfluenceDataObject.h"
#include "ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Canyon/Private/Misc/CanyonGlobalData.h"


void SInfluenceDataEditor::Construct(const FArguments& Args)
{
	auto &PropertyModule{ FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor")) };
		
	m_pEditorDataObject = UInfluenceDataObject::CreateFromFile();
	
	FDetailsViewArgs ViewArgs
	{
		false,
		false,
		false,
		FDetailsViewArgs::ENameAreaSettings::HideNameArea,
		false,
		this,
		false,
		TEXT("InfluenceEditorView")
	};
	auto DetailView{ PropertyModule.CreateDetailView(ViewArgs) };
	
	DetailView->SetObject(m_pEditorDataObject, false);
	ChildSlot[DetailView];

	
}

void SInfluenceDataEditor::RefreshObjectData()
{
	m_pEditorDataObject->InitializeData();


}

void SInfluenceDataEditor::NotifyPreChange(UProperty* PropertyAboutToChange)
{
	if(!m_pEditorDataObject)
	{
		return;
	}

	m_pEditorDataObject->PreEditChange(PropertyAboutToChange);
	

}

void SInfluenceDataEditor::NotifyPreChange(FEditPropertyChain* PropertyAboutToChange)
{
	m_pCurrentPropertyChain = PropertyAboutToChange;

	if (!m_pEditorDataObject)
	{
		return;
	}

	m_pEditorDataObject->PreEditChange(PropertyAboutToChange->GetActiveNode()->GetValue());


}

void SInfluenceDataEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,	UProperty* PropertyThatChanged)
{
	if (!m_pEditorDataObject)
	{
		return;
	}

	if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		FPropertyChangedEvent ChangedEvent{ PropertyChangedEvent };

		if (PropertyThatChanged->IsA<UStructProperty>())
		{
			FPropertyChangedChainEvent Event{ *m_pCurrentPropertyChain, ChangedEvent };
			m_pEditorDataObject->PostEditChangeChainProperty(Event);

			return;
		}		
		m_pEditorDataObject->PostEditChangeProperty(ChangedEvent);		
	}


}


void SInfluenceDataEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,	FEditPropertyChain* PropertyThatChanged)
{
	if (!m_pEditorDataObject)
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
			m_pEditorDataObject->PostEditChangeChainProperty(Event);

			return;
		}
		m_pEditorDataObject->PostEditChangeProperty(ChangedEvent);
	}


}


