// Copyright Epic Games, Inc. All Rights Reserved.

#include "XR_Toolkit.h"
#include "XRCoreSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FXR_ToolkitModule"

void FXR_ToolkitModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->RegisterSettings("Project", "Plugins", "XRCore",
            LOCTEXT("RuntimeSettingsName", "XRCore"),
            LOCTEXT("RuntimeSettingsDescription", "Configure Settings for XRCore."),
            GetMutableDefault<UXRCoreSettings>()
        );
    }
}

void FXR_ToolkitModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXR_ToolkitModule, XR_Toolkit)