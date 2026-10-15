const animation=require('../dist/animation.js'),catalog=require('../dist/catalog.js'),format=require('../dist/project-format.js'),particles=require('../dist/particles.js');
function bakeProject(project,{duration=5,fps=24,maxSamples=250000}={}){
 format.validateProject(project);if(!Number.isFinite(duration)||duration<1||duration>20||!Number.isInteger(fps)||fps<1||fps>30)throw Error('Choose 1–20 seconds and 1–30 frames per second.');
 let seed=1729;const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed/4294967296;};
 const layers=(project.layers||[{name:'Layer 1',enabled:true,effect:project.effect}]).filter(layer=>layer.enabled).map(layer=>({name:layer.name,effect:catalog.normalize(layer.effect),particles:[],frames:[]}));if(!layers.length)throw Error('Enable a particle layer first.');
 let samples=0;const linear=value=>value<=.04045?value/12.92:Math.pow((value+.055)/1.055,2.4);
 for(let frame=0;frame<=Math.ceil(duration*fps);frame++)for(const layer of layers){animation.update(layer,frame?1/fps:0,true,random,false);const settings=animation.evaluate(layer.effect,layer.time||0);const points=layer.particles.map(p=>{const t=Math.min(p.age/p.life,1),tail=particles.trailEnd(p,settings),size=p.size*(settings.size/(p.birthSize||settings.size))*(1+((settings.endSize??1)-1)*t)/100,alpha=Math.min(t*8,1)*(1-t*(1-(settings.endOpacity??0)))*(settings.opacity??1);const shade=animation.particleColor(settings,t),color=[1,3,5].map(index=>linear(parseInt(shade.slice(index,index+2),16)/255));return [p.x/100,p.y/100,p.z/100,size,p.rotation||0,...color,alpha,tail.x/100,tail.y/100,tail.z/100];});samples+=points.length;if(samples>maxSamples)throw Error('This bake is too large. Use a shorter duration, fewer particles, or a lower frame rate.');layer.frames.push(points);}
 return {format:'ember-blender-bake',version:1,fps,duration,project,layers:layers.map(({name,effect,frames})=>({name,effect,frames})),samples};
}
module.exports={bakeProject};
