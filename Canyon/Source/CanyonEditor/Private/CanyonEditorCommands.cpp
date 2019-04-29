#include "CanyonEditorCommands.h"

#define LOCTEXT_NAMESPACE "FCanyonEditorCommands"

void FCanyonEditorCommands::RegisterCommands()
{
	UI_COMMAND(TestCommand, "TestCommand", "This is test command", EUserInterfaceActionType::CollapsedButton, FInputChord{});

}

#undef LOCTEXT_NAMESPACE
