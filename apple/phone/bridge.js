(()=>{
 let busy=false;
 const reply=(id,result)=>window.emberAndroidReply(String(id),result);
 window.EmberAndroid={request:async raw=>{
  const request=JSON.parse(raw),{id,action}=request;if(busy){reply(id,{ok:false,error:'Finish the current file choice first.'});return;}busy=true;
  try{
   if(action==='open'){
    const input=document.createElement('input');input.type='file';input.accept='.json,application/json';input.hidden=true;document.body.append(input);
    let finished=false;const finish=result=>{if(finished)return;finished=true;input.remove();busy=false;reply(id,result);};
    input.oncancel=()=>finish({ok:true,canceled:true});input.onchange=async()=>{try{const file=input.files?.[0];if(!file){finish({ok:true,canceled:true});return;}if(file.size>16*1024*1024)throw Error('Project is too large (16 MB maximum)');finish({ok:true,text:await file.text()});}catch(e){finish({ok:false,error:e.message});}};
    input.click();return;
   }
   if(action!=='save')throw Error('Unsupported file action');
   if(!['application/json','image/png','model/gltf-binary'].includes(request.mime))throw Error('Unsupported file type');
   const binary=atob(request.base64);if(binary.length>16*1024*1024)throw Error('File is too large');const bytes=Uint8Array.from(binary,c=>c.charCodeAt(0));const file=new File([bytes],request.name,{type:request.mime});
   let shared=false;if(navigator.canShare?.({files:[file]})&&navigator.share){try{await navigator.share({files:[file],title:request.name});shared=true;}catch(e){if(e.name==='AbortError')throw e;}}
   if(!shared){const url=URL.createObjectURL(file),link=document.createElement('a');link.href=url;link.download=request.name;link.rel='noopener';document.body.append(link);link.click();link.remove();setTimeout(()=>URL.revokeObjectURL(url),60000);}reply(id,{ok:true});
  }catch(e){reply(id,e.name==='AbortError'?{ok:true,canceled:true}:{ok:false,error:e.message});}finally{if(action!=='open')busy=false;}
 }};
})();
