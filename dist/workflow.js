let history=[],future=[],lastProject=JSON.stringify(emberProject()),restoring=false;
const workflowPersist=persist;persist=()=>{const current=JSON.stringify(emberProject());if(!restoring&&current!==lastProject){history.push(lastProject);if(history.length>60)history.shift();future=[];lastProject=current;}workflowPersist();updateHistoryButtons();};
function updateHistoryButtons(){$('#undo').disabled=!history.length;$('#redo').disabled=!future.length;$('#layer-name').value=layers[activeLayer].name;}
function restoreSnapshot(snapshot){restoring=true;try{emberLoadProject(JSON.parse(snapshot));lastProject=JSON.stringify(emberProject());}finally{restoring=false;}updateHistoryButtons();}
$('#undo').onclick=()=>{if(!history.length)return;future.push(JSON.stringify(emberProject()));restoreSnapshot(history.pop());};
$('#redo').onclick=()=>{if(!future.length)return;history.push(JSON.stringify(emberProject()));restoreSnapshot(future.pop());};
$('#layer-name').onchange=e=>{layers[activeLayer].name=e.target.value.trim().slice(0,80)||'Unnamed layer';persist();};
$('#open-project').onclick=()=>importProject();
$('#new-project').onclick=()=>{emberLoadProject({format:'ember-vfx',version:2,name:'Untitled effect',effect:{...EmberCatalog.presets.find(p=>p.id==='custom')},layers:[{name:'Layer 1',enabled:true,effect:{...EmberCatalog.presets.find(p=>p.id==='custom')}}]});};
const workflowSync=sync;sync=()=>{workflowSync();updateHistoryButtons();};updateHistoryButtons();
window.addEventListener('keydown',e=>{const input=e.target.closest('input,textarea,select');if((input&&!['range','checkbox'].includes(input.type))||!e.ctrlKey||e.key.toLowerCase()!=='z')return;e.preventDefault();if(e.shiftKey)$('#redo').click();else $('#undo').click();});
