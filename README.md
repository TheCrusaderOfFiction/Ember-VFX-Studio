# Ember VFX Studio — current desktop release (1.10.3)

Open **release-v1.2/Ember-win32-x64/Ember.exe**. The existing desktop app folder is updated in place. Keep its runtime and resources together. No browser or server is required. Clicking the window X exits Ember completely. Opening Ember again focuses its existing window instead of starting another copy.

## Create an effect

1. Choose one of the 24 presets, or start with Blank particle.
2. Adjust the selected layer's color, size, emission, spawn shape, and motion.
3. Click **Add to effect** beneath a preset to combine its particles with your current layers. The layer toolbar + adds a blank layer. Add up to eight layers for independent colors and behaviors. Name, duplicate, hide, and remove layers as needed.
4. Use gravity, turbulence, floor bounce, size over life, and opacity over life to shape the result.
5. Orbit, pan, zoom, and use camera views to inspect it. Pause to capture a PNG.
6. Save project writes an editable Ember JSON file. Open accepts both older v1 projects and layered v2 projects.

New, Open, Undo, and Redo are in the toolbar. Restart resets all layer simulations. Recovery settings are stored locally; save a project file for portable backups.

## Your own presets

Click **Save as preset** in the library, name your creation, and save the whole effect or just the selected particle layer. Find it under **My presets**. Click it to apply particles to the active layer; for multi-layer presets, choose which saved layer to use. Other layers stay intact. The picker also offers **Add all layers to effect** to add the entire preset. **Add to effect** combines all its layers with your current particles. Motion, positions, and animations are preserved. Saving makes an independent copy; use Save project for a portable backup.

## Documentation

Click **Documentation**, press **F1**, or use the **?** button beside a feature. The offline menu has searchable articles, ranges, examples, and keyboard shortcuts for every feature. A printable reference is in [docs/FEATURES.md](docs/FEATURES.md).

## Export all layers to Unreal

Choose **Export effect → Send full effect to Unreal**, select 5.6 or 5.8, then select a .uproject. Close that Unreal project first. Ember installs its editor plugin, or backs up and updates an older Ember plugin in Saved/EmberPluginBackups. Existing effects are preserved using suffixes.

The importer creates a Niagara system with an editable CPU sprite emitter for each visible layer, plus generated particle materials. It translates the current spawn shape, particle appearance, amount, size, color, speed, gravity, turbulence, floor bounce, and lifetime size/opacity controls. Hidden layers stay in the Ember project but are omitted from export. Trails use stretched velocity-aligned sprites. Bloom depends on the Unreal scene. Preview and engine results are approximate; random distributions and burst loop timing can differ.

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

## GitHub

Source repository: https://github.com/TheCrusaderOfFiction/Ember. After reviewing and committing changes, use `pnpm push` to push main. Desktop packages, dependencies, private saves, and Unreal build outputs are excluded from Git. The only local desktop package to keep is `release-v1.2/Ember-win32-x64`; other release folders are redundant.

The source uses the repository’s existing GPL-3.0 license. To package on a fresh checkout, first build the Niagara plugin for each Unreal version and collect the binaries into `unreal/packaged`. Those generated binaries remain local.
