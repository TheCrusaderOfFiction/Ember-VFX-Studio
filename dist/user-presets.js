// Saved presets contain ordinary Ember projects, so every layer keeps its settings.
let userPresets=[];
try{const saved=JSON.parse(localStorage.getItem('ember-user-presets'));if(Array.isArray(saved))userPresets=saved.slice(0,100).filter(item=>{try{if(typeof item.id!=='string'||typeof item.name!=='string'||!item.name.trim()||item.name.length>80||item.project?.version!==2)return false;EmberFormat.validateProject(item.project);return true;}catch{return false;}});}catch{}
const clonePreset=value=>JSON.parse(JSON.stringify(value));
function usePresetLayer(source){state=EmberCatalog.normalize(clonePreset(source.effect));layers[activeLayer].effect=state;window.resetSelectedLayer?.();sync();persist();toast('Preset applied to '+layers[activeLayer].name);}
function addAllPresetLayers(item){if(layers.length+item.project.layers.length>8){toast('This preset needs '+item.project.layers.length+' free layers (eight maximum)');return false;}stashLayer();const first=layers.length;for(const source of item.project.layers){let name=source.name,suffix=2;while(layers.some(layer=>layer.name===name))name=source.name+' '+suffix++;layers.push({...clonePreset(source),name,particles:[],budget:0,time:0});}activeLayer=first;state=layers[first].effect;particles=[];sync();persist();toast(item.name+' added');return true;}
function openPresetOnActiveLayer(item){
 if(item.project.layers.length===1){usePresetLayer(item.project.layers[0]);return;}
 dialog('<h2>Choose a preset layer</h2><p id="preset-layer-hint"></p><label>Particle layer<select id="preset-layer-choice" aria-label="Preset particle layer"></select></label><button id="apply-preset-layer" class="primary">Use on active layer</button><button id="add-all-preset-layers">＋ Add all layers to effect</button><p id="preset-layer-capacity"></p><button id="cancel-preset-layer">Cancel</button>');
 $('#preset-layer-hint').textContent='Choose particles from '+item.name+' to use on '+layers[activeLayer].name+'.';
 for(const [index,layer] of item.project.layers.entries()){const option=document.createElement('option');option.value=index;option.textContent=layer.name+' · '+layer.effect.name;$('#preset-layer-choice').append(option);}
 const canAdd=layers.length+item.project.layers.length<=8;$('#add-all-preset-layers').disabled=!canAdd;$('#preset-layer-capacity').textContent=canAdd?'Adds '+item.project.layers.length+' new layers alongside your existing particles.':'This preset needs '+item.project.layers.length+' free layers. You have '+(8-layers.length)+' available (eight maximum).';$('#add-all-preset-layers').onclick=()=>{if(addAllPresetLayers(item))$('#dialog').close();};
 $('#cancel-preset-layer').onclick=()=>$('#dialog').close();
 $('#apply-preset-layer').onclick=()=>{const source=item.project.layers[Number($('#preset-layer-choice').value)];if(source){$('#dialog').close();usePresetLayer(source);}};
}
function storeUserPresets(next){try{localStorage.setItem('ember-user-presets',JSON.stringify(next));userPresets=next;list();return true;}catch{toast('Could not save presets. Local storage may be full.');return false;}}
window.refreshSavedPresetCapacity=()=>document.querySelectorAll('.saved-preset-add').forEach(button=>button.disabled=layers.length+Number(button.dataset.layerCount)>8);
const builtInPresetList=list;
list=()=>{
 builtInPresetList();const host=$('#presets');
 const matches=[...EmberExamplePresets,...userPresets].filter(item=>(filter==='all'||filter===(item.builtIn?'examples':'mine'))&&item.name.toLowerCase().includes($('#search').value.toLowerCase()));
 if(matches.length&&host.querySelector('p'))host.replaceChildren();
 for(const item of matches){
  const card=document.createElement('div');card.className='preset-card saved-preset-card';if(item.builtIn)card.dataset.exampleId=item.id;
  const use=document.createElement('button');use.className='preset saved-preset';use.title='Use on active layer';use.setAttribute('aria-label','Use preset '+item.name);
  const thumb=document.createElement('span');thumb.className='thumb';thumb.textContent='✦';thumb.style.setProperty('--c',item.project.layers[0].effect.color);
  const text=document.createElement('span'),name=document.createElement('b'),tag=document.createElement('small');name.textContent=item.name;tag.textContent=(item.builtIn?'EXAMPLE PRESET · ':'MY PRESET · ')+item.project.layers.length+' '+(item.project.layers.length===1?'LAYER':'LAYERS');text.append(name,tag);use.append(thumb,text);
  use.onclick=()=>openPresetOnActiveLayer(item);
  const actions=document.createElement('div');actions.className='saved-preset-actions';
  const add=document.createElement('button');add.className='preset-add saved-preset-add';add.dataset.layerCount=item.project.layers.length;add.textContent=item.project.layers.length>1?'＋ Add all layers':'＋ Add to effect';add.setAttribute('aria-label','Add '+item.name+' to effect');
  add.onclick=()=>addAllPresetLayers(item);
  const remove=document.createElement('button');remove.className='delete-preset';remove.textContent='×';remove.title='Delete saved preset';remove.setAttribute('aria-label','Delete preset '+item.name);
  remove.onclick=()=>{dialog('<h2>Delete saved preset?</h2><p id="delete-preset-name"></p><p>Your current effect stays as it is.</p><button id="confirm-delete-preset">Delete preset</button><button id="cancel-delete-preset">Keep preset</button>');$('#delete-preset-name').textContent=item.name;$('#cancel-delete-preset').onclick=()=>$('#dialog').close();$('#confirm-delete-preset').onclick=()=>{if(storeUserPresets(userPresets.filter(saved=>saved.id!==item.id))){$('#dialog').close();toast('Preset deleted');}};};
  actions.append(add);if(!item.builtIn)actions.append(remove);card.append(use,actions);host.append(card);
 }
 window.refreshSavedPresetCapacity();
};
// The original search handler captured the old function before this extension loaded.
$('#search').oninput=()=>list();
$('#save-preset').onclick=()=>{
 dialog('<h2>Save your own preset</h2><p>Keep a reusable copy of your particle settings and animations.</p><form id="preset-form"><label>Preset name<input id="preset-name" maxlength="80" required aria-label="Preset name"></label><label>What to save<select id="preset-scope" aria-label="What to save"><option value="effect">Whole effect — all layers</option><option value="layer">Selected particle layer</option></select></label><p id="preset-error" role="alert"></p><button type="submit" class="primary">Save preset</button><button type="button" id="cancel-preset">Cancel</button></form>');
 $('#preset-name').value=$('#name').value==='Untitled effect'?state.name:$('#name').value;$('#preset-name').focus();$('#preset-name').select();$('#cancel-preset').onclick=()=>$('#dialog').close();
 $('#preset-form').onsubmit=event=>{event.preventDefault();const name=$('#preset-name').value.trim();if(!name){$('#preset-error').textContent='Give your preset a name.';return;}if(userPresets.length>=100){$('#preset-error').textContent='You have 100 presets. Delete one to make room.';return;}
  const project=clonePreset(emberProject());project.name=name;if($('#preset-scope').value==='layer'){project.layers=[{...project.layers[activeLayer],enabled:true}];project.effect=clonePreset(state);}
  try{EmberFormat.validateProject(project);}catch(error){$('#preset-error').textContent=error.message;return;}
  if(storeUserPresets([...userPresets,{id:crypto.randomUUID(),name,project}])){$('#dialog').close();$('#search').value='';filter='mine';document.querySelectorAll('[data-filter]').forEach(button=>button.classList.toggle('active',button.dataset.filter==='mine'));list();toast(name+' saved to My presets');}
 };
};
list();
