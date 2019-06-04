#include "WidgetBase/DeckWidgetBase.h"
#include "Runtime/UMG/Public/Components/Button.h"

void UDeckWidgetBase::ReceiveOnInvokeDeck(TSubclassOf<UUserWidget> DeckWidget)
{
	auto *pButton{ NewObject<UButton>(this) };
	pButton->AddChild(CreateWidget<UUserWidget>(this, DeckWidget.Get()));
	m_apDeckButtons.Add(pButton);

	OnInvokeDeck(pButton);
	

}


//Protected------------------

void UDeckWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	int32 PressedIndex{ -1 };
	for(auto *pButton : m_apDeckButtons)
	{
		if(pButton->IsPressed())
		{
			m_apDeckButtons.Find(pButton, PressedIndex);
			break;
		}
	
	}

	if(PressedIndex >= 0)
	{
		m_apDeckButtons.Empty();
		ReceiveOnDeckSelected(PressedIndex);
		m_OnDeckSelected.Broadcast(PressedIndex);

	}

}
