#include "CanyonEditorCommands.h"

#define LOCTEXT_NAMESPACE "FCanyonEditorCommands"

void FCanyonEditorCommands::RegisterCommands()
{
	UI_COMMAND(pCmdOpenInfluenceEditor, "Influence Editor", "Canyon influence editor.", EUserInterfaceActionType::CollapsedButton, FInputChord{});


}

#undef LOCTEXT_NAMESPACE
