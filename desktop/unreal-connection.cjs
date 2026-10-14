const fs=require('node:fs/promises'),path=require('node:path');
async function inspectProject(filename,fallbackVersion='5.8'){
 if(typeof filename!=='string'||path.extname(filename).toLowerCase()!=='.uproject')throw Error('Choose an Unreal .uproject file.');
 const project=await fs.realpath(filename),stat=await fs.stat(project);if(stat.size>1024*1024)throw Error('Project description is too large.');
 const config=JSON.parse(await fs.readFile(project,'utf8'));
 const association=String(config.EngineAssociation||'');
 const version=['5.6','5.8'].includes(association)?association:fallbackVersion;
 if(!['5.6','5.8'].includes(version))throw Error('Choose Unreal 5.6 or 5.8.');
 if(association&&!['5.6','5.8'].includes(association))throw Error('This Ember build connects to installed Unreal 5.6 or 5.8 projects.');
 if(config.Plugins?.some(plugin=>plugin.Name==='Niagara'&&plugin.Enabled===false))throw Error('Enable Niagara in this Unreal project first.');
 return {project,name:path.basename(project,'.uproject'),version};
}
function connectionStore(userData){const filename=path.join(userData,'UnrealConnection.json');return {
 async load(){try{const saved=JSON.parse(await fs.readFile(filename,'utf8'));return await inspectProject(saved.project,saved.version);}catch(error){if(error.code==='ENOENT')return null;return null;}},
 async save(connection){await fs.mkdir(userData,{recursive:true});const temporary=filename+'.tmp';await fs.writeFile(temporary,JSON.stringify(connection,null,2));await fs.rename(temporary,filename);},
 async clear(){await fs.rm(filename,{force:true});}
};}
module.exports={inspectProject,connectionStore};
