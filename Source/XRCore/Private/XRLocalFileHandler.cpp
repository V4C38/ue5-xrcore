
#include "XRLocalFileHandler.h"


bool UXRLocalFileHandler::SaveToLocalFile(FString InString, FString InLocalFilePath)
{
	return FFileHelper::SaveStringToFile(InString, *InLocalFilePath);
}

bool UXRLocalFileHandler::LoadFromLocalFile(FString& OutFileContent, FString InLocalFilePath)
{
	return FFileHelper::LoadFileToString(OutFileContent, *InLocalFilePath);
}
