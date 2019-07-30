#include "WidgetBase/PointCircleWidgetBase.h"
#include "TextBlock.h"


void UPointCircleWidgetBase::SetCounter(int32 NewCounter)
{
	if(m_pCounterText)
	{
		m_pCounterText->SetText(FText::FromString(FString::FromInt(NewCounter)));		
	}


}
