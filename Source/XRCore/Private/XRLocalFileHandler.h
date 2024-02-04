
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "XRLocalFileHandler.generated.h"


UCLASS( ClassGroup=(XRToolkit))
class UXRLocalFileHandler : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	 * Saves the string into a file in the project directory.
	 *@param InString File Content to save
	 *@param InLocalFilePath Path to the file to read from. 
	 */
	UFUNCTION(BlueprintCallable, Category = "XRToolkit|Utility|XRLocalFileHandler")
	static bool SaveToLocalFile(FString InString, FString InLocalFilePath);


	/**
	 * Reads the content of a local file in the project directory. 
	 *@param OutFileContent Content of the file as type string. (if file was found)
	 *@param InLocalFilePath Path of the file to save to - Can be an existing file (which will override the content),
	 *otherwise a new file will be created. 
	 */
	UFUNCTION(BlueprintPure, Category = "XRToolkit|Utility|XRLocalFileHandler")
		static bool LoadFromLocalFile(FString& OutFileContent, FString InLocalFilePath);
};
