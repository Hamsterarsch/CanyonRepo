// Fill out your copyright notice in the Description page of Project Settings.


#include "PrettyWidget.h"

FReply UPrettyWidget::NativeOnMouseButtonDown(const FGeometry &InGeometry, const FPointerEvent &InMouseEvent)
{
	m_eOnMouseButtonDown.Broadcast(this);
	
	return FReply::Handled();


}

void UPrettyWidget::EventOnMouseButtonDownAdd(t_OnMouseDownDelegate &Callback)
{
	m_eOnMouseButtonDown.Add(Callback);


}

void UPrettyWidget::EventOnMouseButtonDownRemove(t_OnMouseDownDelegate &Callback)
{
	m_eOnMouseButtonDown.Remove(Callback);


}
