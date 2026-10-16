(()=>{
 const standalone=()=>matchMedia('(display-mode: standalone)').matches||navigator.standalone;
 const banner=document.createElement('section');banner.className='apple-install';banner.innerHTML='<span>Install Ember: in Safari, tap Share → Add to Home Screen. Keep “Open as Web App” enabled if shown.</span><button aria-label="Dismiss install instructions">×</button>';document.querySelector('header').after(banner);banner.hidden=!!standalone();banner.querySelector('button').onclick=()=>banner.hidden=true;
 if('serviceWorker'in navigator){navigator.serviceWorker.register('./sw.js').then(async()=>{await navigator.serviceWorker.ready;document.body.dataset.offlineReady='true';}).catch(()=>{const notice=document.createElement('p');notice.textContent='Offline setup failed. Reopen Ember while connected before using it offline.';notice.className='apple-install';document.querySelector('header').after(notice);});}
 document.addEventListener('visibilitychange',()=>{if(document.hidden)window.emberMobilePause?.();else window.emberMobileResume?.();});
 window.addEventListener('pagehide',()=>{try{persist();}catch{}});
})();
