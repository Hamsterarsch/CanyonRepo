// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckStateRenderer.h"
#include "Misc/CanyonGM.h"
#include "UI/DeckState.h"
#include "WidgetBase/MainHudWidgetBase.h"
#include "Button.h"
#include "WidgetBase/PlaceableIconWidgetBase.h"


//Public--------------

void UDeckStateRenderer::NotifyOnIconWidgetChanged(const TSubclassOf<UPlaceableIconWidgetBase> WidgetClass, const FCategoryData& Data)
{
	//Find or create widget
	auto *ppWidget{ m_IconWidgetMap.Find(WidgetClass) };

	auto *pClass{ WidgetClass.Get() };
	
	if(!ppWidget)
	{
		ppWidget = &m_IconWidgetMap.Add(WidgetClass, CreateWidget<UPlaceableIconWidgetBase>(m_pGM->GetWorld(), pClass));

		UPlaceableIconWidgetBase::t_ClickDelegate Delegate{};
		Delegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UDeckStateRenderer, NotifyOnIconClicked));
		(*ppWidget)->AddEventToOnClicked(Delegate);

		FGetDeckWidget TooltipDelegate{};
		TooltipDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UDeckStateRenderer, NotifyOnTooltipInvoked));
		(*ppWidget)->SetDelegateOnTooltipInvoked(TooltipDelegate);

		m_pTargetWidget->OnNewPlaceableIconAvailable(*ppWidget);		
	}

	//Update widgets
	if(Data.m_ChargeCount <= 0)
	{
		m_pTargetWidget->OnPlaceableIconUnavailable(*ppWidget);
		m_IconWidgetMap.Remove(WidgetClass);
				
		return;


	}

	(*ppWidget)->SetChargeAmount(Data.m_ChargeCount);


}

void UDeckStateRenderer::NotifyOnNewDecksSet(const TArray<TSubclassOf<UPrettyWidget>>& aWidgetClasses)
{
	m_aPendingSelectableDeckWidgets.Reset(aWidgetClasses.Num());
	for(auto &&WidgetClass : aWidgetClasses)
	{
		auto *pWidget{ CreateWidget<UPrettyWidget>(m_pGM->GetWorld(), WidgetClass.Get()) };

		//add bindings
		FOnMouseButtonDownPrettyDelegate::FDelegate Delegate{};
		Delegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UDeckStateRenderer, NotifyOnDeckClicked));
		pWidget->EventOnMouseButtonDownAdd(Delegate);

		m_aPendingSelectableDeckWidgets.Add(pWidget);
	
	}

	m_pTargetWidget->OnNewDecksSet(m_aPendingSelectableDeckWidgets);


}

UDeckStateRenderer* UDeckStateRenderer::Construct(ACanyonGM* pGM, UDeckState *pDeckState, UMainHudWidgetBase *pTargetWidget)
{
	auto *pObj{ NewObject<UDeckStateRenderer>() };

	pObj->m_pGM = pGM;
	pObj->m_pDeckState = pDeckState;
	pObj->m_pTargetWidget = pTargetWidget;

	UDeckState::t_StateChangedDelegate OnStateChangedDelegate{};
	OnStateChangedDelegate.BindUFunction(pObj, GET_FUNCTION_NAME_CHECKED(UDeckStateRenderer, NotifyOnIconWidgetChanged));
	pObj->m_pDeckState->AddEventToOnStateChanged(OnStateChangedDelegate);

	UDeckState::t_NewDecksSetDelegate OnNewDecksSetDelegate{};
	OnNewDecksSetDelegate.BindUFunction(pObj, GET_FUNCTION_NAME_CHECKED(UDeckStateRenderer, NotifyOnNewDecksSet));
	pObj->m_pDeckState->AddEventToOnDeckStateNewDecksSet(OnNewDecksSetDelegate);

	UDeckStateRenderer::t_OnIconClickedDelegate OnIconClickedDelegate{};
	OnIconClickedDelegate.BindUFunction(pObj->m_pDeckState, GET_FUNCTION_NAME_CHECKED(UDeckState, ReceiveOnWidgetClicked));
	pObj->AddEventToOnClicked(OnIconClickedDelegate);

	FGetDeckWidget GetTooltipDelegate{};
	GetTooltipDelegate.BindUFunction(pDeckState, GET_FUNCTION_NAME_CHECKED(UDeckState, GetTooltipWidgetForDeckWidget));
	pObj->SetDelegateOnTooltipInvoked(GetTooltipDelegate);

	pDeckState->m_eOnDeckCommitted.AddDynamic(pTargetWidget, &UMainHudWidgetBase::OnNewDeckCommitted);

	pTargetWidget->m_pAddDeckButton->OnClicked.AddDynamic(pDeckState, &UDeckState::NotifyAddDeckButtonClicked);

	pDeckState->m_eOnDeckChargeAdded.AddDynamic(pObj, &UDeckStateRenderer::OnDeckChargeAdded);



	return pObj;


}

void UDeckStateRenderer::OnDeckChargeAdded()
{
	m_pTargetWidget->OnDeckChargeAdded();


}

void UDeckStateRenderer::NotifyOnIconClicked(const UWidget* pClickedWidget)
{
	m_eOnIconClicked.Broadcast(pClickedWidget);


}

void UDeckStateRenderer::NotifyOnDeckClicked(UPrettyWidget *pClickedWidget)
{
	int32 FoundAt;
	
	if(m_aPendingSelectableDeckWidgets.Find(pClickedWidget, FoundAt))
	{
		m_pTargetWidget->OnDeckSelected(FoundAt);
		m_pDeckState->NotifyOnDeckWidgetClicked(FoundAt);		
	}

	m_aPendingSelectableDeckWidgets.Reset();


}

UWidget* UDeckStateRenderer::NotifyOnTooltipInvoked(const UWidget *pInstigator)
{
	if(!m_dOnTooltipInvoked.IsBound())
	{
		return CreateWidget(m_pGM->GetWorld());


	}

	return m_dOnTooltipInvoked.Execute(pInstigator);


}

void UDeckStateRenderer::AddEventToOnClicked(t_OnIconClickedDelegate& Callback)
{
	m_eOnIconClicked.Add(Callback);


}

void UDeckStateRenderer::RemoveEventFromOnClicked(t_OnIconClickedDelegate& Callback)
{
	m_eOnIconClicked.Remove(Callback);


}

void UDeckStateRenderer::SetDelegateOnTooltipInvoked(const FGetDeckWidget& Callback)
{
	m_dOnTooltipInvoked = Callback;


}

void UDeckStateRenderer::ClearDelegateOnTooltipInvoked()
{
	m_dOnTooltipInvoked.Clear();


}


//Private-----------------------
