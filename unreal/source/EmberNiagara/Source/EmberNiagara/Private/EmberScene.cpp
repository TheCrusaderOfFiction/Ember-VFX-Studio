#include "EmberScene.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "StaticMeshResources.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Components/SkinnedMeshComponent.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "EmberEffectActor.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
bool EmberSceneWorkspace(const TSharedPtr<FJsonObject>& Request,TSharedPtr<FJsonObject>& Result,FString& Error){
 const FString Action=Request->GetStringField(TEXT("action"));auto& Registry=FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();Registry.SearchAllAssets(true);
 if(Action==TEXT("assets")){FARFilter Filter;Filter.ClassPaths={UStaticMesh::StaticClass()->GetClassPathName(),USkeletalMesh::StaticClass()->GetClassPathName(),UWorld::StaticClass()->GetClassPathName()};Filter.PackagePaths.Add(TEXT("/Game"));Filter.bRecursivePaths=true;TArray<FAssetData> Assets;Registry.GetAssets(Filter,Assets);TArray<TSharedPtr<FJsonValue>> Values;for(const auto& A:Assets){auto V=MakeShared<FJsonObject>();V->SetStringField(TEXT("name"),A.AssetName.ToString());V->SetStringField(TEXT("asset"),A.GetSoftObjectPath().ToString());V->SetStringField(TEXT("kind"),A.AssetClassPath==UWorld::StaticClass()->GetClassPathName()?TEXT("map"):A.AssetClassPath==USkeletalMesh::StaticClass()->GetClassPathName()?TEXT("skeletal"):TEXT("static"));Values.Add(MakeShared<FJsonValueObject>(V));}Result->SetArrayField(TEXT("assets"),Values);return true;}
 FString Asset;if(!Request->TryGetStringField(TEXT("asset"),Asset)||!Asset.StartsWith(TEXT("/Game/"))){Error=TEXT("Choose a project asset");return false;}
 if(Action==TEXT("scene")){auto* World=LoadObject<UWorld>(nullptr,*Asset);if(!World||!World->PersistentLevel){Error=TEXT("Could not load level actors");return false;}TArray<TSharedPtr<FJsonValue>> Values;for(AActor* Actor:World->PersistentLevel->Actors){if(!Actor||Actor->IsTemplate())continue;auto V=MakeShared<FJsonObject>();V->SetStringField(TEXT("name"),Actor->GetName());V->SetStringField(TEXT("label"),Actor->GetActorLabel());TArray<TSharedPtr<FJsonValue>> Sockets;TInlineComponentArray<USkinnedMeshComponent*> Meshes(Actor);if(Meshes.Num())for(FName Socket:Meshes[0]->GetAllSocketNames())Sockets.Add(MakeShared<FJsonValueString>(Socket.ToString()));V->SetArrayField(TEXT("sockets"),Sockets);Values.Add(MakeShared<FJsonValueObject>(V));}Result->SetArrayField(TEXT("actors"),Values);return true;}
 TArray<FVector3f> Vertices;TArray<uint32> Indices;
 if(auto* Mesh=LoadObject<UStaticMesh>(nullptr,*Asset)){auto* Data=Mesh->GetRenderData();if(Data&&Data->LODResources.Num()){auto& LOD=Data->LODResources[0];const auto& Positions=LOD.VertexBuffers.PositionVertexBuffer;for(uint32 I=0;I<Positions.GetNumVertices();I++)Vertices.Add(Positions.VertexPosition(I));const auto View=LOD.IndexBuffer.GetArrayView();for(int32 I=0;I<View.Num();I++)Indices.Add(View[I]);}}
 else if(auto* CharacterMesh=LoadObject<USkeletalMesh>(nullptr,*Asset)){auto* Data=CharacterMesh->GetResourceForRendering();if(Data&&Data->LODRenderData.Num()){auto& LOD=Data->LODRenderData[0];const auto& Positions=LOD.StaticVertexBuffers.PositionVertexBuffer;for(uint32 I=0;I<Positions.GetNumVertices();I++)Vertices.Add(Positions.VertexPosition(I));const auto* Buffer=LOD.MultiSizeIndexContainer.GetIndexBuffer();if(Buffer)for(int32 I=0;I<Buffer->Num();I++)Indices.Add(Buffer->Get(I));}}
 if(Vertices.IsEmpty()||Indices.Num()<3){Error=TEXT("Mesh has no readable surface. Enable Allow CPU Access on the mesh and save it in Unreal.");return false;}
 TArray<double> Areas;double Total=0;for(int32 I=0;I+2<Indices.Num();I+=3){if(Indices[I]>=uint32(Vertices.Num())||Indices[I+1]>=uint32(Vertices.Num())||Indices[I+2]>=uint32(Vertices.Num())){Error=TEXT("Invalid mesh triangles");return false;}Total+=FVector3f::CrossProduct(Vertices[Indices[I+1]]-Vertices[Indices[I]],Vertices[Indices[I+2]]-Vertices[Indices[I]]).Size()*.5;Areas.Add(Total);}if(Total<=0){Error=TEXT("Mesh surface has zero area");return false;}
 FRandomStream Random(1729);TArray<TSharedPtr<FJsonValue>> Points;for(int32 I=0;I<2048;I++){const double Pick=Random.FRand()*Total;int32 Low=0,High=Areas.Num()-1;while(Low<High){const int32 Mid=(Low+High)/2;if(Areas[Mid]<Pick)Low=Mid+1;else High=Mid;}const float U=FMath::Sqrt(Random.FRand()),V=Random.FRand();const auto P=Vertices[Indices[Low*3]]*(1-U)+Vertices[Indices[Low*3+1]]*(U*(1-V))+Vertices[Indices[Low*3+2]]*(U*V);Points.Add(MakeShared<FJsonValueArray>(TArray<TSharedPtr<FJsonValue>>{MakeShared<FJsonValueNumber>(P.X),MakeShared<FJsonValueNumber>(P.Y),MakeShared<FJsonValueNumber>(P.Z)}));}Result->SetArrayField(TEXT("points"),Points);return true;
}
bool EmberCreateSceneActor(UNiagaraSystem* System,const TSharedPtr<FJsonObject>& Effect,FString& Error){
 const TSharedPtr<FJsonObject>* Attachment=nullptr;if(!Effect->TryGetObjectField(TEXT("attachment"),Attachment))return true;
 FString PackageName,Name;FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get().CreateUniqueAssetName(TEXT("/Game/Ember/Actors/BP_")+System->GetName(),TEXT(""),PackageName,Name);auto* Package=CreatePackage(*PackageName);auto* BP=FKismetEditorUtilities::CreateBlueprint(AEmberEffectActor::StaticClass(),Package,FName(*Name),BPTYPE_Normal,UBlueprint::StaticClass(),UBlueprintGeneratedClass::StaticClass());if(!BP){Error=TEXT("Could not create companion effect actor");return false;}auto* Defaults=Cast<AEmberEffectActor>(BP->GeneratedClass->GetDefaultObject());Defaults->Effect=System;Defaults->TargetActorName=(*Attachment)->GetStringField(TEXT("actor"));Defaults->Socket=FName((*Attachment)->GetStringField(TEXT("socket")));FKismetEditorUtilities::CompileBlueprint(BP);FAssetRegistryModule::AssetCreated(BP);Package->MarkPackageDirty();FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;if(!UPackage::SavePackage(Package,BP,*FPackageName::LongPackageNameToFilename(PackageName,FPackageName::GetAssetPackageExtension()),Args)){Error=TEXT("Could not save companion actor");return false;}UE_LOG(LogTemp,Display,TEXT("EMBER_ACTOR_SUCCESS %s"),*BP->GetPathName());return true;
}
