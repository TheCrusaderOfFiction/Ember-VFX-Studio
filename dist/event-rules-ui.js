(()=>{
 const panel=document.createElement('details');panel.id='event-rules-panel';panel.className='advanced';panel.open=true;
 panel.innerHTML='<summary>Particle event rules</summary><p class="animation-intro">When a particle hits or dies, create a burst right there.</p><div class="animation-actions"><button id="add-collision-rule">＋ On first collision</button><button id="add-death-rule">＋ On death</button><button id="event-rules-help" aria-label="Event rules help">?</button></div><p id="event-rules-hint" class="animation-intro"></p><div id="event-rules-list"></div>';
 $('#animation-panel').before(panel);
 function commit(rules){try{EmberEventRules.validate(rules);state.eventRules=rules;window.resetSelectedLayer();persist();refresh();}catch(error){toast(error.message);}}
 function refresh(){const rules=state.eventRules||[],host=$('#event-rules-list');host.replaceChildren();for(const trigger of ['collision','death'])$('#add-'+trigger+'-rule').disabled=rules.some(r=>r.trigger===trigger);
  $('#event-rules-hint').textContent=rules.some(r=>r.trigger==='collision')&&(!state.bounce||state.motion==='orbit')?'Collision rule needs Collide with a surface enabled and movement other than ring orbit.':'Responses run once per source particle. Restart to replay bursts. Responses do not trigger more responses.';
  rules.forEach((rule,index)=>{const card=document.createElement('div');card.className='event-rule-card';const title=document.createElement('b');title.textContent=rule.trigger==='collision'?'On first collision → burst':'On particle death → burst';card.append(title);
   const change=(key,value)=>{const next=structuredClone(state.eventRules);next[index][key]=value;commit(next);};
   const preset=selectInput(Object.keys(EmberEventRules.presets).map(id=>[id,id[0].toUpperCase()+id.slice(1)]),rule.burst,'Response style');preset.onchange=()=>{const next=structuredClone(state.eventRules);next[index]={...EmberEventRules.rule(rule.trigger,preset.value),count:rule.count};commit(next);};field(card,'Response style',preset);
   for(const [key,label,min,max,step]of [['count','Particles per event',1,32,1],['size','Response size',1,30,.1],['speed','Response speed',1,100,1],['life','Response lifetime (seconds)',1,8,.1]]){const input=numberInput(rule[key],min,max,step,label);input.onchange=()=>{if(input.checkValidity())change(key,input.valueAsNumber);};field(card,label,input);}
   const color=document.createElement('input');color.type='color';color.value=rule.color;color.setAttribute('aria-label','Response color');color.onchange=()=>change('color',color.value);field(card,'Response color',color);
   for(const key of ['glow','trail']){const input=document.createElement('input');input.type='checkbox';input.checked=rule[key];input.onchange=()=>change(key,input.checked);field(card,'Response '+key,input).closest('label').className='switch-label';}
   const remove=document.createElement('button');remove.textContent='Remove rule';remove.onclick=()=>{const next=structuredClone(state.eventRules);next.splice(index,1);commit(next);};card.append(remove);host.append(card);
  });
 }
 for(const trigger of ['collision','death'])$('#add-'+trigger+'-rule').onclick=()=>commit([...(state.eventRules||[]),EmberEventRules.rule(trigger,trigger==='death'?'smoke':'sparks')]);
 $('#event-rules-help').onclick=()=>openDocumentation('event-rules');
 const previous=sync;sync=()=>{previous();refresh();};const oldBounce=$('#advanced-bounce').oninput;$('#advanced-bounce').oninput=event=>{oldBounce(event);refresh();};refresh();
})();
