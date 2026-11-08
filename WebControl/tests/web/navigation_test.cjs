const fs = require('fs'), vm = require('vm'), assert = require('assert');
const html = fs.readFileSync(require('path').join(__dirname, '../../include/WebPage.h'),'utf8');
let script = html.split('<script>')[1].split('</script>')[0].replace(/\s*poll\(\);\s*$/, '');
const node = (extra={}) => ({disabled:false, value:'40', checked:false, dataset:{}, classList:{add(){},remove(){}},setPointerCapture(){}, ...extra});
const ids = Object.fromEntries([...html.matchAll(/id="([^"]+)"/g)].map(x=>[x[1],node()]));
const moves=[...html.matchAll(/<button\b[^>]*data-direction="([^"]+)"[^>]*>/gs)].map(x=>node({dataset:{direction:x[1]}}));
assert.equal(moves.length,8);
const turns=[['90','left'],['90','right'],['180','left'],['180','right'],['360','left'],['360','right']].map(([turn,turnDirection])=>node({dataset:{turn,turnDirection}}));
const events={}, intervals=new Map(), calls=[]; let nextTimer=0;
let state = {minSpeed:50,maxSpeed:100,armed:false,left:0,right:0,buttonPressed:false,buttonCount:0,red:255,green:0,blue:0,turretMin:0,turretMax:180,turretAngle:90,turretActive:false,imu:{valid:true,model:'BMI160',ageMs:0,ax:0,ay:0,az:1,gx:0,gy:0,gz:0},heading:{ready:true,calibrating:false,angle:0,rate:0,status:'bereit'},navigation:{mode:'idle',target:0,error:0,status:'bereit'}};
const ctx={console,URLSearchParams,AbortSignal,Uint8Array,crypto:require('crypto').webcrypto,
 document:{getElementById:id=>{assert(ids[id],id);return ids[id]},querySelectorAll:q=> q.includes('data-color')?[]:[...(q.includes('.move')?moves:[]),...(q.includes('[data-turn]')?turns:[])],addEventListener:(n,f)=>events[n]=f,hidden:false},
 window:{addEventListener:(n,f)=>events[n]=f},setInterval:(fn,ms)=>{assert.equal(ms,150);intervals.set(++nextTimer,fn);return nextTimer},clearInterval:id=>intervals.delete(id),setTimeout:()=>{},
 fetch:async(path,opts)=>{const body=Object.fromEntries(new URLSearchParams(opts.body));calls.push({path,...body});
 if(path==='/api/turn') state={...state,navigation:{mode:'turn',target:90,error:90,status:'dreht'}};
 if(path==='/api/arm') state={...state,armed:body.enabled==='1',navigation:{mode:'idle',target:90,error:0,status:'gestoppt'}};
 return {ok:true,json:async()=>state};}};
vm.createContext(ctx);vm.runInContext(script,ctx);
(async()=>{
 ctx.render(state);assert(turns.every(x=>x.disabled));
 state={...state,armed:true};ctx.render(state);assert(turns.every(x=>!x.disabled));
 await turns[0].onclick();assert.equal(calls[0].path,'/api/turn');assert.equal(calls[0].direction,'left');assert.equal(calls[0].degrees,'90');assert.equal(calls[0].speed,'50');
 assert.equal(intervals.size,1);assert(moves.every(x=>x.disabled));
 const old=[...intervals.values()][0];await old();assert.equal(calls[1].path,'/api/turn/heartbeat');assert(+calls[1].seq>+calls[0].seq);
 // Erfolgreiche Drehung: keine erneute Freigabe, naechste Drehung sofort moeglich.
 state={...state,armed:true,navigation:{mode:'idle',target:90,error:0.2,status:'Drehziel erreicht'}};
 ctx.render(state);assert.equal(intervals.size,0);assert(turns.every(x=>!x.disabled));
 const stopCalls=calls.filter(x=>x.path==='/api/arm').length;
 await turns[4].onclick();assert.equal(intervals.size,1);assert.equal(calls.at(-1).degrees,'360');
 assert.equal(calls.filter(x=>x.path==='/api/arm').length,stopCalls);
 await ids.emergency.onclick();assert.equal(intervals.size,0);assert(turns.every(x=>x.disabled));
 const count=calls.length;await old();assert.equal(calls.length,count);
 state={...state,armed:true,heading:{...state.heading,ready:false}};ctx.render(state);assert(turns.every(x=>x.disabled));
 // Diagonale Taste verwendet dieselbe Halten-/Loslassen-Logik und Stopp-Route.
 state={...state,armed:true,heading:{...state.heading,ready:true}};ctx.render(state);
 const curve=moves.find(b=>b.dataset.direction==='forward-left');
 curve.onpointerdown({isPrimary:true,preventDefault(){},pointerId:1});
 await new Promise(setImmediate);
 assert.equal(calls.at(-1).path,'/api/drive');assert.equal(calls.at(-1).direction,'forward-left');
 await curve.onpointerup();assert.equal(calls.at(-1).path,'/api/stop');assert.equal(intervals.size,0);
 await ids.padStop.onclick();assert.equal(calls.at(-1).path,'/api/arm');assert.equal(calls.at(-1).enabled,'0');
 // Manuelle Fahrantwort darf die Token einer laufenden Aktionsgeneration nicht zuruecksetzen.
 vm.runInContext('motionEpoch = 42; moving = "forward";',ctx);
 await ctx.drive();assert.equal(vm.runInContext('motionEpoch',ctx),42);
 ctx.clearMotion();
 console.log('Browser: calibrated/armed guards, turn request, heartbeat sequence, STOPP and late timer cancellation passed');
})().catch(e=>{console.error(e);process.exitCode=1});
