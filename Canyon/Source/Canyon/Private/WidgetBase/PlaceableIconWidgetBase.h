#pragma once

#include "CoreMinimal.h"
#include "UserWidget.h"
#include "SlateFontInfo.h"
#include "UI/DeckStateRenderer.h"
#include "PlaceableIconWidgetBase.generated.h"


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeckWidgetClickedDelegate, const class UWidget &, ClickedWidget)

UCLASS()
class UPlaceableIconWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPlaceableIconWidgetBase(const FObjectInitializer& ObjectInitializer);

	void SetChargeAmount(uint32 Charges);

	virtual bool Initialize() override;


protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;


	UPROPERTY(EditAnywhere, Category="Appearance", DisplayName="Charge Display Font Style")
		FSlateFontInfo m_ChargesFontStyle;

	UPROPERTY(BlueprintReadOnly)
		class UPointCircleWidgetBase *m_pPointCounter;


private:
	UFUNCTION()
		UWidget *ReceiveOnTooltipInvoked();

	UPROPERTY()
		FDeckWidgetClickedDelegate m_eOnClicked;

	UPROPERTY()
		FGetDeckWidget m_dOnInvokeTooltip;

	UPROPERTY()
		TSubclassOf<class UPointCircleWidgetBase> m_pPointCounterClass;

	
public:
	using t_ClickDelegate = decltype(m_eOnClicked)::FDelegate;

	void AddEventToOnClicked(t_ClickDelegate &Callback);

	void RemoveEventFromOnClicked(t_ClickDelegate &Callback);

	void SetDelegateOnTooltipInvoked(const FGetDeckWidget &Callback);
		
	void ClearDelegateOnTooltipInvoked();

	
};
