#include "EmberWorkspace.h"
#include "EmberScene.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
#include "NiagaraParameterStore.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/SavePackage.h"

namespace {
TSharedPtr<FJsonObject> Describe(UNiagaraSystem* System){
 auto Result=MakeShared<FJsonObject>();Result->SetStringField(TEXT("asset"),System->GetPathName());Result->SetStringField(TEXT("name"),System->GetName());
 TArray<TSharedPtr<FJsonValue>> Emitters,Parameters;
 for(const auto& Handle:System->GetEmitterHandles()){
  auto E=MakeShared<FJsonObject>();E->SetStringField(TEXT("name"),Handle.GetName().ToString());E->SetBoolField(TEXT("enabled"),Handle.GetIsEnabled());
  const auto* Data=Handle.GetEmitterData();E->SetStringField(TEXT("simulation"),Data&&Data->SimTarget==ENiagaraSimTarget::GPUComputeSim?TEXT("GPU"):TEXT("CPU"));
  TArray<TSharedPtr<FJsonValue>> Renderers;if(Data)for(auto* R:Data->GetRenderers())Renderers.Add(MakeShared<FJsonValueString>(R->GetClass()->GetName()));E->SetArrayField(TEXT("renderers"),Renderers);Emitters.Add(MakeShared<FJsonValueObject>(E));
 }
 auto& Store=System->GetExposedParameters();TArray<FNiagaraVariable> Vars;Store.GetParameters(Vars);
 for(const auto& V:Vars){
  auto P=MakeShared<FJsonObject>();P->SetStringField(TEXT("name"),V.GetName().ToString());FString Type=TEXT("advanced");
  if(V.GetType()==FNiagaraTypeDefinition::GetFloatDef()){Type=TEXT("number");P->SetNumberField(TEXT("value"),Store.GetParameterValue<float>(V));}
  else if(V.GetType()==FNiagaraTypeDefinition::GetIntDef()){Type=TEXT("integer");P->SetNumberField(TEXT("value"),Store.GetParameterValue<int32>(V));}
  else if(V.GetType()==FNiagaraTypeDefinition::GetBoolDef()){Type=TEXT("boolean");P->SetBoolField(TEXT("value"),Store.GetParameterValue<FNiagaraBool>(V).GetValue());}
  else if(V.GetType()==FNiagaraTypeDefinition::GetVec3Def()){Type=TEXT("vector");const auto Value=Store.GetParameterValue<FVector3f>(V);P->SetArrayField(TEXT("value"),{MakeShared<FJsonValueNumber>(Value.X),MakeShared<FJsonValueNumber>(Value.Y),MakeShared<FJsonValueNumber>(Value.Z)});}
  else if(V.GetType()==FNiagaraTypeDefinition::GetColorDef()){Type=TEXT("color");const auto Value=Store.GetParameterValue<FLinearColor>(V);P->SetArrayField(TEXT("value"),{MakeShared<FJsonValueNumber>(Value.R),MakeShared<FJsonValueNumber>(Value.G),MakeShared<FJsonValueNumber>(Value.B),MakeShared<FJsonValueNumber>(Value.A)});}
  P->SetStringField(TEXT("type"),Type);P->SetStringField(TEXT("nativeType"),V.GetType().GetName());Parameters.Add(MakeShared<FJsonValueObject>(P));
 }
 Result->SetArrayField(TEXT("emitters"),Emitters);Result->SetArrayField(TEXT("parameters"),Parameters);return Result;
}
}
bool EmberWorkspace(const FString& RequestFile,const FString& OutputFile,FString& Error){
 FString Text;TSharedPtr<FJsonObject> Request;
 if(!FFileHelper::LoadFileToString(Text,*RequestFile)||Text.Len()>1024*1024||!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text),Request)){Error=TEXT("Invalid workspace request");return false;}
 FString Action;if(!Request->TryGetStringField(TEXT("action"),Action)){Error=TEXT("Missing workspace action");return false;}
 TSharedPtr<FJsonObject> Result=MakeShared<FJsonObject>();
 if(Action==TEXT("assets")||Action==TEXT("surface")||Action==TEXT("scene")){if(!EmberSceneWorkspace(Request,Result,Error))return false;}else if(Action==TEXT("list")){
  auto& Registry=FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();Registry.SearchAllAssets(true);
  FARFilter Filter;Filter.ClassPaths.Add(UNiagaraSystem::StaticClass()->GetClassPathName());Filter.PackagePaths.Add(TEXT("/Game"));Filter.bRecursivePaths=true;TArray<FAssetData> Assets;Registry.GetAssets(Filter,Assets);
  TArray<TSharedPtr<FJsonValue>> Items;for(const auto& A:Assets){auto Item=MakeShared<FJsonObject>();Item->SetStringField(TEXT("name"),A.AssetName.ToString());Item->SetStringField(TEXT("asset"),A.GetSoftObjectPath().ToString());Items.Add(MakeShared<FJsonValueObject>(Item));}Result->SetArrayField(TEXT("systems"),Items);
 }else{
  FString Asset;if(!Request->TryGetStringField(TEXT("asset"),Asset)||!Asset.StartsWith(TEXT("/Game/"))){Error=TEXT("Choose a project Niagara system");return false;}
  auto* System=LoadObject<UNiagaraSystem>(nullptr,*Asset);if(!System){Error=TEXT("Niagara system could not be loaded");return false;}
  if(Action==TEXT("copy")){
   FString Name;if(!Request->TryGetStringField(TEXT("name"),Name)||Name.IsEmpty()||Name.Len()>80){Error=TEXT("Enter a copy name");return false;}for(TCHAR C:Name)if(!FChar::IsAlnum(C)&&C!='_'){Error=TEXT("Use letters, numbers and underscores in copy names");return false;}
   FString Package,AssetName;auto& Tools=FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();Tools.CreateUniqueAssetName(TEXT("/Game/Ember/Systems/")+Name,TEXT(""),Package,AssetName);
   System=Cast<UNiagaraSystem>(Tools.DuplicateAsset(AssetName,FPackageName::GetLongPackagePath(Package),System));if(!System){Error=TEXT("Could not copy Niagara system");return false;}
   const TSharedPtr<FJsonObject>* Changes=nullptr;
   if(Request->TryGetObjectField(TEXT("values"),Changes)){
    auto& Store=System->GetExposedParameters();TArray<FNiagaraVariable> Vars;Store.GetParameters(Vars);
    for(const auto& Entry:(*Changes)->Values){const auto* V=Vars.FindByPredicate([&](const FNiagaraVariable& P){return P.GetName().ToString().Equals(*Entry.Key);});if(!V){Error=TEXT("Unknown exposed parameter");return false;}
     double Number;bool Flag;const TArray<TSharedPtr<FJsonValue>>* Array=nullptr;
     if(V->GetType()==FNiagaraTypeDefinition::GetFloatDef()&&Entry.Value->TryGetNumber(Number)&&FMath::IsFinite(Number)&&FMath::Abs(Number)<=1e9)Store.SetParameterValue(float(Number),*V);
     else if(V->GetType()==FNiagaraTypeDefinition::GetIntDef()&&Entry.Value->TryGetNumber(Number)&&Number>=MIN_int32&&Number<=MAX_int32&&Number==FMath::FloorToDouble(Number))Store.SetParameterValue(int32(Number),*V);
     else if(V->GetType()==FNiagaraTypeDefinition::GetBoolDef()&&Entry.Value->TryGetBool(Flag)){FNiagaraBool B;B.SetValue(Flag);Store.SetParameterValue(B,*V);}
     else if((V->GetType()==FNiagaraTypeDefinition::GetVec3Def()||V->GetType()==FNiagaraTypeDefinition::GetColorDef())&&Entry.Value->TryGetArray(Array)&&Array->Num()==(V->GetType()==FNiagaraTypeDefinition::GetVec3Def()?3:4)){
      TArray<float> N;for(const auto& Item:*Array){if(!Item->TryGetNumber(Number)||!FMath::IsFinite(Number)||FMath::Abs(Number)>1e9){Error=TEXT("Invalid vector or color");return false;}N.Add(float(Number));}if(N.Num()==3)Store.SetParameterValue(FVector3f(N[0],N[1],N[2]),*V);else Store.SetParameterValue(FLinearColor(N[0],N[1],N[2],N[3]),*V);
     }else{Error=TEXT("Unsupported or invalid parameter value");return false;}
    }
   }
   auto* PackageObject=System->GetOutermost();PackageObject->MarkPackageDirty();FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;Args.SaveFlags=SAVE_NoError;
   if(!UPackage::SavePackage(PackageObject,System,*FPackageName::LongPackageNameToFilename(PackageObject->GetName(),FPackageName::GetAssetPackageExtension()),Args)){Error=TEXT("Could not save Niagara copy");return false;}
  }else if(Action!=TEXT("inspect")){Error=TEXT("Unknown workspace action");return false;}
  Result=Describe(System);
 }
 FString Output;FJsonSerializer::Serialize(Result.ToSharedRef(),TJsonWriterFactory<>::Create(&Output));if(!FFileHelper::SaveStringToFile(Output,*OutputFile)){Error=TEXT("Could not write workspace result");return false;}return true;
}
