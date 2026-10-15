(function(root,factory){const node=typeof module==='object'&&module.exports;const api=factory(node?require('./catalog.js'):root.EmberCatalog,node?require('./event-rules.js'):root.EmberEventRules,node?require('./motion-path.js'):root.EmberMotionPath);if(node)module.exports=api;else root.EmberExamplePresets=api;})(globalThis,(catalog,events,paths)=>{
 const clone=value=>JSON.parse(JSON.stringify(value));
 const make=(key,name,id,changes)=>{const effect={...clone(catalog.presets.find(p=>p.id===id)),...changes,name};return {id:'example-'+key,name,builtIn:true,thumbnail:'assets/examples/'+key+'.svg',project:{format:'ember-vfx',version:2,name,effect:clone(effect),layers:[{name,enabled:true,effect}]}};};
 return [
  make('impact','Impact responses','custom',{motion:'aimed',shape:'point',velocityPitch:-90,amount:5,speed:100,life:5,positionZ:100,gravity:0,trail:true,bounce:true,colliderType:'plane',collisionHeight:0,collisionResponse:'kill',eventRules:[events.rule('collision'),events.rule('death','smoke')]}),
  make('fireball','Fireball path','fire',paths.recipe({...clone(catalog.presets.find(p=>p.id==='fire')),motion:'aimed',velocityYaw:0,velocityPitch:0,speed:1,spread:5,turbulence:0,gravity:0,life:2,amount:65,size:7,trail:true,trailMode:'ribbon',ribbonWidth:12,ribbonTaper:1,positionX:-150,positionZ:80},'arc')),
  make('ribbon','Flowing ribbon','custom',{amount:70,spread:5,size:1,life:4,motion:'aimed',velocityPitch:45,speed:65,windY:35,gravity:-25,turbulence:0,trail:true,trailMode:'ribbon',ribbonWidth:10,ribbonTaper:1,endOpacity:0}),
  make('obstacle','Sphere deflection','custom',{motion:'aimed',shape:'point',speed:100,life:6,spread:30,positionX:-150,positionZ:52,trail:true,trailLength:2,bounce:true,colliderType:'sphere',colliderZ:60,colliderRadius:50,color:'#54d8ff'}),
  make('tornado','Tornado','vortex',{shape:'cone',motion:'preset',speed:30,windZ:70,vortexForce:260,attraction:80,forceZ:100,drag:.4,life:6,size:6,gravity:0}),
  make('black-hole','Black hole','galaxy',{shape:'sphere',motion:'preset',speed:12,vortexForce:280,attraction:500,forceZ:100,drag:.6,life:6,size:3,endSize:.1,trail:true}),
  make('smoke','Drifting smoke','smoke',{windX:65,windZ:25,drag:.8,colorOverLife:true,endColor:'#323b50'}),
  make('ricochet','Ricochet sparks','meteor',{bounce:true,collisionResponse:'bounce',collisionHeight:0,restitution:.85,surfaceFriction:.05,rotationSpeed:240,colorOverLife:true,endColor:'#ec3b16'}),
  make('ring','Spinning ring','custom',{shape:'ring',motion:'orbit',ringSpin:90,amount:100,life:4,trail:true}),
  make('dissolve','Grow & dissolve','smoke',{lifetime:{size:[{time:0,value:.1},{time:.35,value:1.5},{time:1,value:3}],opacity:[{time:0,value:0},{time:.12,value:1},{time:.6,value:.7},{time:1,value:0}]}}),
  make('gradient','Hot spark gradient','meteor',{lifetime:{color:[{time:0,value:'#ffffff'},{time:.2,value:'#ffe388'},{time:.6,value:'#ff7518'},{time:1,value:'#530917'}]}})
 ];
});
