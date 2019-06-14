// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget.h"
#include "PrettyWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMouseButtonDownPrettyDelegate, class UPrettyWidget *, pWidget);

UCLASS()
class UPrettyWidget : public UUserWidget
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintImplementableEvent)
		void ShowWidget();

	UFUNCTION(BlueprintImplementableEvent)
		void HideWidget();


protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	FOnMouseButtonDownPrettyDelegate m_eOnMouseButtonDown;


public:
	using t_OnMouseDownDelegate = decltype(m_eOnMouseButtonDown)::FDelegate;

	void EventOnMouseButtonDownAdd(t_OnMouseDownDelegate &Callback);

	void EventOnMouseButtonDownRemove(t_OnMouseDownDelegate &Callback);


};
