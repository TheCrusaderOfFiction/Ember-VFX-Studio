const fs=require('node:fs/promises'),path=require('node:path'),{randomUUID}=require('node:crypto'),{spawn}=require('node:child_process');
const assetPattern=/^\/Game\/[A-Za-z0-9_/]+\.[A-Za-z0-9_]+$/;
function decodeResult(buffer){return JSON.parse((buffer[0]===0xff&&buffer[1]===0xfe?buffer.subarray(2).toString('utf16le'):buffer.toString('utf8')).replace(/^\uFEFF/,''));}
function validateRequest(request){
 if(!request||!['list','inspect','copy'].includes(request.action))throw Error('Unknown editor action.');
 if(request.action!=='list'&&(!assetPattern.test(request.asset)||request.asset.length>1024))throw Error('Choose a Niagara system from your project.');
 if(request.action==='copy'){
  if(typeof request.name!=='string'||! /^[A-Za-z0-9_]{1,80}$/.test(request.name))throw Error('Use letters, numbers and underscores for the copy name.');
  if(!request.values||Array.isArray(request.values)||typeof request.values!=='object'||Object.keys(request.values).length>256)throw Error('Invalid parameter changes.');
  for(const [name,value]of Object.entries(request.values)){if(!name.startsWith('User.')||name.length>256)throw Error('Invalid parameter name.');const validNumber=n=>typeof n==='number'&&Number.isFinite(n)&&n>=-2147483648&&n<=2147483647;if(typeof value!=='boolean'&&!validNumber(value)&&!(Array.isArray(value)&&[3,4].includes(value.length)&&value.every(validNumber)))throw Error('Invalid parameter value.');}
 }
 return request;
}
async function requestWorkspace(connection,request,{userData,transfer,pluginRoot}){
 validateRequest(request);await transfer.ensurePlugin(connection,pluginRoot);const folder=path.join(userData,'NiagaraWorkspace');await fs.mkdir(folder,{recursive:true});const id=randomUUID(),input=path.join(folder,id+'.request.json'),output=path.join(folder,id+'.result.json');
 try{await fs.writeFile(input,JSON.stringify(request));const log=await transfer.runEngine(connection,[`-Workspace="${input}"`,`-WorkspaceOutput="${output}"`]);if(!log.includes('EMBER_WORKSPACE_SUCCESS'))throw Error('Unreal did not confirm the editor operation.');const stat=await fs.stat(output);if(stat.size>16*1024*1024)throw Error('Project inventory is too large.');return decodeResult(await fs.readFile(output));}finally{await Promise.allSettled([fs.unlink(input),fs.unlink(output)]);}
}
async function openSystem(connection,asset){
 if(!assetPattern.test(asset))throw Error('Invalid Niagara asset.');const executable=`C:/Program Files/Epic Games/UE_${connection.version}/Engine/Binaries/Win64/UnrealEditor.exe`;
 await new Promise((resolve,reject)=>{const child=spawn(executable,[connection.project,`-EmberOpenSystem=${asset}`],{detached:true,stdio:'ignore',windowsHide:false});child.once('error',reject);child.once('spawn',()=>{child.unref();resolve();});});
}
module.exports={validateRequest,requestWorkspace,openSystem,decodeResult};
