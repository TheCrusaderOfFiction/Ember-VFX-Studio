(function(root,factory){const api=factory();if(typeof module==='object'&&module.exports)module.exports=api;else root.EmberRibbons=api})(typeof window==='object'?window:globalThis,()=>{
 function validate(s){if(s.trailMode!==undefined&&!['streak','ribbon'].includes(s.trailMode))throw Error('Invalid trail mode');for(const [key,min,max]of [['ribbonWidth',.1,100],['ribbonTaper',0,1]])if(s[key]!==undefined&&(!Number.isFinite(s[key])||s[key]<min||s[key]>max))throw Error('Invalid '+key);}
 function width(s,t){return (s.ribbonWidth??8)*(1-(s.ribbonTaper??1)*Math.max(0,Math.min(1,t)));}
 return {validate,width};
});
