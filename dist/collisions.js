(function(root,factory){const api=factory();if(typeof module==='object'&&module.exports)module.exports=api;else root.EmberCollisions=api;})(globalThis,()=>{
 const ranges={colliderX:['Obstacle center X',-10000,10000,0],colliderY:['Obstacle center Y',-10000,10000,0],colliderZ:['Obstacle center Z',-10000,10000,100],colliderRadius:['Sphere radius',1,10000,50],colliderSizeX:['Box width X',1,10000,100],colliderSizeY:['Box depth Y',1,10000,100],colliderSizeZ:['Box height Z',1,10000,100]};
 const defaults={colliderType:'plane',...Object.fromEntries(Object.entries(ranges).map(([key,def])=>[key,def[3]]))};
 function validate(s){if(s.colliderType!==undefined&&!['plane','sphere','box'].includes(s.colliderType))throw Error('Invalid collision obstacle');for(const [key,[,min,max]]of Object.entries(ranges))if(s[key]!==undefined&&(!Number.isFinite(s[key])||s[key]<min||s[key]>max))throw Error('Invalid '+key);}
 function resolve(p,s){if(!s.bounce||s.motion==='orbit')return true;const restitution=s.restitution??.65,friction=s.surfaceFriction??.15,type=s.colliderType||'plane';if(type==='plane'){if(p.z>=(s.collisionHeight||0))return true;if(s.collisionResponse==='kill')return false;p.z=2*(s.collisionHeight||0)-p.z;p.vz=Math.abs(p.vz)*restitution;p.vx*=1-friction;p.vy*=1-friction;return true;}
  const cx=s.colliderX??0,cy=s.colliderY??0,cz=s.colliderZ??100,dx=p.x-cx,dy=p.y-cy,dz=p.z-cz;let nx=0,ny=0,nz=0;
  if(type==='sphere'){const radius=s.colliderRadius??50,length=Math.hypot(dx,dy,dz);if(length>=radius)return true;if(s.collisionResponse==='kill')return false;if(length>1e-6){nx=dx/length;ny=dy/length;nz=dz/length;}else nz=1;p.x=cx+nx*radius;p.y=cy+ny*radius;p.z=cz+nz*radius;}
  else{const hx=(s.colliderSizeX??100)/2,hy=(s.colliderSizeY??100)/2,hz=(s.colliderSizeZ??100)/2,px=hx-Math.abs(dx),py=hy-Math.abs(dy),pz=hz-Math.abs(dz);if(px<=0||py<=0||pz<=0)return true;if(s.collisionResponse==='kill')return false;if(px<=py&&px<=pz){nx=dx<0?-1:1;p.x=cx+nx*hx;}else if(py<=pz){ny=dy<0?-1:1;p.y=cy+ny*hy;}else{nz=dz<0?-1:1;p.z=cz+nz*hz;}}
  const normalSpeed=p.vx*nx+p.vy*ny+p.vz*nz;if(normalSpeed<0){const tx=p.vx-normalSpeed*nx,ty=p.vy-normalSpeed*ny,tz=p.vz-normalSpeed*nz;p.vx=tx*(1-friction)-normalSpeed*restitution*nx;p.vy=ty*(1-friction)-normalSpeed*restitution*ny;p.vz=tz*(1-friction)-normalSpeed*restitution*nz;}return true;
 }
 return {ranges,defaults,validate,resolve};
});
