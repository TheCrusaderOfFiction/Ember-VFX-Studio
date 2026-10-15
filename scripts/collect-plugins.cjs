const fs=require('fs'),path=require('path');const root=path.resolve(__dirname,'..');
for(const version of ['5.6','5.8']){
 const destination=path.join(root,'unreal/packaged',version,'EmberNiagara');
 const candidates=[path.join(root,'unreal/validation',version,'Plugins/EmberNiagara/Binaries/Win64'),path.join(root,'unreal/build',version+'-host/HostProject/Plugins/EmberNiagara/Binaries/Win64'),path.join(destination,'Binaries/Win64')];
 const names=['UnrealEditor-EmberNiagara.dll','UnrealEditor-EmberRuntime.dll','UnrealEditor.modules'];
 const binarySource=candidates.find(folder=>names.every(name=>fs.existsSync(path.join(folder,name))));
 if(!binarySource)throw Error('Build EmberNiagara for Unreal '+version+' first. No compiled plugin binaries were found.');
 fs.mkdirSync(path.join(destination,'Binaries/Win64'),{recursive:true});
 fs.cpSync(path.join(root,'unreal/source/EmberNiagara/Source'),path.join(destination,'Source'),{recursive:true});
 for(const name of names){const target=path.join(destination,'Binaries/Win64',name),source=path.join(binarySource,name);if(source!==target)fs.copyFileSync(source,target);}
 const descriptor=JSON.parse(fs.readFileSync(path.join(root,'unreal/source/EmberNiagara/EmberNiagara.uplugin')));
 descriptor.EngineVersion=version+'.0';descriptor.Installed=true;
 fs.writeFileSync(path.join(destination,'EmberNiagara.uplugin'),JSON.stringify(descriptor,null,2));
 console.log('Bundled plugin: Unreal '+descriptor.EngineVersion);
}
