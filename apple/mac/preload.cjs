const {contextBridge,ipcRenderer}=require('electron');
contextBridge.exposeInMainWorld('emberMac',{saveProject:project=>ipcRenderer.invoke('mac-save',project),openProject:()=>ipcRenderer.invoke('mac-open'),saveImage:image=>ipcRenderer.invoke('mac-image',image)});
