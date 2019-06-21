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

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidgetOptional))
		class UTextBlock *m_pChargesTextBlock;


private:
	FDeckWidgetClickedDelegate m_eOnClicked;


public:
	using t_ClickDelegate = decltype(m_eOnClicked)::FDelegate;

	void AddEventToOnClicked(t_ClickDelegate &Callback);

	void RemoveEventFromOnClicked(t_ClickDelegate &Callback);

	
};
