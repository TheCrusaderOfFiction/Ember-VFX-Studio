#include "EmberMaterial.h"
#include "NiagaraSystem.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionParticleColor.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionVertexNormalWS.h"
#include "Materials/MaterialExpressionCameraVectorWS.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
UMaterial* EmberCreateSpriteMaterial(UNiagaraSystem* System,const FString& LayerName,const FString& Shape,bool Glow,FString& Error,bool Anime,int32 Steps,float Outline){
 const FString PackageName=System->GetOutermost()->GetName()+TEXT("_M_")+LayerName+TEXT("_")+FGuid::NewGuid().ToString(EGuidFormats::Digits).Left(8);auto* Package=CreatePackage(*PackageName);const FName Name(*FPackageName::GetLongPackageAssetName(PackageName));auto* Material=NewObject<UMaterial>(Package,Name,RF_Public|RF_Standalone);
 Material->BlendMode=Glow&&!Anime?BLEND_Additive:BLEND_Translucent;Material->SetShadingModel(MSM_Unlit);Material->TwoSided=true;bool Changed=false;Material->SetMaterialUsage(Changed,MATUSAGE_NiagaraSprites);
 if(Shape==TEXT("ribbon"))Material->SetMaterialUsage(Changed,MATUSAGE_NiagaraRibbons);
 if(Shape==TEXT("mesh"))Material->SetMaterialUsage(Changed,MATUSAGE_NiagaraMeshParticles);
 auto* UV=NewObject<UMaterialExpressionTextureCoordinate>(Material);auto* Mask=NewObject<UMaterialExpressionCustom>(Material);auto* Color=NewObject<UMaterialExpressionParticleColor>(Material);auto* Opacity=NewObject<UMaterialExpressionMultiply>(Material);
 FString Code=Shape==TEXT("trail")?TEXT("float2 q=UV*2.0-1.0;return exp(-q.x*q.x*6.0)*pow(saturate(1.0-UV.y),1.5);"):TEXT("float2 q=UV*2.0-1.0;float r=length(q);\n");
 if(Shape==TEXT("square"))Code+=TEXT("r=max(abs(q.x),abs(q.y));\n");if(Shape==TEXT("diamond"))Code+=TEXT("r=abs(q.x)+abs(q.y);\n");if(Shape==TEXT("star"))Code+=TEXT("r*=1.0+.35*cos(atan2(q.y,q.x)*5.0);\n");
 if(Shape!=TEXT("trail"))Code+=Shape==TEXT("ribbon")?TEXT("return 1.0;"):Shape==TEXT("mesh")?TEXT("return 1.0;"):Shape==TEXT("ring")?TEXT("return smoothstep(.55,.7,r)*(1.0-smoothstep(.8,1.0,r));"):Shape==TEXT("soft")?TEXT("return exp(-r*r*5.0)*saturate(1.0-r);"):TEXT("return 1.0-smoothstep(.8,1.0,r);");
 FString Distance=TEXT("float2 q=UV*2.0-1.0;float r=length(q);");if(Shape==TEXT("square"))Distance+=TEXT("r=max(abs(q.x),abs(q.y));");if(Shape==TEXT("diamond"))Distance+=TEXT("r=abs(q.x)+abs(q.y);");if(Shape==TEXT("star"))Distance+=TEXT("r*=1.0+.35*cos(atan2(q.y,q.x)*5.0);");
 if(Anime&&Shape!=TEXT("mesh")&&Shape!=TEXT("trail")&&Shape!=TEXT("ribbon"))Code=Distance+(Shape==TEXT("ring")?TEXT("return step(.55,r)*step(r,1.0);"):TEXT("return step(r,1.0);"));
 Mask->Code=Code;Mask->OutputType=CMOT_Float1;FCustomInput Input;Input.InputName=TEXT("UV");Input.Input.Expression=UV;Mask->Inputs.Add(Input);Opacity->A.Expression=Mask;Opacity->B.Expression=Color;Opacity->B.OutputIndex=4;
 auto& Expressions=Material->GetExpressionCollection();Expressions.AddExpression(UV);Expressions.AddExpression(Mask);Expressions.AddExpression(Color);Expressions.AddExpression(Opacity);Material->GetEditorOnlyData()->EmissiveColor.Expression=Color;
 if(Anime){
  auto* Toon=NewObject<UMaterialExpressionCustom>(Material);Toon->OutputType=CMOT_Float3;
  for(const auto& Pair:TArray<TPair<FString,UMaterialExpression*>>{{TEXT("UV"),UV},{TEXT("Tint"),Color}}){FCustomInput I;I.InputName=FName(*Pair.Key);I.Input.Expression=Pair.Value;Toon->Inputs.Add(I);}
  if(Shape==TEXT("mesh")){auto* Normal=NewObject<UMaterialExpressionVertexNormalWS>(Material);auto* View=NewObject<UMaterialExpressionCameraVectorWS>(Material);Expressions.AddExpression(Normal);Expressions.AddExpression(View);for(const auto& Pair:TArray<TPair<FString,UMaterialExpression*>>{{TEXT("Normal"),Normal},{TEXT("View"),View}}){FCustomInput I;I.InputName=FName(*Pair.Key);I.Input.Expression=Pair.Value;Toon->Inputs.Add(I);}Toon->Code=FString::Printf(TEXT("float f=abs(dot(normalize(Normal),normalize(View)));float shade=.35+.65*floor(min(f,.9999)*%d.0)/%d.0;return lerp(Tint*shade,float3(.008,.01,.018),1.0-step(%.4f,f));"),Steps,Steps-1,Outline);}
  else if(Shape==TEXT("trail")||Shape==TEXT("ribbon"))Toon->Code=TEXT("return Tint;");
  else Toon->Code=Distance+FString::Printf(TEXT("float shade=.35+.65*floor(clamp(1.0-r,0.0,.9999)*%d.0)/%d.0;float ink=step(%.4f,r);"),Steps,Steps-1,1-Outline)+(Shape==TEXT("ring")?FString::Printf(TEXT("ink=max(ink,1.0-step(%.4f,r));"),.55+Outline):TEXT(""))+TEXT("return lerp(Tint*shade,float3(.008,.01,.018),ink);");
  if(Glow){Toon->Code.ReplaceInline(TEXT("Tint*shade"),TEXT("Tint*shade*3.0"));if(Shape==TEXT("trail")||Shape==TEXT("ribbon"))Toon->Code=TEXT("return Tint*3.0;");}
  Expressions.AddExpression(Toon);Material->GetEditorOnlyData()->EmissiveColor.Expression=Toon;
 }
 Material->GetEditorOnlyData()->Opacity.Expression=Opacity;Material->PostEditChange();FAssetRegistryModule::AssetCreated(Material);Material->MarkPackageDirty();
 FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;const FString File=FPackageName::LongPackageNameToFilename(PackageName,FPackageName::GetAssetPackageExtension());if(!UPackage::SavePackage(Package,Material,*File,Args)){Error=TEXT("Could not save particle material.");return nullptr;}return Material;
}
