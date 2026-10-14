const fs=require('fs'),path=require('path'),{spawn}=require('child_process');const root=path.resolve(__dirname,'..');
(async()=>{for(const version of ['5.6','5.8']){
 const projectRoot=path.join(root,'unreal/validation','Portable '+version);fs.mkdirSync(projectRoot,{recursive:true});
 const project=path.join(projectRoot,'Ember Portable.uproject');fs.writeFileSync(project,JSON.stringify({FileVersion:3,EngineAssociation:version,Plugins:[{Name:'EmberNiagara',Enabled:true}]}));
 fs.cpSync(path.join(root,'unreal/packaged',version,'EmberNiagara'),path.join(projectRoot,'Plugins/EmberNiagara'),{recursive:true});
 const effect=path.join(projectRoot,'Burst With Spaces.ember.json');fs.copyFileSync(path.join(root,'unreal/tests/fire-burst.ember.json'),effect);
 const output=await new Promise((resolve,reject)=>{let log='';const exe=`C:/Program Files/Epic Games/UE_${version}/Engine/Binaries/Win64/UnrealEditor-Cmd.exe`;const child=spawn(exe,[`"${project}"`,'-run=EmberImport',`-Effect="${effect}"`,'-unattended','-nop4','-nosplash','-nullrhi'],{windowsHide:true,windowsVerbatimArguments:true,argv0:`"${exe}"`});child.stdout.on('data',d=>log+=d);child.stderr.on('data',d=>log+=d);child.on('error',reject);child.on('exit',code=>{fs.writeFileSync(path.join(projectRoot,'distribution-test.log'),log);if(code===0&&log.includes('EMBER_IMPORT_SUCCESS'))resolve(log);else reject(Error('Unreal '+version+' distribution test failed: '+log.slice(-1500)))})});
 console.log('Unreal '+version+': '+output.match(/EMBER_IMPORT_SUCCESS[^\r\n]*/)[0]);
 }} )().catch(e=>{console.error(e.message);process.exitCode=1});
