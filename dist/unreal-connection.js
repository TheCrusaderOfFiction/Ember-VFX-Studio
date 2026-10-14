let connectedUnrealProject=null;
function updateUnrealConnection(){const button=$('#unreal-connect');button.textContent=connectedUnrealProject?'Unreal · '+connectedUnrealProject.name:'Connect Unreal project';button.title=connectedUnrealProject?connectedUnrealProject.project:'Choose a .uproject file inside Ember';}
async function showUnrealConnection(){
 if(!native){toast('Project connection is available in the Ember desktop app.');return;}
 const result=await native.getUnrealConnection();if(result.ok)connectedUnrealProject=result.connection;updateUnrealConnection();
 dialog('<h2>Connect Unreal project</h2><p>Choose your project here. Ember will remember it and send effects there.</p><p id="unreal-project-current"></p><label>Unreal version<select id="connect-unreal-version" aria-label="Connection Unreal version"><option value="5.8">Unreal 5.8</option><option value="5.6">Unreal 5.6</option></select></label><button id="choose-unreal-project" class="primary">Choose project…</button><button id="disconnect-unreal-project">Disconnect</button><p id="connection-message" role="status"></p>');
 $('#unreal-project-current').textContent=connectedUnrealProject?connectedUnrealProject.name+' · Unreal '+connectedUnrealProject.version+' · '+connectedUnrealProject.project:'No project connected.';
 $('#connect-unreal-version').value=connectedUnrealProject?.version||'5.8';$('#disconnect-unreal-project').disabled=!connectedUnrealProject;
 $('#choose-unreal-project').onclick=async()=>{const button=$('#choose-unreal-project');button.disabled=true;try{const result=await native.connectUnrealProject($('#connect-unreal-version').value);if(result.ok&&!result.canceled){connectedUnrealProject=result.connection;updateUnrealConnection();$('#dialog').close();toast('Connected to '+connectedUnrealProject.name);}else $('#connection-message').textContent=result.canceled?'Project selection canceled.':result.error;}finally{button.disabled=false;}};
 $('#disconnect-unreal-project').onclick=async()=>{const result=await native.disconnectUnrealProject();if(result.ok){connectedUnrealProject=null;updateUnrealConnection();$('#dialog').close();toast('Unreal project disconnected');}else $('#connection-message').textContent=result.error;};
}
$('#unreal-connect').onclick=showUnrealConnection;
if(native)native.getUnrealConnection().then(result=>{if(result.ok)connectedUnrealProject=result.connection;updateUnrealConnection();});
const connectedExport=$('#export').onclick;
$('#export').onclick=async()=>{await connectedExport();const version=$('#engine-version');if(version&&connectedUnrealProject){version.value=connectedUnrealProject.version;$('#engine-status').textContent='Connected: '+connectedUnrealProject.name+' · Unreal '+connectedUnrealProject.version;}};
