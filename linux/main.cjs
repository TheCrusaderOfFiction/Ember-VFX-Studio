const {app,BrowserWindow,ipcMain,dialog,Menu}=require('electron');
const fs=require('node:fs/promises'),path=require('node:path');
const {validateProject}=require('../desktop/project.cjs');
const {installLifecycle,closeWithWindow}=require('../desktop/lifecycle.cjs');
const root=path.resolve(__dirname,'..');let window;
function handler(name,fn){ipcMain.handle(name,async(event,...args)=>{if(event.sender!==window.webContents)throw Error('Untrusted window');try{return {ok:true,...await fn(...args)}}catch(e){return {ok:false,error:e.message}}});}
const name=value=>String(value||'Effect').replace(/[^a-z0-9_-]/gi,'-').slice(0,80);
handler('mac-save',async data=>{validateProject(data);const r=await dialog.showSaveDialog(window,{title:'Save Ember project',defaultPath:name(data.name)+'.ember.json',filters:[{name:'Ember project',extensions:['json']}]});if(r.canceled)return {canceled:true};await fs.writeFile(r.filePath,JSON.stringify(data,null,2));return {};});
handler('mac-open',async()=>{const r=await dialog.showOpenDialog(window,{properties:['openFile'],filters:[{name:'Ember project',extensions:['json']}]});if(r.canceled)return {canceled:true};if((await fs.stat(r.filePaths[0])).size>16*1024*1024)throw Error('Project is too large (16 MB maximum)');const data=JSON.parse(await fs.readFile(r.filePaths[0],'utf8'));validateProject(data);return {data};});
handler('mac-image',async data=>{if(typeof data!=='string'||!data.startsWith('data:image/png;base64,')||data.length>40e6)throw Error('Invalid preview image');const r=await dialog.showSaveDialog(window,{defaultPath:'Ember-preview.png',filters:[{name:'PNG image',extensions:['png']}]});if(r.canceled)return {canceled:true};await fs.writeFile(r.filePath,Buffer.from(data.slice(22),'base64'));return {};});
if(installLifecycle(app,()=>window))app.whenReady().then(()=>{
 window=new BrowserWindow({width:1420,height:940,minWidth:850,minHeight:650,title:'Ember VFX Studio',backgroundColor:'#17151e',webPreferences:{preload:path.join(__dirname,'preload.cjs'),contextIsolation:true,nodeIntegration:false,sandbox:true}});closeWithWindow(app,window);
 window.webContents.setWindowOpenHandler(()=>({action:'deny'}));window.webContents.on('will-navigate',e=>e.preventDefault());window.loadFile(path.join(root,'dist/index.html'));
 Menu.setApplicationMenu(Menu.buildFromTemplate([{label:'Ember',submenu:[{role:'about'},{type:'separator'},{role:'quit'}]},{label:'File',submenu:[{label:'Open project…',accelerator:'CmdOrCtrl+O',click:()=>window.webContents.executeJavaScript("document.querySelector('#open-project').click()")},{label:'Save project…',accelerator:'CmdOrCtrl+S',click:()=>window.webContents.executeJavaScript("document.querySelector('#save').click()")},{type:'separator'},{role:'close'}]},{role:'editMenu'},{role:'viewMenu'}]));
});
