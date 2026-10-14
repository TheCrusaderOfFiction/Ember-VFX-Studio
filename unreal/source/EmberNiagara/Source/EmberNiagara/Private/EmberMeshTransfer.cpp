#include "EmberMeshTransfer.h"
#include "Engine/StaticMesh.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "IAssetTools.h"
#include "AutomatedAssetImportData.h"
#include "Factories/FbxFactory.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxStaticMeshImportData.h"
#include "AssetExportTask.h"
#include "Exporters/Exporter.h"
#include "Exporters/FbxExportOption.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"

bool EmberImportMesh(const FString& File,FString& Asset,FString& Error){
 if(!File.EndsWith(TEXT(".fbx"),ESearchCase::IgnoreCase)){Error=TEXT("Expected a static-mesh FBX file.");return false;}
 auto* Factory=NewObject<UFbxFactory>();Factory->SetDetectImportTypeOnImport(false);Factory->ImportUI->MeshTypeToImport=FBXIT_StaticMesh;Factory->ImportUI->bImportAsSkeletal=false;Factory->ImportUI->bImportAnimations=false;Factory->ImportUI->bAutomatedImportShouldDetectType=false;Factory->ImportUI->bImportMaterials=true;Factory->ImportUI->bImportTextures=true;Factory->ImportUI->StaticMeshImportData->bCombineMeshes=true;
 auto* Import=NewObject<UAutomatedAssetImportData>();Import->Filenames.Add(File);Import->DestinationPath=TEXT("/Game/Ember/Meshes/Transfer_")+FGuid::NewGuid().ToString(EGuidFormats::Digits).Left(8);Import->Factory=Factory;Import->bReplaceExisting=false;
 const auto Objects=FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get().ImportAssetsAutomated(Import);
 TArray<FAssetData> ImportedAssets;FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get().GetAssetsByPath(FName(*Import->DestinationPath),ImportedAssets,true);for(const auto& Entry:ImportedAssets){auto* Object=Entry.GetAsset();if(!Object)continue;auto* Package=Object->GetOutermost();FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;const FString Filename=FPackageName::LongPackageNameToFilename(Package->GetName(),FPackageName::GetAssetPackageExtension());if(!UPackage::SavePackage(Package,Object,*Filename,Args)){Error=TEXT("Could not save imported mesh materials or textures.");return false;}}
 UStaticMesh* Mesh=nullptr;for(auto* Object:Objects){auto* Package=Object->GetOutermost();FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;const FString Filename=FPackageName::LongPackageNameToFilename(Package->GetName(),FPackageName::GetAssetPackageExtension());if(!UPackage::SavePackage(Package,Object,*Filename,Args)){Error=TEXT("Could not save imported mesh assets.");return false;}if(!Mesh)Mesh=Cast<UStaticMesh>(Object);}
 if(!Mesh){Error=TEXT("No static mesh was imported. Export visible mesh objects from Blender.");return false;}Asset=Mesh->GetPathName();return true;
}
bool EmberExportMesh(const FString& Asset,const FString& File,FString& Error){
 auto* Mesh=LoadObject<UStaticMesh>(nullptr,*Asset);if(!Mesh){Error=TEXT("Choose a static mesh asset from the Unreal project.");return false;}
 auto* Task=NewObject<UAssetExportTask>();Task->Object=Mesh;Task->Filename=File;Task->bAutomated=true;Task->bPrompt=false;Task->bSelected=false;Task->bReplaceIdentical=false;Task->Exporter=UExporter::FindExporter(Mesh,TEXT("fbx"));Task->Options=GetMutableDefault<UFbxExportOption>();
 if(!Task->Exporter||!UExporter::RunAssetExportTask(Task)){Error=TEXT("Unreal could not export this static mesh as FBX.");return false;}return true;
}
