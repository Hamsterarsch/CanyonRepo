#pragma once
#include "CoreMinimal.h"
#include "UserWidget.h"
#include "PointCircleWidgetBase.generated.h"


UCLASS()
class UPointCircleWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetCounter(int32 NewCounter);


protected:
	UPROPERTY(VisibleAnywhere, Meta=(BindWidget))
		class UTextBlock *m_pCounterText;


};
