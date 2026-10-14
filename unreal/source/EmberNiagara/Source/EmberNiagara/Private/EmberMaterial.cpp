#include "EmberMaterial.h"
#include "NiagaraSystem.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionParticleColor.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
UMaterial* EmberCreateSpriteMaterial(UNiagaraSystem* System,const FString& LayerName,const FString& Shape,bool Glow,FString& Error){
 const FString PackageName=System->GetOutermost()->GetName()+TEXT("_M_")+LayerName+TEXT("_")+FGuid::NewGuid().ToString(EGuidFormats::Digits).Left(8);auto* Package=CreatePackage(*PackageName);const FName Name(*FPackageName::GetLongPackageAssetName(PackageName));auto* Material=NewObject<UMaterial>(Package,Name,RF_Public|RF_Standalone);
 Material->BlendMode=Glow?BLEND_Additive:BLEND_Translucent;Material->SetShadingModel(MSM_Unlit);Material->TwoSided=true;bool Changed=false;Material->SetMaterialUsage(Changed,MATUSAGE_NiagaraSprites);
 auto* UV=NewObject<UMaterialExpressionTextureCoordinate>(Material);auto* Mask=NewObject<UMaterialExpressionCustom>(Material);auto* Color=NewObject<UMaterialExpressionParticleColor>(Material);auto* Opacity=NewObject<UMaterialExpressionMultiply>(Material);
 FString Code=Shape==TEXT("trail")?TEXT("float2 q=UV*2.0-1.0;return exp(-q.x*q.x*6.0)*pow(saturate(1.0-UV.y),1.5);"):TEXT("float2 q=UV*2.0-1.0;float r=length(q);\n");
 if(Shape==TEXT("square"))Code+=TEXT("r=max(abs(q.x),abs(q.y));\n");if(Shape==TEXT("diamond"))Code+=TEXT("r=abs(q.x)+abs(q.y);\n");if(Shape==TEXT("star"))Code+=TEXT("r*=1.0+.35*cos(atan2(q.y,q.x)*5.0);\n");
 if(Shape!=TEXT("trail"))Code+=Shape==TEXT("ring")?TEXT("return smoothstep(.55,.7,r)*(1.0-smoothstep(.8,1.0,r));"):Shape==TEXT("soft")?TEXT("return exp(-r*r*5.0)*saturate(1.0-r);"):TEXT("return 1.0-smoothstep(.8,1.0,r);");
 Mask->Code=Code;Mask->OutputType=CMOT_Float1;FCustomInput Input;Input.InputName=TEXT("UV");Input.Input.Expression=UV;Mask->Inputs.Add(Input);Opacity->A.Expression=Mask;Opacity->B.Expression=Color;Opacity->B.OutputIndex=4;
 auto& Expressions=Material->GetExpressionCollection();Expressions.AddExpression(UV);Expressions.AddExpression(Mask);Expressions.AddExpression(Color);Expressions.AddExpression(Opacity);Material->GetEditorOnlyData()->EmissiveColor.Expression=Color;Material->GetEditorOnlyData()->Opacity.Expression=Opacity;Material->PostEditChange();FAssetRegistryModule::AssetCreated(Material);Material->MarkPackageDirty();
 FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;const FString File=FPackageName::LongPackageNameToFilename(PackageName,FPackageName::GetAssetPackageExtension());if(!UPackage::SavePackage(Package,Material,*File,Args)){Error=TEXT("Could not save particle material.");return nullptr;}return Material;
}
