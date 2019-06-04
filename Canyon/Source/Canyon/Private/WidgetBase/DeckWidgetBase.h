#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PrettyWidget.h"
#include "DeckWidgetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeckSelectedDelegate, int32, DeckIndex);

UCLASS(BlueprintType)
class UDeckWidgetBase: public UPrettyWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveOnDeckSelected(int32 DeckIndex);
	   
	void ReceiveOnInvokeDeck(TSubclassOf<UUserWidget> DeckWidget);


	UPROPERTY()
		FDeckSelectedDelegate m_OnDeckSelected;


protected:
	virtual void NativeTick(const FGeometry &MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
		void OnInvokeDeck(class UButton *pDeckWidget);


private:
	UPROPERTY()
		TArray<class UButton *> m_apDeckButtons;


};
