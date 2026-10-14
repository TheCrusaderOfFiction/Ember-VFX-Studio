const native=window.emberDesktop;
const projectData=()=>emberProject();
function applyProject(data){emberLoadProject(data)}
if(native){
  document.querySelector('.brand small').textContent='DESKTOP STUDIO';
  $('#save').onclick=async()=>{const r=await native.saveProject(projectData());if(r.ok&&!r.canceled){persist();toast('Project saved')}else if(!r.ok)toast(r.error)};
  importProject=async()=>{const r=await native.openProject();if(r.ok&&!r.canceled){applyProject(r.data);$('#dialog').close();toast('Project opened')}else if(!r.ok)toast(r.error)};
  snapshot=async()=>{const r=await native.saveImage(canvas.toDataURL('image/png'));if(r.ok&&!r.canceled)toast('Preview image saved');else if(!r.ok)toast(r.error)};
  $('#snapshot').onclick=snapshot;
  $('#help').onclick=()=>dialog('<div class="eyebrow">EMBER DESKTOP</div><h2>Create. Adjust. Send.</h2><p>Choose a preset, then adjust its color, density, size, movement, spread, and lifetime.</p><p><b>Save project</b> keeps an editable file. <b>Export effect</b> sends your effect to an Unreal project or saves a PNG.</p><p>Niagara exports use 3D sprite emitters. Glow becomes emissive color; trails become stretched, velocity-aligned sprites. Drag to orbit, scroll to zoom, and right-drag to pan around the 3D preview. The preview is an approximation of the Niagara result.</p><button id="import">Open a saved project</button>');
  $('#export').onclick=async()=>{
    const r=await native.unrealStatus();
    dialog('<div class="eyebrow">YOUR PARTICLE CREATION</div><h2>Put your effect to work.</h2><p>Save the complete particle project to keep all layers and controls. Send all visible layers to Unreal as editable sprite emitters, including spawn shapes, gravity, turbulence, bounce, and lifetime changes. Trails use stretched sprites.</p><label>Unreal version<select id="engine-version"><option value="5.6">Unreal 5.6</option><option value="5.8">Unreal 5.8</option></select></label><p id="engine-status"></p><button id="niagara-send" class="primary">Send full effect to Unreal…</button><button id="plugin-folder">Get editor plugin</button><hr><button id="native-json">Save Ember project</button><button id="native-png">Save preview image</button><p id="export-status" role="status"></p>');
    $('#engine-status').textContent=r.ok?'Detected: '+(r.engines.map(e=>'Unreal '+e.version).join(', ')||'No supported Unreal installation'):'Could not detect Unreal';
    $('#niagara-send').disabled=!layers.some(l=>l.enabled);$('#niagara-send').onclick=async()=>{const b=$('#niagara-send');b.disabled=true;$('#export-status').textContent='Choose your project. Creating Niagara assets can take a few minutes…';const result=await native.exportNiagara(projectData(),$('#engine-version').value);b.disabled=false;$('#export-status').textContent=result.canceled?'Export canceled.':result.ok?'Created '+result.asset:result.error};
    $('#plugin-folder').onclick=async()=>{const result=await native.showPlugin($('#engine-version').value);$('#export-status').textContent=result.ok?'Copy this EmberNiagara folder into your project’s Plugins folder. Restart Unreal, then use Tools → Import Ember VFX.':result.error};
    $('#native-json').onclick=async()=>{const result=await native.saveProject(projectData());$('#export-status').textContent=result.canceled?'Save canceled.':result.ok?'Project saved.':result.error};
    $('#native-png').onclick=snapshot;
  };
}
