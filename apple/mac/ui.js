(()=>{
 const bridge=window.emberMac;if(!bridge)return;
 $('.brand small').textContent='MAC STUDIO';
 const report=async promise=>{try{const r=await promise;if(!r.ok)throw Error(r.error);if(!r.canceled)toast('File saved');return r;}catch(e){toast(e.message);return {ok:false};}};
 $('#save').onclick=()=>{persist();return report(bridge.saveProject(emberProject()));};
 importProject=async()=>{try{const r=await bridge.openProject();if(!r.ok)throw Error(r.error);if(r.canceled)return;EmberFormat.validateProject(r.data);emberLoadProject(r.data);if($('#dialog').open)$('#dialog').close();if(layers.some(l=>l.effect.renderer==='mesh'&&!l.effect.meshData))toast('Mesh files remain on the Windows PC; sprite and ribbon layers preview here.');else toast('Project opened');}catch(e){toast(e.message);}};
 snapshot=()=>report(bridge.saveImage(canvas.toDataURL('image/png')));$('#snapshot').onclick=snapshot;$('#open-project').onclick=importProject;
 $('#export').onclick=()=>{dialog('<div class="eyebrow">EMBER FOR MAC</div><h2>Keep creating across devices.</h2><p>Save a complete Ember project or a preview image. Open the project on your Windows PC for Niagara or Blender export.</p><button id="mac-save-project" class="primary">Save Ember project</button><button id="mac-save-image">Save preview image</button><p>External mesh geometry is stored separately on the PC. Existing mesh references and Unreal scene bindings stay in your project.</p>');$('#mac-save-project').onclick=$('#save').onclick;$('#mac-save-image').onclick=snapshot;};
 for(const id of ['blender-menu','unreal-connect','native-workspace','import-particle-mesh','particle-mesh-label'])$('#'+id).hidden=true;
 $('.editor-navigation').hidden=true;
 const panel=$('#advanced-authoring-panel');let hide=false;for(const child of panel.children){if(child.tagName==='H3'&&child.textContent==='Selected layer in Unreal')hide=true;if(hide)child.hidden=true;}panel.querySelector('summary').textContent='Whole effect controls';
 const note=document.createElement('p');note.className='animation-intro';note.textContent='Unreal scene bindings are retained. Edit them and create Niagara assets in Ember on Windows.';panel.append(note);
 $('.brand').onclick=e=>{e.preventDefault();viewport?.home();};
})();
