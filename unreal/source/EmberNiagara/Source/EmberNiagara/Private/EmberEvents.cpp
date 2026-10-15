#include "EmberEvents.h"
#include "NiagaraEmitter.h"
#include "NiagaraScript.h"
#include "NiagaraScriptSource.h"
#include "NiagaraGraph.h"
#include "NiagaraNodeInput.h"
#include "NiagaraNodeOutput.h"
#include "NiagaraNodeCustomHlsl.h"
#include "NiagaraNodeWriteDataSet.h"
#include "NiagaraNodeReadDataSet.h"
#include "ViewModels/Stack/NiagaraStackGraphUtilities.h"
#include "UObject/UnrealType.h"

bool EmberValidateEvents(const TSharedPtr<FJsonObject>& E,FString& Error){
 if(!E->HasField(TEXT("eventRules")))return true;
 const TArray<TSharedPtr<FJsonValue>>* Rules=nullptr;
 if(!E->TryGetArrayField(TEXT("eventRules"),Rules)||Rules->Num()>2){Error=TEXT("Use up to two event rules");return false;}
 TSet<FString> Seen;
 for(const auto& Value:*Rules){const TSharedPtr<FJsonObject>* R=nullptr;FString Trigger,Burst,Color;bool Flag;
  if(!Value->TryGetObject(R)||!(*R)->TryGetStringField(TEXT("trigger"),Trigger)||!TArray<FString>{TEXT("collision"),TEXT("death")}.Contains(Trigger)||Seen.Contains(Trigger)||!(*R)->TryGetStringField(TEXT("burst"),Burst)||!TArray<FString>{TEXT("sparks"),TEXT("smoke"),TEXT("explosion"),TEXT("custom")}.Contains(Burst)||!(*R)->TryGetStringField(TEXT("color"),Color)||Color.Len()!=7||Color[0]!='#'||!(*R)->TryGetBoolField(TEXT("glow"),Flag)||!(*R)->TryGetBoolField(TEXT("trail"),Flag)){Error=TEXT("Invalid event response");return false;}
  if(Burst==TEXT("custom")){const TSharedPtr<FJsonObject>* Response=nullptr;if(!(*R)->TryGetObjectField(TEXT("response"),Response)||(*Response)->HasField(TEXT("eventRules"))){Error=TEXT("Custom responses cannot chain events");return false;}double N;FString Text;bool B;for(const TCHAR* K:{TEXT("id"),TEXT("color"),TEXT("shape"),TEXT("sprite"),TEXT("mode")})if(!(*Response)->TryGetStringField(K,Text)){Error=TEXT("Invalid custom response");return false;}for(const TCHAR* K:{TEXT("amount"),TEXT("size"),TEXT("speed"),TEXT("spread"),TEXT("life"),TEXT("gravity"),TEXT("turbulence"),TEXT("endSize"),TEXT("endOpacity")})if(!(*Response)->TryGetNumberField(K,N)||!FMath::IsFinite(N)){Error=TEXT("Invalid custom response numbers");return false;}for(const TCHAR* K:{TEXT("glow"),TEXT("trail"),TEXT("bounce")})if(!(*Response)->TryGetBoolField(K,B)){Error=TEXT("Invalid custom response behavior");return false;}}
  Seen.Add(Trigger);for(int32 I=1;I<7;I++)if(!FChar::IsHexDigit(Color[I])){Error=TEXT("Invalid response color");return false;}
  for(const auto& Range:TArray<TTuple<FString,double,double>>{{TEXT("count"),1,32},{TEXT("size"),1,30},{TEXT("speed"),1,100},{TEXT("life"),1,8}}){double N;if(!(*R)->TryGetNumberField(Range.Get<0>(),N)||!FMath::IsFinite(N)||N<Range.Get<1>()||N>Range.Get<2>()||(Range.Get<0>()==TEXT("count")&&N!=FMath::FloorToDouble(N))){Error=TEXT("Invalid event response amount or appearance");return false;}}
 }return true;
}
TSharedPtr<FJsonObject> EmberResponseEffect(const TSharedPtr<FJsonObject>& R,const TSharedPtr<FJsonObject>& Parent){
 const TSharedPtr<FJsonObject>* Response=nullptr;if(R->GetStringField(TEXT("burst"))==TEXT("custom")&&R->TryGetObjectField(TEXT("response"),Response)){auto E=MakeShared<FJsonObject>(**Response);for(const TCHAR* K:{TEXT("animation"),TEXT("eventRules"),TEXT("attachment"),TEXT("surface")})E->RemoveField(K);E->SetStringField(TEXT("sceneCollision"),TEXT("off"));E->SetStringField(TEXT("simulation"),TEXT("cpu"));E->SetStringField(TEXT("mode"),TEXT("burst"));E->SetNumberField(TEXT("amount"),5);for(const TCHAR* K:{TEXT("size"),TEXT("speed"),TEXT("life")})E->SetNumberField(K,R->GetNumberField(K));E->SetStringField(TEXT("color"),R->GetStringField(TEXT("color")));for(const TCHAR* K:{TEXT("glow"),TEXT("trail")})E->SetBoolField(K,R->GetBoolField(K));for(const TCHAR* K:{TEXT("positionX"),TEXT("positionY"),TEXT("positionZ")})E->SetNumberField(K,0);bool Anime=false;Parent->TryGetBoolField(TEXT("anime"),Anime);E->SetBoolField(TEXT("anime"),Anime);for(const TCHAR* K:{TEXT("animeSteps"),TEXT("animeOutline")}){double N=FString(K)==TEXT("animeSteps")?3:8;Parent->TryGetNumberField(K,N);E->SetNumberField(K,N);}return E;}
 auto E=MakeShared<FJsonObject>();const FString Burst=R->GetStringField(TEXT("burst"));const bool Smoke=Burst==TEXT("smoke"),Explosion=Burst==TEXT("explosion");
 for(const auto& Pair:TArray<TPair<FString,FString>>{{TEXT("id"),Smoke?TEXT("smoke"):TEXT("custom")},{TEXT("color"),R->GetStringField(TEXT("color"))},{TEXT("sprite"),TEXT("soft")},{TEXT("mode"),TEXT("burst")},{TEXT("shape"),Explosion?TEXT("sphere"):Smoke?TEXT("point"):TEXT("cone")},{TEXT("motion"),Explosion?TEXT("preset"):TEXT("aimed")}})E->SetStringField(Pair.Key,Pair.Value);
 for(const TCHAR* Key:{TEXT("size"),TEXT("speed"),TEXT("life")})E->SetNumberField(Key,R->GetNumberField(Key));
 for(const auto& Pair:TArray<TPair<FString,double>>{{TEXT("amount"),5},{TEXT("spread"),Explosion?5.:Smoke?20.:35.},{TEXT("velocityYaw"),0},{TEXT("velocityPitch"),90},{TEXT("directionRandom"),Smoke?15.:Explosion?0.:65.},{TEXT("gravity"),Smoke?0.:Explosion?-10.:-45.},{TEXT("turbulence"),Smoke?12.:0.},{TEXT("endSize"),Smoke?3.:.2},{TEXT("endOpacity"),0}})E->SetNumberField(Pair.Key,Pair.Value);
 for(const TCHAR* Key:{TEXT("glow"),TEXT("trail")})E->SetBoolField(Key,R->GetBoolField(Key));E->SetBoolField(TEXT("bounce"),false);
 bool Anime=false;Parent->TryGetBoolField(TEXT("anime"),Anime);E->SetBoolField(TEXT("anime"),Anime);for(const TCHAR* Key:{TEXT("animeSteps"),TEXT("animeOutline")}){double N=FString(Key)==TEXT("animeSteps")?3:8;Parent->TryGetNumberField(Key,N);E->SetNumberField(Key,N);}return E;
}
namespace {
UEdGraphPin* Pin(UEdGraphNode* Node,const TCHAR* Name){for(auto* P:Node->Pins)if(P->PinName==Name)return P;return nullptr;}
void Payload(UNiagaraNodeDataSetBase* Node,FName Event){
 Node->ExternalStructAsset=FindObject<UScriptStruct>(nullptr,TEXT("/Script/Niagara.NiagaraCollisionEventPayload"));
 Node->Variables={FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(),TEXT("CollisionPos")),FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(),TEXT("CollisionNormal")),FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(),TEXT("CollisionVelocity")),FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(),TEXT("ParticleIndex")),FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(),TEXT("PhysicalMaterialIndex"))};Node->DataSet.Name=Event;Node->DataSet.Type=ENiagaraDataSetType::Event;
}
}
UNiagaraScript* EmberEventModule(UNiagaraEmitter* Emitter,FName Event,bool Receive,double CenterZ){
 const FString Name=(Receive?TEXT("Receive_"):TEXT("Generate_"))+Event.ToString();auto* Script=NewObject<UNiagaraScript>(Emitter,FName(*Name),RF_Transactional);Script->SetUsage(ENiagaraScriptUsage::Module);Script->CheckVersionDataAvailable();Script->GetLatestScriptData()->ModuleUsageBitmask=(1<<int32(ENiagaraScriptUsage::ParticleUpdateScript))|(1<<int32(ENiagaraScriptUsage::ParticleEventScript));
 auto* Source=NewObject<UNiagaraScriptSource>(Script);auto* Graph=NewObject<UNiagaraGraph>(Source);Source->NodeGraph=Graph;Script->SetLatestSource(Source);
 FGraphNodeCreator<UNiagaraNodeInput> IC(*Graph);auto* In=IC.CreateNode();In->Usage=ENiagaraInputNodeUsage::Parameter;In->Input=FNiagaraVariable(FNiagaraTypeDefinition::GetParameterMapDef(),TEXT("MapIn"));IC.Finalize();
 FGraphNodeCreator<UNiagaraNodeOutput> OC(*Graph);auto* Out=OC.CreateNode();Out->SetUsage(ENiagaraScriptUsage::Module);Out->Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetParameterMapDef(),TEXT("MapOut")));OC.Finalize();
 FGraphNodeCreator<UNiagaraNodeCustomHlsl> HC(*Graph);auto* H=HC.CreateNode();H->ScriptUsage=ENiagaraScriptUsage::Module;H->Signature.Name=FName(*Name);H->Signature.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetParameterMapDef(),TEXT("MapIn")));H->Signature.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetParameterMapDef(),TEXT("MapOut")));
 FString Code;
 if(Receive){H->Signature.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(),TEXT("EventPosition")));Code=TEXT("MapOut.Particles.EmberResponseOrigin=EventPosition-float3(0,0,")+FString::SanitizeFloat(CenterZ)+TEXT(");MapOut.Particles.Position=MapIn.Particles.Position+MapOut.Particles.EmberResponseOrigin;MapOut.Particles.EmberRenderedPosition=MapOut.Particles.Position*MapIn.Particles.EmberScale;");}
 else {H->Signature.Outputs.Append({FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(),TEXT("Condition")),FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(),TEXT("EventPosition"))});Code=TEXT("Condition=MapIn.Particles.")+(Event==FName(TEXT("EmberCollision"))?FString(TEXT("EmberCollisionEvent")):FString(TEXT("EmberDeathEvent")))+TEXT(">0.5;EventPosition=MapIn.Particles.EmberEventPosition;");}
 FindFProperty<FStrProperty>(H->GetClass(),TEXT("CustomHlsl"))->SetPropertyValue_InContainer(H,Code);HC.Finalize();
 if(Receive){FGraphNodeCreator<UNiagaraNodeReadDataSet> RC(*Graph);auto* Read=RC.CreateNode();Payload(Read,Event);RC.Finalize();In->Pins[0]->MakeLinkTo(Pin(Read,TEXT("__ParamMapIn")));Pin(Read,TEXT("__ParamMapOut"))->MakeLinkTo(Pin(H,TEXT("MapIn")));Pin(Read,TEXT("CollisionPos"))->MakeLinkTo(Pin(H,TEXT("EventPosition")));Pin(H,TEXT("MapOut"))->MakeLinkTo(Out->Pins[0]);}
 else {FGraphNodeCreator<UNiagaraNodeWriteDataSet> WC(*Graph);auto* Write=WC.CreateNode();Payload(Write,Event);Write->EventName=Event;WC.Finalize();for(auto* P:Write->Pins)if(P->Direction==EGPD_Input){P->DefaultValue=P->PinName==TEXT("ParticleIndex")||P->PinName==TEXT("PhysicalMaterialIndex")?TEXT("0"):TEXT("(X=0,Y=0,Z=0)");}In->Pins[0]->MakeLinkTo(Pin(H,TEXT("MapIn")));Pin(H,TEXT("MapOut"))->MakeLinkTo(Pin(Write,TEXT("__ParamMapIn")));Pin(H,TEXT("Condition"))->MakeLinkTo(Pin(Write,TEXT("__Condition")));Pin(H,TEXT("EventPosition"))->MakeLinkTo(Pin(Write,TEXT("CollisionPos")));Pin(Write,TEXT("__ParamMapOut"))->MakeLinkTo(Out->Pins[0]);}
 return Script;
}
bool EmberReceiveEvents(UNiagaraEmitter* Emitter,const FGuid& Source,FName Event,int32 Count,double CenterZ){
 auto* Data=Emitter->GetLatestEmitterData();FNiagaraEventScriptProperties Props;Props.Script=NewObject<UNiagaraScript>(Emitter,NAME_None,RF_Transactional);Props.Script->SetUsage(ENiagaraScriptUsage::ParticleEventScript);Props.Script->SetUsageId(FGuid::NewGuid());Props.Script->SetLatestSource(Data->GraphSource);Props.ExecutionMode=EScriptExecutionMode::SpawnedParticles;Props.SpawnNumber=Count;Props.MaxEventsPerFrame=128;Props.SourceEmitterID=Source;Props.SourceEventName=Event;Emitter->AddEventHandler(Props,Data->Version.VersionGuid);
 UNiagaraGraph* Graph=CastChecked<UNiagaraScriptSource>(Data->GraphSource)->NodeGraph;FGraphNodeCreator<UNiagaraNodeOutput> OC(*Graph);auto* Out=OC.CreateNode();Out->SetUsage(ENiagaraScriptUsage::ParticleEventScript);Out->SetUsageId(Props.Script->GetUsageId());Out->Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetParameterMapDef(),TEXT("Out")));OC.Finalize();FGraphNodeCreator<UNiagaraNodeInput> IC(*Graph);auto* In=IC.CreateNode();In->Input=FNiagaraVariable(FNiagaraTypeDefinition::GetParameterMapDef(),TEXT("InputMap"));In->Usage=ENiagaraInputNodeUsage::Parameter;IC.Finalize();In->Pins[0]->MakeLinkTo(Out->Pins[0]);auto* Module=EmberEventModule(Emitter,Event,true,CenterZ);return Out&&Module&&FNiagaraStackGraphUtilities::AddScriptModuleToStack(Module,*Out);
}
