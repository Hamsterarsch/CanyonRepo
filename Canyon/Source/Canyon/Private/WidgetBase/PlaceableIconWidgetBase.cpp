#include "WidgetBase/PlaceableIconWidgetBase.h"
#include "CanvasPanelSlot.h"
#include "TextBlock.h"
#include "CanvasPanel.h"
#include "WidgetTree.h"
#include "Button.h"
#include "ConstructorHelpers.h"
#include "Engine/Font.h"
#include "ScaleBox.h"
#include "ScaleBoxSlot.h"


//Public--------------

UPlaceableIconWidgetBase::UPlaceableIconWidgetBase(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());
	m_ChargesFontStyle = FSlateFontInfo(RobotoFontObj.Object, 24, FName("Bold"));
	m_ChargesFontStyle.OutlineSettings.OutlineSize = 1;
	

}

void UPlaceableIconWidgetBase::SetChargeAmount(const uint32 Charges)
{
	m_pChargesTextBlock->SetText( FText::FromString(FString::FromInt(static_cast<int32>(Charges))) );
	
	
}

bool UPlaceableIconWidgetBase::Initialize()
{
	const auto SuperRet{ Super::Initialize() };

	if(!HasAnyFlags(RF_ClassDefaultObject))
	{
		auto *pCanvasPanel{ WidgetTree->ConstructWidget<UCanvasPanel>() };
		auto *pScaleBox{ WidgetTree->ConstructWidget<UScaleBox>() };
		
		//reparent the old root
		if(WidgetTree->RootWidget)
		{
			auto *pOldRoot{ WidgetTree->RootWidget };

			pScaleBox->SetStretch(EStretch::ScaleToFitY);
			pScaleBox->AddChild(pOldRoot);

			auto *pSlot{ pCanvasPanel->AddChildToCanvas(pScaleBox) };
			pSlot->SetAnchors(FAnchors{ 0, 0, 1, 1 });
			pSlot->SetOffsets(FMargin{ 0 });
			pSlot->SetAutoSize(true);
		}
		WidgetTree->RootWidget = pCanvasPanel;

		//tooltip binding
		ToolTipWidgetDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UPlaceableIconWidgetBase, ReceiveOnTooltipInvoked));

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

void UPlaceableIconWidgetBase::SetDelegateOnTooltipInvoked(const FGetDeckWidget &Callback)
{	
	m_dOnInvokeTooltip = Callback;


}

void UPlaceableIconWidgetBase::ClearDelegateOnTooltipInvoked()
{
	m_dOnInvokeTooltip.Clear();

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

	auto *pSlot{ Cast<UCanvasPanel>(GetRootWidget())->AddChildToCanvas(m_pChargesTextBlock) };
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

//Private------------------------------

UWidget* UPlaceableIconWidgetBase::ReceiveOnTooltipInvoked()
{
	return m_dOnInvokeTooltip.Execute(this);


}

