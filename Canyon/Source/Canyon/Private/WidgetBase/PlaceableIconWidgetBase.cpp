#include "WidgetBase/PlaceableIconWidgetBase.h"
#include "CanvasPanelSlot.h"
#include "TextBlock.h"
#include "CanvasPanel.h"
#include "WidgetTree.h"
#include "Button.h"
#include "ConstructorHelpers.h"
#include "Engine/Font.h"
#include "ScaleBox.h"
#include "WidgetBase/PointCircleWidgetBase.h"
#include "ScaleBoxSlot.h"


//Public--------------

UPlaceableIconWidgetBase::UPlaceableIconWidgetBase(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());
	m_ChargesFontStyle = FSlateFontInfo(RobotoFontObj.Object, 24, FName("Bold"));
	m_ChargesFontStyle.OutlineSettings.OutlineSize = 1;


	static ConstructorHelpers::FClassFinder<UPointCircleWidgetBase> PointCounterClass{TEXT("WidgetBlueprint'/Game/Widgets/PointCircleIndicator.PointCircleIndicator_C'")};
	m_pPointCounterClass = PointCounterClass.Class;

}

void UPlaceableIconWidgetBase::SetChargeAmount(const uint32 Charges)
{
	m_pPointCounter->SetCounter(static_cast<int32>(Charges));
	
	
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
	
	m_pPointCounter = NewObject<UPointCircleWidgetBase>(this, m_pPointCounterClass.Get());
	m_pPointCounter->SetCounter(0);		
	m_pPointCounter->SetClipping(EWidgetClipping::ClipToBoundsWithoutIntersecting);

	auto *pSlot{ Cast<UCanvasPanel>(GetRootWidget())->AddChildToCanvas(m_pPointCounter) };
	pSlot->SetAnchors(FAnchors{ 1, 0, 1, 0 });	
	pSlot->SetAutoSize(true);
	pSlot->SetAlignment(FVector2D{ 0.75f, 0.5f });
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

