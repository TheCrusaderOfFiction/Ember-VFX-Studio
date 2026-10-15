# Ember VFX Studio — current desktop release (1.20)

Open **release-v1.2/Ember-win32-x64/Ember.exe**.

## Collision and death response bursts

Open **Particle event rules**. Add **On first collision** or **On death**, choose Sparks, Smoke or Explosion, and set its count, size, speed, lifetime, color, Glow and Trail. Responses appear where the source particle hits or dies. **Add impact example** adds a separate layer without replacing your effect.

Collision needs **Collide with a surface** enabled and works with the floor, sphere and box. Each source particle reports its first contact once; death also includes collision removal. Timeline Clear, Restart and layer deletion do not trigger responses. Responses inherit the source anime look and do not trigger more responses. Rules save in projects, presets and recovery. Burst layers with rules play once; use Restart to replay them.

Unreal 5.6 and 5.8 exports use live CPU Niagara event generators and response emitters. Blender bakes response particles into separate layers. Preview response pools are limited to 4,000 particles each; Niagara handles up to 128 events per frame per rule. This first version offers preset-based response bursts, not arbitrary nested layer graphs or scene collision sampling.

## Visual emitter motion paths

Open **Emitter motion path** for straight, arc and closed routes. Edit waypoint times and position offsets, or enable **Drag waypoints in preview**, choose the X/Y, X/Z or Y/Z plane and drag the handles. Hold Shift to snap to 5 cm. The guide shows the route and current emitter center. **Add fireball path example** adds an independent layer. Routes use straight segments, play once and hold at the end. Restart replays them. Preset/aimed emission leaves older particles behind; locked ring motion carries its orbit with the center.

Routes reuse ordinary position animation keys, so existing save, preset, Blender and Niagara workflows carry them. Editing a route replaces position keys only. Other properties and particle events stay. Each point uses three of the timeline’s 64 keys, with up to 21 waypoints. **Remove path, keep start** removes movement and keeps the first position.

## Curves, textures and ribbons

- **Lifetime curves & color gradient** edits two to eight stops for particle size, opacity and color, from birth to death. Grow & dissolve and Hot spark gradient provide starting points.
- **Particle textures & flipbooks** imports a PNG or animation sheet, with rows, columns, frame rate and looping. Images are embedded in projects and presets; use PNGs up to 2048 × 2048 and 256 KB.
- **Curved ribbons** connects a layer’s particles in birth order, with width and age taper. Add flowing ribbon creates a separate example layer. Particle lifetime controls stream length; this is separate from straight velocity streak length.

All three work in the 3D preview and export to Unreal 5.6/5.8 and Blender. Niagara uses native ribbon renderers and saved texture/material assets. Blender saves animated mesh frames and packed images; textured planes and ribbons use a fixed viewing direction, so they do not rotate to follow a moved Blender camera. These additions are the first stage of the Niagara roadmap. Arbitrary event-driven layer graphs, Unreal actor attachments, character surface emission, scene collisions and builder GPU simulation remain to be implemented.

## Native Niagara editor workspace

Choose **Project Niagara systems**, connect your Unreal project, and click **Read project effects**. Save and close the project in Unreal before reading or writing from Ember. Select an effect to inspect its emitters, CPU/GPU simulation, renderers, and exposed controls. Edit numbers, integers, switches, vectors, and linear RGBA colors, then **Save a new system**. Copies retain native Niagara graphs and dependencies; originals stay unchanged. **Open full editor & preview** launches Unreal with the selected Niagara system.

This workspace preserves advanced Niagara features in native assets. Full graph authoring and native simulation still use Unreal's editor. Ember does not yet offer a standalone replacement for all Niagara authoring tools. Native systems stay in the connected project and are separate from Ember JSON projects and presets. Read the in-app documentation for the workflow.

## Create an effect

1. Choose one of the 24 presets, or start with Blank particle.
2. Adjust the selected layer's color, size, emission, spawn shape, and motion.
3. Click **Add to effect** beneath a preset to combine its particles with your current layers. The layer toolbar + adds a blank layer. Add up to eight layers for independent colors and behaviors. Name, duplicate, hide, and remove layers as needed.
4. Use gravity, turbulence, floor bounce, size over life, and opacity over life to shape the result.
5. Orbit, pan, zoom, and use camera views to inspect it. Pause to capture a PNG.
6. Save project writes an editable Ember JSON file.

New, Open, Undo, and Redo are in the toolbar. Restart resets all layer simulations. Recovery settings are stored locally; save a project file for portable backups.

## Anime filter

Enable **Anime filter** under Look & feel on any particle layer. Adjust **Shading steps** (2–6) and **Ink outline** (0–25%). Use **Apply this look to all layers** to style the whole effect. Sprites use crisp silhouettes and cel shading; imported meshes use stepped view-based shading with dark rims. Connected ribbons and straight trails now have stepped shading across their width and dark ink edges. Shading steps and Ink outline affect both. Glow brightens their body shading while keeping ink edges dark; gradients and lifetime opacity still apply. Turn Glow off for a flatter look. Unreal scene bloom controls the exported halo. Settings persist in projects and presets, and generated Niagara and Blender materials include the style. Blender shades mesh geometry, so it is an approximation of the sprite preview. This does not apply a screen filter to existing native Niagara systems or an Unreal scene.

## Forces, collisions and lifetime

Use **Forces, collisions & lifetime** to combine wind, drag, attraction, swirl, and lift. Place the shared force center with X/Y/Z. Animate forces through **Change a property**. Recipe buttons add tornado, black hole, drifting smoke, or ricochet sparks layers without replacing the current effect.

Enable **Collide with a surface** for a movable horizontal plane. Choose Bounce or Remove particle, adjust its height, bounciness, and surface friction. Set continuous particle spin and a final lifetime color. All these controls work in the 3D preview, generated Niagara systems, saved projects/presets, and Blender bakes. Locked ring motion ignores forces and collisions. The collision plane is local to the effect and does not sample Unreal scene geometry.

This expands direct authoring; full standalone Niagara parity, GPU simulation, arbitrary graphs, skeletal sampling and arbitrary scene collisions remain outside Ember's particle builder. Native project systems retain their advanced features in Unreal.

## 3D collision obstacles

Enable **Collide with a surface** and open **3D collision obstacles**. Choose a floor, sphere, or box. Position and resize it in effect-local centimeters; the blue wireframe follows the selected layer and animated values while the grid is on. Choose Bounce or Remove particle in the dynamics controls. Sphere and box contact preserve the reflected normal velocity with adjustable bounciness and tangential friction. Use **Add sphere deflection example** for an editable starting layer.

These mathematical obstacles work in the preview, Niagara exports, and Blender particle bakes. They do not add scene meshes or sample arbitrary scene geometry. Collision uses per-step positions, so fast particles may pass through thin obstacles. Locked ring movement ignores collisions.

## Longer trails

Enable **Trail**, then set **Trail length**. 1× is the original length; use 3× or 5× for longer streaks (range 0.1×–10×). Each layer has its own length. Use a Trail length animation key to change it over time. Projects, presets, and Niagara exports keep the multiplier. Preview trails follow current velocity rather than recording curved paths; Niagara uses a separate velocity-aligned trail renderer behind each particle, with a fading material. Re-export older effects to get the corrected trail renderer.

## Your own presets

Click **Save as preset** in the library, name your creation, and save the whole effect or just the selected particle layer. Find it under **My presets**. Click it to apply particles to the active layer; for multi-layer presets, choose which saved layer to use. Other layers stay intact. The picker also offers **Add all layers to effect** to add the entire preset. **Add to effect** combines all its layers with your current particles. Motion, positions, and animations are preserved. Saving makes an independent copy. Presets save immediately and are flushed to disk when you close Ember; they remain available the next time you open it. Use Save project for a portable backup.

## Documentation

Click **Documentation**, press **F1**, or use the **?** button beside a feature. The offline menu has searchable articles, ranges, examples, and keyboard shortcuts for every feature. A printable reference is in [docs/FEATURES.md](docs/FEATURES.md).

## Blender and mesh transfer

Open **Blender & files**. Ember detects Blender or lets you choose its executable. **Save effect as .blend** bakes visible layers, trails, and particle animation into a replayable Blender scene. **Open Ember effect from .blend** recovers the embedded original settings; manual Blender scene edits do not become Ember animation keys.

Use **Import Blender mesh** to load visible mesh objects from `.blend`, `.glb`, or `.fbx` as particle shapes. Meshes use layer colors in Ember and persist in a local cache. Keep the original file for moving projects to another computer.

Connect Unreal, then use **Send selected mesh to Unreal** or **Unreal mesh → Blender** for static meshes. Exporting mesh-particle effects automatically imports their meshes and creates Niagara mesh renderers. Existing Unreal assets are preserved. Close the Unreal project before transferring. Rigs, Blueprints, scene logic, and Niagara node graphs are not converted between application formats. Blender particle scenes are baked; sprite styles become simple 3D geometry. Tested with installed Blender 5.1.

## Connect your Unreal project

Click **Connect Unreal project** in Ember, then **Choose project…** and select your `.uproject` file. Ember detects Unreal 5.6 or 5.8 and remembers the project after closing. Exports then use that project. Use the same menu to change projects or disconnect. Connecting only reads your project; plugin installation happens when you confirm an export.

## Export all layers to Unreal

Choose **Export effect → Send full effect to Unreal**, select 5.6 or 5.8, then select a .uproject. Close that Unreal project first. Ember installs its editor plugin, or backs up and updates an older Ember plugin in Saved/EmberPluginBackups. Existing effects are preserved using suffixes.

The importer creates a Niagara system with an editable CPU sprite emitter for each visible layer, plus generated particle materials. It translates the current spawn shape, particle appearance, amount, size, color, speed, gravity, turbulence, floor bounce, and lifetime size/opacity controls. Hidden layers stay in the Ember project but are omitted from export. Trails use separate velocity-aligned streak renderers behind the particles. Bloom depends on the Unreal scene. Preview and engine results are approximate; random distributions and burst loop timing can differ.

For manual import, **Get editor plugin** opens the matching folder. Copy EmberNiagara to your project's Plugins folder, restart Unreal, then use **Tools → Import Ember VFX**. Neither path requires Python.

The prebuilt plugins match the installed Unreal 5.6 and 5.8 builds. Other engine builds may require rebuilding the included plugin source. The engine installations should be in their normal Epic Games directories.

## Scope

This release completes the current sprite-particle authoring workflow, rather than every feature in Niagara. Arbitrary node scripting, event graphs, skeletal-mesh sampling, mesh renderers, and arbitrary scene geometry collision are not implemented. Floor collision is Z=0. Three-dimensional particles render using bundled WebGL 2 libraries, offline.

## Development

Use pnpm start for Electron, pnpm check for validation, and pnpm package to rebuild. Use node scripts/package.cjs release-v1.2 to replace the current release folder. The renderer is sandboxed with context isolation and no Node access. Plugin source is in unreal/source/EmberNiagara; scripts/verify-advanced.cjs verifies all presets in eight-layer groups across both engine versions.

Version 1.4 adds per-layer starting position offsets (X/Y/Z), a Center reset, and matching Niagara export in Unreal 5.6 and 5.8. Find these controls under Shape & motion. Offsets are in centimeters relative to the preset's original spawn region.

## Animate particles

Select a layer and click **Animate** below the preview. **Change a property** schedules a color, size, opacity, amount, emission, motion, or spawn-position change. Choose **At this time** for an instant switch or **Gradually** for a blend from the previous value. **Add particles / clear** schedules an extra burst or removes existing particles from that layer. Turn emission off to stop new particles without killing existing ones.

Drag the playback slider to preview a chosen time; this pauses playback. Animated layers play their steps once and keep their final values. Restart replays them. Timeline length sets the editable and scrub range (1–60 seconds), not an automatic stop. Position, speed, spread, and lifetime keys affect new particles; appearance and forces affect existing ones. Export includes animation keys, timed bursts, and clearing in Unreal 5.6/5.8.

## Aim velocity or spin in a ring

Under **Shape & motion → Particle movement**, choose **Aim the velocity** and set Heading and Elevation in degrees. Movement speed sets the launch speed. Choose **Spin around a ring** for a horizontal circular orbit, or click **Try a spinning ring** to set one up. Ring spin is degrees per second: +90 turns counterclockwise, −90 clockwise, and zero pauses rotation. Spread controls radius. Orbit ignores gravity and turbulence to keep a stable circle. Heading, elevation, and ring spin can be keyframed.

## Randomness and emission stretch

**Spin variation** gives each orbiting particle a stable random speed multiplier. **Direction variation** adds random launch heading/elevation, while **Random particle angle** varies the starting sprite orientation. Use squares, diamonds, or stars to see visual rotation. **Emission stretch X/Y/Z** scales the spawn region independently (0.1×–10×); Reset stretch returns all axes to 1×. X/Y stretching makes orbit rings elliptical. Point and flat spawn regions gain height when Z is above 1×. All new controls can be keyframed and exported.

**Distance from center variation** widens ring emission into a band by assigning different radii to new particles. At 40%, radii range from 60% to 140% of the base ring size. Each particle keeps its selected orbit size. This is separate from spin-speed variation and can be animated for new particles.
