require('esbuild').buildSync({entryPoints:['src/three-entry.js'],bundle:true,minify:true,format:'iife',globalName:'Ember3D',outfile:'dist/vendor/three.js',legalComments:'eof'});
require('fs').copyFileSync('node_modules/three/LICENSE','dist/vendor/THREE-LICENSE.txt');
