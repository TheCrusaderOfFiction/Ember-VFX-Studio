(function(root,factory){const api=factory();if(typeof module==='object'&&module.exports)module.exports=api;else root.EmberParticles=api})(typeof window==='object'?window:globalThis,()=>{
  function spawn(settings,random=Math.random){
    const a=random()*Math.PI*2,u=random()*2-1,r=settings.spread/100,speed=settings.speed,shape=settings.shape||(settings.id==='magic'?'ring':settings.id==='snow'?'box':'point');
    const planar=Math.sqrt(1-u*u);let x=Math.cos(a)*r*random()*30,y=Math.sin(a)*r*random()*30,z=8,vx=Math.cos(a)*speed*r*.6,vy=Math.sin(a)*speed*r*.6,vz=speed*(1+random());
    if(shape==='sphere'){const radius=r*100*Math.cbrt(random());x=Math.cos(a)*planar*radius;y=Math.sin(a)*planar*radius;z=100+u*radius;vx=Math.cos(a)*planar*speed;vy=Math.sin(a)*planar*speed;vz=u*speed;}
    if(shape==='cone'){vx=Math.cos(a)*speed*r;vy=Math.sin(a)*speed*r;vz=speed*(1+random());}
    if(shape==='ring'){const radius=30+r*95;x=Math.cos(a)*radius;y=Math.sin(a)*radius;z=8;vx=Math.cos(a)*speed;vy=Math.sin(a)*speed;vz=settings.id==='shockwave'?0:speed*.3;if(settings.id==='magic'){y=(random()-.5)*12;z=125+Math.sin(a)*radius;vx=Math.cos(a)*speed*.35;vy=(random()-.5)*speed*.25;vz=Math.sin(a)*speed*.35;}}
    if(shape==='box'){x=(random()-.5)*r*650;y=(random()-.5)*r*650;z=350;}
    if(settings.id==='cosmic'&&shape==='point'){vx=Math.cos(a)*planar*speed*r*2;vy=Math.sin(a)*planar*speed*r*2;vz=u*speed*r*2+20;z=70;}
    if(['snow','rain','leaves','petals'].includes(settings.id)){vz=-speed*2;vx*=.3;vy*=.3;}
    if(settings.id==='smoke'){vx*=.35;vy*=.35;vz=speed*(.7+random()*.5);}
    if(settings.motion==='aimed'){const yaw=(settings.velocityYaw||0)*Math.PI/180,pitch=(settings.velocityPitch||0)*Math.PI/180;vx=speed*Math.cos(pitch)*Math.cos(yaw);vy=speed*Math.cos(pitch)*Math.sin(yaw);vz=speed*Math.sin(pitch);}
    if(settings.motion==='orbit'){const radius=30+r*95,omega=(settings.ringSpin??90)*Math.PI/180;x=Math.cos(a)*radius;y=Math.sin(a)*radius;z=8;vx=-y*omega;vy=x*omega;vz=0;}
    if(shape==='ring'||settings.motion==='orbit'){const factor=1+(random()*2-1)*(settings.radiusVariation||0)/100;x*=factor;if(settings.motion!=='orbit'&&settings.id==='magic'){z=125+(z-125)*factor;}else y*=factor;}
    const sx=settings.emitScaleX??1,sy=settings.emitScaleY??1,sz=settings.emitScaleZ??1;
    const center=settings.motion==='orbit'?8:shape==='sphere'?100:shape==='box'?350:shape==='ring'&&settings.id==='magic'?125:shape==='point'&&settings.id==='cosmic'?70:8;
    x*=sx;y*=sy;z=center+(z-center)*sz;
    if(settings.motion==='orbit'||!(shape==='sphere'||shape==='ring'&&settings.id==='magic'))z+=(random()-.5)*r*30*Math.max(sz-1,0);
    const spinJitter=random()*2-1,rotation=(random()*2-1)*(settings.spriteRotationRandom||0)*Math.PI/180;
    if(settings.motion!=='orbit'&&(settings.directionRandom||0)>0){const yaw=(random()*2-1)*settings.directionRandom*Math.PI/180,pitch=(random()*2-1)*settings.directionRandom*Math.PI/180;const px=vx*Math.cos(pitch)-vz*Math.sin(pitch),pz=vx*Math.sin(pitch)+vz*Math.cos(pitch);vx=px*Math.cos(yaw)-vy*Math.sin(yaw);vy=px*Math.sin(yaw)+vy*Math.cos(yaw);vz=pz;}
    if(settings.motion==='orbit'){const omega=(settings.ringSpin??90)*(1+spinJitter*(settings.spinVariation||0)/100)*Math.PI/180;vx=-y*omega*sx/sy;vy=x*omega*sy/sx;vz=0;}
    x+=settings.positionX||0;y+=settings.positionY||0;z+=settings.positionZ||0;
    return {rotation,spinJitter,orbitScaleX:sx,orbitScaleY:sy,orbitZ:z-(settings.positionZ||0),orbitCx:settings.positionX||0,orbitCy:settings.positionY||0,x,y,z,vx,vy,vz,age:0,life:settings.life*(.5+random()*.5),size:settings.size*(.4+random()*.8),seed:random()*Math.PI*2};
  }
  function step(p,dt,settings){p.age+=dt;if(settings.motion==='orbit'){const cx=settings.positionX||0,cy=settings.positionY||0,angle=(settings.ringSpin??90)*(1+(p.spinJitter||0)*(settings.spinVariation||0)/100)*Math.PI/180*dt,sx=settings.emitScaleX??1,sy=settings.emitScaleY??1,x=(p.x-(p.orbitCx??cx))/(p.orbitScaleX??sx),y=(p.y-(p.orbitCy??cy))/(p.orbitScaleY??sy),c=Math.cos(angle),s=Math.sin(angle);p.x=cx+(x*c-y*s)*sx;p.y=cy+(x*s+y*c)*sy;p.z=(p.orbitZ??8)+(settings.positionZ||0);const omega=(settings.ringSpin??90)*(1+(p.spinJitter||0)*(settings.spinVariation||0)/100)*Math.PI/180;p.vx=-(p.y-cy)*omega*sx/sy;p.vy=(p.x-cx)*omega*sy/sx;p.vz=0;p.orbitCx=cx;p.orbitCy=cy;p.orbitScaleX=sx;p.orbitScaleY=sy;return p.age<p.life;}p.vz+=(settings.gravity||0)*dt;p.x+=p.vx*dt;p.y+=p.vy*dt;p.z+=p.vz*dt;const turbulence=settings.turbulence??(['fire','smoke','ember'].includes(settings.id)?12:0);p.vx+=Math.sin(p.age*3+p.seed)*dt*turbulence;p.vy+=Math.cos(p.age*2+p.seed)*dt*turbulence;if(settings.id==='vortex'||settings.id==='galaxy'){const radius=Math.hypot(p.x-(settings.positionX||0),p.y-(settings.positionY||0))||1;p.vx+=-(p.y-(settings.positionY||0))/radius*settings.speed*dt;p.vy+=(p.x-(settings.positionX||0))/radius*settings.speed*dt;}if(settings.bounce&&p.z<0){p.z=-p.z;p.vz=Math.abs(p.vz)*.65;p.vx*=.85;p.vy*=.85;}return p.age<p.life;}
  function trailEnd(p,settings){const length=.15*(settings.trailLength??1);return {x:p.x-p.vx*length,y:p.y-p.vy*length,z:p.z-p.vz*length};}
  return {spawn,step,trailEnd};
});
