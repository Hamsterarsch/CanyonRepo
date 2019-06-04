#include "DetailsCustom/InfluenceDataObjectDetailsCustom.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "InfluenceDataObject.h"
#include "DetailWidgetRow.h"
#include "SGridPanel.h"
#include "STextBlock.h"
#include "SEditableTextBox.h"
#include "SButton.h"


TSharedRef<IDetailCustomization> FInfluenceDataObjectDetailsCustom::MakeInstance()
{
	return MakeShared<FInfluenceDataObjectDetailsCustom>();


}

void FInfluenceDataObjectDetailsCustom::CustomizeDetails(IDetailLayoutBuilder &DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> aObjects;
	DetailBuilder.GetObjectsBeingCustomized(aObjects);
	
	if(aObjects.Num() != 1 )
	{
		return;
	}

	auto *pCastObject{ Cast<UInfluenceDataObject>(aObjects[0].Get()) };
	if(!pCastObject)
	{
		return;
	}
		
	TArray<void *> aMaps;
	auto hPropRef_RowDataMap{ DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UInfluenceDataObject, m_InfluenceRowDataMap)) };
	hPropRef_RowDataMap->AccessRawData(aMaps);
	
	if(aMaps.Num() != 1)
	{
		return;
	}
	auto *pMap{ reinterpret_cast<UInfluenceDataObject::InfluenceGroupDataType *>(aMaps[0]) };

	//Widget generation
	auto GridWidget{ SNew(SGridPanel) };
	static const FMargin SlotPadding{ 4.5 };

	//save button
	GridWidget->AddSlot(0,0)
	.Padding(SlotPadding)
	[
		SNew(SButton)
		.ButtonColorAndOpacity(FSlateColor{FLinearColor::Green})
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Save")))
			.Justification(ETextJustify::Center)
		]
		.OnPressed_Lambda([pWeakTarget = aObjects[0]]()
		{
			auto  *pTarget{ Cast<UInfluenceDataObject>(pWeakTarget.Get()) };
			if(!pTarget)
			{
				return;
			}

			pTarget->SaveToFile();


		})
	];
	
	//Influence matrix row and column names
	TArray<FString> aKeyNames;
	pMap->GetKeys(aKeyNames);
	for(int32 KeyNameIndex{ 0 }; KeyNameIndex < aKeyNames.Num(); ++KeyNameIndex)
	{
		//row
		{
			auto &Slot{ GridWidget->AddSlot(0, KeyNameIndex + 1) };
			Slot
			.VAlign(EVerticalAlignment::VAlign_Center)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.Padding(SlotPadding)	
			[
				SNew(STextBlock).Text(FText::FromString(aKeyNames[KeyNameIndex]))
			];
		}

		//column
		{
			auto &Slot{ GridWidget->AddSlot(KeyNameIndex + 1, 0) };
			Slot
			.VAlign(EVerticalAlignment::VAlign_Top)
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.Padding(SlotPadding)
			[
				SNew(STextBlock).Text(FText::FromString(aKeyNames[KeyNameIndex]))
			];
		}

	}

	//influence matrix content fields
	for(int32 RowIndex{ 1 }; RowIndex < aKeyNames.Num() + 1; ++RowIndex)
	{
		for(int32 ColIndex{ 1 }; ColIndex < aKeyNames.Num() + 1; ++ColIndex)
		{			
			const auto CurrentInfluenceValue
			{
				pCastObject->m_InfluenceRowDataMap
				[
					aKeyNames[RowIndex - 1]
				]
				.m_InfluenceMapping
				[
					aKeyNames[ColIndex - 1]
				]
			};

			auto &Slot{ GridWidget->AddSlot(ColIndex, RowIndex) };
			Slot
			.Padding(SlotPadding)
			[
				SNew(SEditableTextBox)
				.Justification(ETextJustify::Center)
				.Text
				(
					FText::FromString
					(
						FString::FromInt
						(
							CurrentInfluenceValue
						)
					)
				)
				//On input text
				.OnTextCommitted_Lambda
				(
					[RowKey = aKeyNames[RowIndex - 1], ColKey = aKeyNames[ColIndex - 1], pWeakDataObj = aObjects[0]]
					(const FText &CommitedText, ETextCommit::Type CommitType)
					{						
						auto  *pDataObj{ Cast<UInfluenceDataObject>(pWeakDataObj.Get()) };
						if(!pDataObj)
						{
							return;
						}

						if(CommitedText.IsNumeric())
						{
							//set influence value
							pDataObj->m_InfluenceRowDataMap[RowKey].m_InfluenceMapping[ColKey] = FCString::Atoi(*CommitedText.ToString());							
						}
						

					}
				)
			];
			
			GridWidget->SetColumnFill(ColIndex, 1.f);
		}
		GridWidget->SetRowFill(RowIndex, 1.f);

	}

	//radius and ui widget properties
	
	
	//Add grid as category content
	auto &CategoryBuilder{ DetailBuilder.EditCategory(TEXT("Influence Matrix")) };
	
	auto &Row{ CategoryBuilder.AddCustomRow(FText::FromString("Data")) };
	Row
	[
		GridWidget
	];
	
	//hide standard category
	//DetailBuilder.HideCategory(TEXT("InfluenceDataObject"));



}
