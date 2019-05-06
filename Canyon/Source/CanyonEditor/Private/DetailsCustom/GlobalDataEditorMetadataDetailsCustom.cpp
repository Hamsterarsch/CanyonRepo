#include "DetailsCustom/GlobalDataEditorMetadataDetailsCustom.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"

TSharedRef<IDetailCustomization> FGlobalDataEditorMetadataDetailsCustom::MakeInstance()
{
	return MakeShared<FGlobalDataEditorMetadataDetailsCustom>();


}

void FGlobalDataEditorMetadataDetailsCustom::CustomizeDetails(IDetailLayoutBuilder &DetailBuilder)
{
	auto &CategoryBuilder{ DetailBuilder.EditCategory(TEXT("GlobalDataEditorMetadata")) };
	

}
