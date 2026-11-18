/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

using System.IO;
using UnrealBuildTool;

public class VivoxCoreLibrary : ModuleRules
{
    public VivoxCoreLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        UnrealTargetPlatform parsedPlatform;

        if (UnrealTargetPlatform.TryParse("Win64", out parsedPlatform) && Target.Platform == parsedPlatform)
        {
            // Add the Win64 specific Include and Library paths
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Windows", "include"));
            string VivoxLibraryPath = Path.Combine(ModuleDirectory, "Windows", "Release", "x64");

            PublicAdditionalLibraries.Add(Path.Combine(VivoxLibraryPath, "vivoxsdk.lib"));

            // Delay-load the DLL, so we can load it from the right place
            PublicDelayLoadDLLs.Add("vivoxsdk.dll");
            RuntimeDependencies.Add(Path.Combine(VivoxLibraryPath, "vivoxsdk.dll"));
        }
        else if (UnrealTargetPlatform.TryParse("PS4", out parsedPlatform) && Target.Platform == parsedPlatform)
        {
            // Add the PS4 specific Include and Library paths
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "PlayStation4", "include"));
            string VivoxLibraryPath = Path.Combine(ModuleDirectory, "PlayStation4", "Release");

            PublicAdditionalLibraries.Add(Path.Combine(VivoxLibraryPath, "libvivoxsdk_stub.a"));

            PublicDelayLoadDLLs.Add("libvivoxsdk.prx");
            RuntimeDependencies.Add(Path.Combine(VivoxLibraryPath, "libvivoxsdk.prx"));

#if UE_4_24_OR_LATER
            PublicSystemLibraries.Add("SceSha1");
            PublicSystemLibraries.Add("SceAudioIn_stub_weak");
            PublicSystemLibraries.Add("SceHmac");
#else
            PublicAdditionalLibraries.Add("SceSha1");
            PublicAdditionalLibraries.Add("SceAudioIn_stub_weak");
            PublicAdditionalLibraries.Add("SceHmac");
#endif
        }
        else if (UnrealTargetPlatform.TryParse("PS5", out parsedPlatform) && Target.Platform == parsedPlatform)
        {
            // Add the PS5 specific Include and Library paths
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "PlayStation5", "include"));
            string VivoxLibraryPath = Path.Combine(ModuleDirectory, "PlayStation5", "Release");

            PublicAdditionalLibraries.Add(Path.Combine(VivoxLibraryPath, "libvivoxsdk_stub.a"));

            PublicDelayLoadDLLs.Add("libvivoxsdk.prx");
            RuntimeDependencies.Add(Path.Combine(VivoxLibraryPath, "libvivoxsdk.prx"));

#if UE_4_24_OR_LATER
            PublicSystemLibraries.Add("SceSha1");
            PublicSystemLibraries.Add("SceHmac");
#else
            PublicAdditionalLibraries.Add("SceSha1");
            PublicAdditionalLibraries.Add("SceHmac");
#endif
        }        
        else if (UnrealTargetPlatform.TryParse("Mac", out parsedPlatform) && Target.Platform == parsedPlatform)
        {
            // Add the Mac specific Include and Library paths
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Mac", "include"));
            string VivoxLibraryPath = Path.Combine(ModuleDirectory, "Mac", "Release");
            string EnginePluginPath = Path.Combine(Path.GetFullPath(Target.RelativeEnginePath), "Plugins", "VivoxCore", "Source", "ThirdParty", "VivoxCoreLibrary", "Mac", "Release");
            string sdkLib = "libvivoxsdk.dylib";

            if(Target.bBuildEditor)
            {
                PublicAdditionalLibraries.Add(Path.Combine(VivoxLibraryPath, sdkLib));
            }
            else
            {
                PublicAdditionalLibraries.Add(Path.Combine(EnginePluginPath, sdkLib));
            }
            RuntimeDependencies.Add(Path.Combine(EnginePluginPath, sdkLib), Path.Combine(VivoxLibraryPath, sdkLib), StagedFileType.NonUFS);
            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("MacPlugin", Path.Combine(PluginPath, "VivoxCoreSDK_Mac_UPL.xml"));
        }
        else if (UnrealTargetPlatform.TryParse("Android", out parsedPlatform) && Target.Platform == parsedPlatform)
        {
            // Add the Android specific Include and Library paths
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Android", "include"));
            string VivoxLibraryPath = Path.Combine(ModuleDirectory, "Android", "Release");

            PublicAdditionalLibraries.Add(Path.Combine(VivoxLibraryPath, "arm64-v8a", "libvivox-sdk.so"));
            PublicAdditionalLibraries.Add(Path.Combine(VivoxLibraryPath, "armeabi-v7a", "libvivox-sdk.so"));
            PublicAdditionalLibraries.Add(Path.Combine(VivoxLibraryPath, "x86", "libvivox-sdk.so"));
            PublicAdditionalLibraries.Add(Path.Combine(VivoxLibraryPath, "x86_64", "libvivox-sdk.so"));

            PublicDependencyModuleNames.AddRange(new string[] { "Launch" });
            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "VivoxCoreSDK_UPL.xml"));
        }
        else if (UnrealTargetPlatform.TryParse("IOS", out parsedPlatform) && Target.Platform == parsedPlatform)
        {
            // Add the IOS specific Include and Library paths
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "IOS", "include"));
            string VivoxLibraryPath = Path.Combine(ModuleDirectory, "IOS", "Release");

            PublicAdditionalLibraries.Add(Path.Combine(VivoxLibraryPath, "libvivoxsdk.a"));
            PublicFrameworks.Add( "CFNetwork" );
            PublicFrameworks.Add( "AVFoundation");
            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(PluginPath, "VivoxCoreSDK_IOS_UPL.xml"));
        }
        else if (UnrealTargetPlatform.TryParse("Switch", out parsedPlatform) && Target.Platform == parsedPlatform)
        {
            // Add the Switch specific Include and Library paths
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "NX", "include"));
            string VivoxLibraryPath = Path.Combine(ModuleDirectory, "NX", "Release");

            PublicAdditionalLibraries.Add(Path.Combine(VivoxLibraryPath, "libvivoxsdk.a"));
#if UE_4_24_OR_LATER
            PublicSystemLibraries.Add("curl");
            PublicSystemLibraries.Add("z");
#else
            PublicAdditionalLibraries.Add("curl");
            PublicAdditionalLibraries.Add("z");
#endif
        }
        else if (UnrealTargetPlatform.TryParse("Switch2", out parsedPlatform) && Target.Platform == parsedPlatform)
        {
            // Add the Switch2 specific Include and Library paths
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Ounce", "include"));
            string VivoxLibraryPath = Path.Combine(ModuleDirectory, "Ounce", "Release");

            PublicAdditionalLibraries.Add(Path.Combine(VivoxLibraryPath, "libvivoxsdk.a"));
#if UE_4_24_OR_LATER
            PublicSystemLibraries.Add("curl");
            PublicSystemLibraries.Add("z");
#else
            PublicAdditionalLibraries.Add("curl");
            PublicAdditionalLibraries.Add("z");
#endif
        }
        else if (UnrealTargetPlatform.TryParse("XSX", out parsedPlatform) && Target.Platform == parsedPlatform)
        {
            // Add the XSX specific Include and Library paths
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Scarlett", "include"));
            string VivoxLibraryPath = Path.Combine(ModuleDirectory, "Scarlett", "Release");

            PublicAdditionalLibraries.Add(Path.Combine(VivoxLibraryPath, "vivoxsdk.lib"));

            // Declare dependency, marshalling the DLL alongside the executable
            RuntimeDependencies.Add("$(TargetOutputDir)/vivoxsdk.dll", Path.Combine(VivoxLibraryPath, "vivoxsdk.dll"), StagedFileType.NonUFS);
        }
        else if (UnrealTargetPlatform.TryParse("XboxOneGDK", out parsedPlatform) && Target.Platform == parsedPlatform)
        {
            // Add the XboxOneGDK specific Include and Library paths
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "XB1GDK", "include"));
            string VivoxLibraryPath = Path.Combine(ModuleDirectory, "XB1GDK", "Release");

            PublicAdditionalLibraries.Add(Path.Combine(VivoxLibraryPath, "vivoxsdk.lib"));

            // Declare dependency, marshalling the DLL alongside the executable
            RuntimeDependencies.Add("$(TargetOutputDir)/vivoxsdk.dll", Path.Combine(VivoxLibraryPath, "vivoxsdk.dll"), StagedFileType.NonUFS);
        }
    }
}
