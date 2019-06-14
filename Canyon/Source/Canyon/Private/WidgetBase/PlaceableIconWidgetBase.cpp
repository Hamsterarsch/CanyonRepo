#include "WidgetBase/PlaceableIconWidgetBase.h"
#include "CanvasPanelSlot.h"
#include "TextBlock.h"
#include "CanvasPanel.h"
#include "WidgetTree.h"
#include "Button.h"


//Public--------------

void UPlaceableIconWidgetBase::SetChargeAmount(const uint32 Charges)
{
	m_pChargesTextBlock->SetText( FText::FromString(FString::FromInt(static_cast<int32>(Charges))) );
	
	
}

bool UPlaceableIconWidgetBase::Initialize()
{
	const auto SuperRet{ Super::Initialize() };

	if(!HasAnyFlags(RF_ClassDefaultObject))
	{
		auto *pCanvasPanel = WidgetTree->ConstructWidget<UCanvasPanel>();

		//reparent the old root
		if(WidgetTree->RootWidget)
		{
			auto *pOldRoot{ WidgetTree->RootWidget };

			auto *pSlot{ pCanvasPanel->AddChildToCanvas(pOldRoot) };
			pSlot->SetAnchors(FAnchors{ 0, 0, 1, 1 });
			pSlot->SetOffsets(FMargin{ 0 });
		}
		WidgetTree->RootWidget = pCanvasPanel;
	}

	return SuperRet;


}

void UPlaceableIconWidgetBase::AddEventToOnClicked(t_ClickDelegate &Callback)
{
	m_eOnClicked.Add(Callback);


}

void UPlaceableIconWidgetBase::RemoveEventFromOnClicked(t_ClickDelegate &Callback)
{
	m_eOnClicked.Remove(Callback);


}


//Protected-----------------------

void UPlaceableIconWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	if(!m_pChargesTextBlock)
	{
		m_pChargesTextBlock = NewObject<UTextBlock>(this);
		m_pChargesTextBlock->SetText(FText::FromString("0"));	
		m_pChargesTextBlock->SetFont(m_ChargesFontStyle);		
	}

	auto *pSlot =	Cast<UCanvasPanel>(GetRootWidget())->AddChildToCanvas(m_pChargesTextBlock);
	pSlot->SetAnchors(FAnchors{ 1, 1, 1, 1 });	
	pSlot->SetAutoSize(true);
	pSlot->SetAlignment(FVector2D{ 1, 1 });
	pSlot->SetOffsets(FMargin{ 0 });


}

FReply UPlaceableIconWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	m_eOnClicked.Broadcast(this);
		
	return FReply::Handled();


}

