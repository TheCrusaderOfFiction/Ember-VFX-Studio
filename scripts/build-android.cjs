const fs=require('fs'),path=require('path'),os=require('os'),cp=require('child_process'),crypto=require('crypto');
const root=path.resolve(__dirname,'..'),debug=process.argv.includes('--debug'),win=process.platform==='win32';
const sdk=process.env.ANDROID_HOME||process.env.ANDROID_SDK_ROOT||path.join(os.homedir(),'AppData/Local/Android/Sdk');
const highest=dir=>fs.readdirSync(dir).sort((a,b)=>a.localeCompare(b,undefined,{numeric:true})).at(-1);
const tools=path.join(sdk,'build-tools',highest(path.join(sdk,'build-tools'))),platform=path.join(sdk,'platforms',highest(path.join(sdk,'platforms')),'android.jar');
const javaHome=process.env.JAVA_HOME||path.dirname(path.dirname(cp.execFileSync(win?'where.exe':'which',['java'],{encoding:'utf8'}).trim().split(/\r?\n/)[0]));
const javaTool=name=>path.join(javaHome,'bin',name+(win?'.exe':''));
const build=path.join(root,'.openai/android-build',debug?'debug':'release'),assets=path.join(build,'assets'),classes=path.join(build,'classes'),dex=path.join(build,'dex');
// Clean only this generated output tree, after checking the resolved workspace boundary.
if(!build.startsWith(path.join(root,'.openai/android-build')+path.sep))throw Error('Unsafe Android output path');
fs.rmSync(build,{recursive:true,force:true});for(const dir of [assets,classes,dex])fs.mkdirSync(dir,{recursive:true});
fs.cpSync(path.join(root,'dist'),assets,{recursive:true});
let html=fs.readFileSync(path.join(assets,'index.html'),'utf8').replace('</head>','<link rel="stylesheet" href="mobile.css"></head>').replace('</body>','<script src="mobile.js"></script></body>');
fs.writeFileSync(path.join(assets,'index.html'),html);for(const f of ['mobile.js','mobile.css'])fs.copyFileSync(path.join(root,'android',f),path.join(assets,f));
const version=JSON.parse(fs.readFileSync(path.join(root,'package.json'),'utf8')).version,parts=version.split('.').map(Number);if(parts.length!==3||parts.some(n=>!Number.isInteger(n)||n<0)||parts[1]>=100||parts[2]>=100)throw Error('Android needs a numeric major.minor.patch version with minor/patch below 100');
const versionCode=parts[0]*10000+parts[1]*100+parts[2];
let manifest=fs.readFileSync(path.join(root,'android/AndroidManifest.xml'),'utf8').replace(/android:versionCode="[^"]+"/,'android:versionCode="'+versionCode+'"').replace(/android:versionName="[^"]+"/,'android:versionName="'+version+'"');if(debug)manifest=manifest.replace('package="com.embervfx.studio"','package="com.embervfx.studio.qa"').replace('android:debuggable="false"','android:debuggable="true"').replace('android:label="Ember VFX"','android:label="Ember QA"');
const manifestFile=path.join(build,'AndroidManifest.xml');fs.writeFileSync(manifestFile,manifest);
const env={...process.env,JAVA_HOME:javaHome};
function run(exe,args){const r=cp.spawnSync(exe,args,{cwd:root,env,encoding:'utf8',windowsHide:true,maxBuffer:8*1024*1024});if(r.status!==0)throw Error((r.stdout||'')+(r.stderr||'')+(r.error?.message||''));if(r.stderr&&/warning|error/i.test(r.stderr))process.stderr.write(r.stderr);}
const tool=name=>path.join(tools,name+(win?'.exe':''));
run(tool('aapt2'),['compile','--dir',path.join(root,'android/res'),'-o',path.join(build,'resources.zip')]);
const unsigned=path.join(build,'unsigned.apk');
run(tool('aapt2'),['link','-I',platform,'--manifest',manifestFile,'-A',assets,'-o',unsigned,path.join(build,'resources.zip')]);
// Windows AAPT2 can emit backslashes in nested asset entry names. Android assets use '/'.
const zip=fs.readFileSync(unsigned);let end=zip.length-22;while(end>=0&&zip.readUInt32LE(end)!==0x06054b50)end--;if(end<0)throw Error('APK ZIP directory missing');
let directory=zip.readUInt32LE(end+16);for(let i=0;i<zip.readUInt16LE(end+10);i++){if(zip.readUInt32LE(directory)!==0x02014b50)throw Error('Invalid APK ZIP entry');const length=zip.readUInt16LE(directory+28),local=zip.readUInt32LE(directory+42);for(let j=0;j<length;j++)if(zip[directory+46+j]===92){zip[directory+46+j]=47;zip[local+30+j]=47;}directory+=46+length+zip.readUInt16LE(directory+30)+zip.readUInt16LE(directory+32);}fs.writeFileSync(unsigned,zip);
run(javaTool('javac'),['-encoding','UTF-8','-source','8','-target','8','-bootclasspath',platform+path.delimiter+path.join(tools,'core-lambda-stubs.jar'),'-d',classes,path.join(root,'android/src/com/embervfx/studio/MainActivity.java')]);
function files(dir,suffix){return fs.readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?files(path.join(dir,e.name),suffix):e.name.endsWith(suffix)?[path.join(dir,e.name)]:[]);}
run(javaTool('java'),['-cp',path.join(tools,'lib/d8.jar'),'com.android.tools.r8.D8','--release','--min-api','28','--lib',platform,'--output',dex,...files(classes,'.class')]);
run(javaTool('jar'),['uf',unsigned,'-C',dex,'classes.dex']);
const aligned=path.join(build,'aligned.apk');run(tool('zipalign'),['-f','4',unsigned,aligned]);
const signing=path.join(root,'.openai/android-signing');fs.mkdirSync(signing,{recursive:true});
const passwordFile=path.join(signing,'password.txt'),key=path.join(signing,'ember-release.jks');
if(!fs.existsSync(passwordFile)){if(fs.existsSync(key))throw Error('Signing password is missing; restore the signing backup');fs.writeFileSync(passwordFile,crypto.randomBytes(32).toString('hex'),{mode:0o600});}
env.EMBER_ANDROID_STORE_PASSWORD=fs.readFileSync(passwordFile,'utf8').trim();
if(!fs.existsSync(key))run(javaTool('keytool'),['-genkeypair','-keystore',key,'-alias','ember','-keyalg','RSA','-keysize','3072','-validity','10000','-dname','CN=Ember VFX Studio','-storepass:env','EMBER_ANDROID_STORE_PASSWORD','-keypass:env','EMBER_ANDROID_STORE_PASSWORD']);
const output=path.join(root,'release-android');fs.mkdirSync(output,{recursive:true});
const apk=path.join(output,debug?'Ember-Android-QA.apk':'Ember-Android.apk');
run(javaTool('java'),['-jar',path.join(tools,'lib/apksigner.jar'),'sign','--ks',key,'--ks-key-alias','ember','--ks-pass','env:EMBER_ANDROID_STORE_PASSWORD','--key-pass','env:EMBER_ANDROID_STORE_PASSWORD','--out',apk,aligned]);
run(javaTool('java'),['-jar',path.join(tools,'lib/apksigner.jar'),'verify','--verbose',apk]);
const hash=crypto.createHash('sha256').update(fs.readFileSync(apk)).digest('hex');fs.writeFileSync(apk+'.sha256',hash+'  '+path.basename(apk)+'\n');
console.log('Signed '+(debug?'QA':'release')+' APK: '+apk+' ('+(fs.statSync(apk).size/1024/1024).toFixed(1)+' MB)');
