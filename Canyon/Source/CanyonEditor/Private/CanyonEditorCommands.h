#pragma once
#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "EditorStyleSet.h"

class FCanyonEditorCommands : public TCommands<FCanyonEditorCommands>
{
public:
	FCanyonEditorCommands() :
	TCommands<FCanyonEditorCommands>
	(
		TEXT("CanyonEditor"),
		NSLOCTEXT("Contexts", "CanyonEditor", "CanyonEditor Editor"),
		NAME_None,
		FEditorStyle::GetStyleSetName()
	)
	{}

	virtual void RegisterCommands() override;

	TSharedPtr<class FUICommandInfo> TestCommand;

};
