(()=>{
 if(!window.EmberAndroid)return;
 document.documentElement.classList.add('ember-android');
 const requests=new Map();let next=0,current='preview',resumePlaying=false;
 window.emberAndroidReply=(id,result)=>{const resolve=requests.get(id);if(resolve){requests.delete(id);resolve(result);}};
 function request(action,data={}){return new Promise(resolve=>{const id=String(++next);requests.set(id,resolve);try{EmberAndroid.request(JSON.stringify({id,action,...data}));}catch(e){requests.delete(id);resolve({ok:false,error:e.message});}});}
 async function saveBlob(blob,name){if(blob.size>16*1024*1024)throw Error('File is too large (16 MB maximum)');const bytes=new Uint8Array(await blob.arrayBuffer());let binary='';for(let i=0;i<bytes.length;i+=32768)binary+=String.fromCharCode(...bytes.subarray(i,i+32768));return request('save',{name,mime:blob.type,base64:btoa(binary)});}
 window.emberSaveMeshBlob=(blob,name)=>saveBlob(blob,name);
 async function saveProject(){try{persist();const result=await saveBlob(new Blob([JSON.stringify(emberProject(),null,2)],{type:'application/json'}),filename()+'.ember.json');if(!result.ok)throw Error(result.error);if(!result.canceled)toast('Project saved — open this file in Ember on your PC too');}catch(e){toast(e.message);}}
 importProject=async()=>{try{const result=await request('open');if(!result.ok)throw Error(result.error);if(result.canceled)return;const project=JSON.parse(result.text);EmberFormat.validateProject(project);emberLoadProject(project);if($('#dialog').open)$('#dialog').close();show('preview');toast('Project opened');}catch(e){toast(e.message||'Invalid Ember project');}};
 download=async(blob,name)=>{try{const result=await saveBlob(blob,name);if(!result.ok)throw Error(result.error);if(!result.canceled)toast('File saved');}catch(e){toast(e.message);}};
 snapshot=async()=>{try{const blob=await new Promise(resolve=>canvas.toBlob(resolve,'image/png'));if(!blob)throw Error('Could not capture the preview');const result=await saveBlob(blob,filename()+'.png');if(!result.ok)throw Error(result.error);if(!result.canceled)toast('Preview image saved');}catch(e){toast(e.message);}};
 $('#save').onclick=saveProject;$('#snapshot').onclick=snapshot;$('#open-project').onclick=importProject;
 $('#export').textContent='Share / export';$('#export').onclick=()=>{dialog('<div class="eyebrow">EMBER FOR ANDROID</div><h2>Keep creating anywhere.</h2><p>Save an editable project and move it to your PC for Niagara or Blender export.</p><button id="mobile-save-project" class="primary">Save Ember project</button><button id="mobile-save-image">Save preview image</button><p>Projects include all layers, animation and embedded PNG textures. Imported mesh geometry is stored separately on the desktop and is not included in a project file.</p>');$('#mobile-save-project').onclick=saveProject;$('#mobile-save-image').onclick=snapshot;};
 $('.brand small').textContent='ANDROID';$('.camera-hint').textContent='One finger: orbit · Pinch: zoom · Two fingers: pan';
 for(const option of $('#speed').options)option.textContent=option.value+'×';
 for(const id of ['blender-menu','unreal-connect','native-workspace','import-particle-mesh','particle-mesh-label'])$('#'+id).hidden=true;
 $('.editor-navigation').hidden=true;
 // Keep desktop-only scene controls out of the phone workflow while retaining saved values.
 const advanced=$('#advanced-authoring-panel');let desktopOnly=false;
 for(const child of [...advanced.children]){if(child.tagName==='H3'&&child.textContent==='Selected layer in Unreal')desktopOnly=true;if(desktopOnly)child.hidden=true;}
 advanced.querySelector('summary').textContent='Whole effect controls';
 const sceneNote=document.createElement('p');sceneNote.className='animation-intro';sceneNote.textContent='Unreal scene bindings are kept in your project. Edit them in Ember on your PC.';advanced.append(sceneNote);
 
 const nav=document.createElement('nav');nav.className='mobile-tabs';nav.setAttribute('aria-label','Ember panels');
 for(const [id,label]of [['presets','Presets & layers'],['preview','Preview'],['controls','Controls']]){const button=document.createElement('button');button.textContent=label;button.dataset.mobilePanel=id;button.onclick=()=>show(id);nav.append(button);}document.body.append(nav);
 function show(panel){current=panel;document.body.dataset.mobilePanel=panel;for(const button of nav.children)button.setAttribute('aria-pressed',String(button.dataset.mobilePanel===panel));requestAnimationFrame(()=>window.dispatchEvent(new Event('resize')));}
 $('.brand').onclick=event=>{event.preventDefault();show('preview');viewport?.home();};
 const previousLoad=emberLoadProject;emberLoadProject=function(...args){const result=previousLoad(...args);if(layers.some(l=>l.effect.renderer==='mesh'&&!l.effect.meshData))toast('Mesh files stay on the PC; sprite and ribbon layers preview here.');return result;};
 window.emberMobilePause=()=>{try{persist();}catch{}resumePlaying=playing;if(playing){playing=false;$('#play').textContent='▶';$('#play').setAttribute('aria-label','Play preview');}};
 window.emberMobileResume=()=>{if(resumePlaying){playing=true;$('#play').textContent='Ⅱ';$('#play').setAttribute('aria-label','Pause preview');resumePlaying=false;}};
 window.emberMobileBack=()=>{if($('#dialog').open){$('#dialog').close();return true;}if(current!=='preview'){show('preview');return true;}return false;};
 show('preview');
 window.dispatchEvent(new Event('ember-mobile-ready'));
})();
