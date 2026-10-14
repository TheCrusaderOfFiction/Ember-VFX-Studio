// Close the whole desktop app when its main window closes. A second launch
// focuses the existing app instead of leaving another instance running.
function installLifecycle(app,getWindow,{schedule=setTimeout}={}){
 if(!app.requestSingleInstanceLock()){app.quit();return false;}
 app.on('second-instance',()=>{const window=getWindow();if(!window||window.isDestroyed())return;if(window.isMinimized())window.restore();window.show();window.focus();});
 let exitTimer;
 app.on('before-quit',()=>{if(!exitTimer){exitTimer=schedule(()=>app.exit(0),5000);exitTimer.unref?.();}});
 app.on('will-quit',()=>{const window=getWindow();if(window&&!window.isDestroyed())window.webContents.session.flushStorageData();});
 app.on('window-all-closed',()=>app.quit());
 return true;
}
function closeWithWindow(app,window){
 // Flush browser storage while the renderer and its session still exist.
 window.on('close',()=>window.webContents.session.flushStorageData());
 window.on('closed',()=>app.quit());
}
module.exports={installLifecycle,closeWithWindow};
