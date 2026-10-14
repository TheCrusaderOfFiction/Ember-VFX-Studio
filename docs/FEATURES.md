# Ember feature guide

Available inside the app through Documentation, F1, and the ? buttons beside controls.

## Getting started

Workflow

Choose a preset or Blank particle. Adjust the selected layer on the right, then add more layers on the left. Save an Ember project to retain editable settings.

Try this: Try combining Campfire with Soft smoke.

## 24 starting effects

Workflow

Presets supply editable starting values. Selecting a preset replaces the selected layer only. Add to effect creates another particle layer and keeps your existing layers. Blank particle is a simple starting point. Use the search box and Energy / Nature filters.

Try this: A preset is a starting point; it is not a locked effect.

## Create your own presets

Workflow

Click Save as preset in the library. Name it and choose Whole effect to keep all layers, or Selected particle layer to save just the layer you are editing. Particle positions, motion, randomness, and animation are included. Find saved copies in My presets or search by name. Clicking a saved preset replaces only the active layer’s particle settings and animation. Other layers, the project name, and the active layer’s name and visibility stay as they are. For a preset with multiple layers, choose which particle layer to use, or click Add all layers to effect in the picker to add the entire preset alongside your current layers. Undo restores the previous settings. Add to effect combines its layers with your current effect when there is room within the eight-layer limit. The × button deletes the library copy after confirmation. Presets save immediately on this device and remain available after closing and reopening Ember. Save an Ember project file for a portable backup. Saving a preset makes a copy: later edits do not change it. Save again to create another version. Up to 100 saved presets.

Try this: Build a fireball from flames, smoke, and sparks, then save the whole effect as Fireball.

## Particle layers

Layers

An effect can contain up to eight independent emitters. Select a layer to edit its controls. Each has its own color, spawn shape, motion, lifetime, and particle appearance.

Try this: Combine a continuous smoke layer with a burst of sparks.

## Add preset particles

Layers

Click Add to effect underneath any preset to add its particles as a new layer. Existing layers and their settings are preserved. The new layer is selected so you can change its amount, color, position, motion, and animation. Each layer is independent, and all visible layers export together to Unreal. The limit is eight layers; remove a layer to make room. Undo removes an addition and Redo brings it back.

Try this: Try adding Soft smoke to Campfire, then move the smoke layer above the flames.

## Add a layer

Layers

The + button creates a new Blank particle layer and selects it. Existing layers keep running. The limit is eight layers.

Try this: Create a separate layer when you need another color or behavior.

## Duplicate a layer

Layers

The duplicate button copies the selected layer’s settings. The copy has its own controls and simulation.

Try this: Duplicate sparks, then change the copy to blue and slow it down.

## Show and hide layers

Layers

The checkbox beside a layer controls whether it appears in the preview and export. Hidden layers keep their settings. They do not advance in the preview while hidden.

Try this: Hide other layers to isolate the one you are editing.

## Remove a layer

Layers

The × button removes that layer. At least one layer remains. Undo can restore a removed layer during this app session.

Try this: Save your project before making large changes.

## Layer names

Layers

Edit the name beside the layer list to describe its role. Names are saved in the project.

Try this: Use names such as Core fire, Smoke plume, and Impact sparks.

## Particle color

Appearance

Choose a color swatch or use the custom color picker. The selected layer updates immediately; existing particles also receive its color.

Try this: Warm colors suit embers; low-saturation colors suit smoke.

## Particle amount

Appearance

In Continuous flow, this is the number of particles emitted per second (5–200). Burst creates twice this amount at once. More layers can increase the total density.

Try this: Begin around 30 and increase until the effect reads clearly.

## Particle size

Appearance

Controls the starting size of each particle (1–30). Individual particles vary slightly. Glow also adds a visible halo.

Try this: Large particles need fewer spawns to fill space.

## Particle appearance

Appearance

Choose Soft dot, Star, Square, Diamond, or Ring / bubble. These are procedural particle masks, not imported 3D meshes.

Try this: Use squares for confetti and rings for bubbles.

## Glow

Appearance

Adds a bright core and halo in the preview. In Unreal it uses emissive particle color; scene bloom affects the visible halo.

Try this: Turn glow off for leaves, confetti, and opaque-looking debris.

## Trail length

Appearance

Enable Trail, then adjust Trail length from 0.1× to 10×. 1× preserves the original streak length, 3× triples it, and 10× makes a much longer streak. Each layer has its own length. Trail length can change over time using a property animation, and is saved in projects and presets. Preview streaks extend behind the particle along its current velocity; they do not record a curved movement history. Niagara export uses stretched velocity-aligned sprites with the same length multiplier, so the exact appearance can differ.

Try this: Try a laser or spark effect with Trail on and Trail length at 4×.

## Trails

Appearance

Draws a short line behind moving particles in the preview. Unreal export uses stretched, velocity-aligned sprites rather than a full ribbon renderer.

Try this: Increase movement speed to make trails easier to see.

## Size over lifetime

Lifetime

Size at end of life is a multiplier from 0–5. 0 shrinks to nothing; 1 holds the size; 3 grows to three times the starting size. The change is linear.

Try this: Growing particles work well for steam and smoke.

## Opacity over lifetime

Lifetime

Opacity at end of life ranges from 0 (transparent) to 1 (visible). Particles fade in briefly, then change linearly toward this value.

Try this: Use 0 to avoid particles disappearing abruptly.

## Lifetime

Lifetime

Each particle lives for 50–100% of this value (1–8 seconds). Short lifetimes produce quick effects; long lifetimes allow wider motion.

Try this: Increase lifetime before reducing speed for drifting effects.

## Continuous and burst emission

Motion

Continuous flow emits particles steadily. Burst emits them together. The preview repeats bursts after Lifetime plus 0.5 seconds. Playback speed affects preview timing only.

Try this: Use Burst for explosions and Continuous flow for weather.

## Spawn shapes

Motion

Point concentrates the origin. Sphere fills a volume. Cone spreads moving particles upward. Ring places particles around a circle. Box spreads them across an area. Some presets have specialized directions.

Try this: Use Sphere for explosions, Box for weather, and Cone for fountains.

## Movement speed

Motion

Sets the initial particle velocity (1–100). Each preset interprets direction differently. Gravity and turbulence change velocity after spawning.

Try this: High speed plus a short lifetime produces tight impacts.

## Spread

Motion

Widens the spawn region or velocity spread (5–100), depending on the selected spawn shape. Changing it affects new particles.

Try this: Use a low value for a narrow jet and a high value for a shower.

## Starting position X

Motion

Moves this layer’s spawn region left or right along X. Enter an offset from −10000 to 10000 in Unreal units (centimeters). Other layers keep their own positions.

Try this: Use Center to reset all three offsets.

## Starting position Y

Motion

Moves this layer’s spawn region forward or back along Y. Positions are local to the effect and are included in saved projects and Unreal exports.

Try this: Separate smoke and sparks by giving each layer a different offset.

## Starting position Z

Motion

Moves this layer’s spawn region up or down along Z, relative to the preset’s original starting region. Editing a position restarts that layer so you see the new placement immediately. Ground bounce stays at Z=0.

Try this: Raise a fireball with a positive Z offset.

## Emission stretch X

Motion

Scales the spawn area left and right along X, from 0.1× to 10×. One keeps the original width. Two doubles it. Starting-position offsets stay separate. For ring orbit, X and Y stretch make an oval while particles follow the oval path.

Try this: Try X=3 and Y=0.5 for a long, narrow emission area.

## Emission stretch Y

Motion

Scales spawn depth along Y from 0.1× to 10×. This affects the emission area rather than the size of individual particles. Reset stretch restores X, Y, and Z to 1×.

Try this: Use a box shape and increase Y to fill a long corridor.

## Emission stretch Z

Motion

Scales the spawn height around its original center, from 0.1× to 10×. A sphere becomes taller or flatter. Point, cone, box, and horizontal ring regions gain height variation when Z is above 1×. Orbit particles keep their individual starting heights.

Try this: Increase Z above 1 to turn a flat starting region into a taller band.

## Distance from center variation

Motion

Gives each new ring particle a randomly chosen distance from the center, from 0–100% variation. At zero, particles start at the same radius. At 40%, radii range from 60% to 140% of the base ring radius. Orbiting particles keep their chosen orbit size as they spin. Works with stretched oval rings and ordinary ring emission. Animation keys affect newly created particles.

Try this: Try 35% to turn a thin ring into a wider band.

## Random ring rotation speed

Motion

Spin variation sets a stable random speed difference for each orbiting particle, from 0–100%. Zero keeps the same speed for all particles. At 50%, speeds range from half to one-and-a-half times Ring spin. At 100%, they range from zero to twice Ring spin. The random choice stays fixed for each particle and can be adjusted with animation keys.

Try this: Try Ring spin=90 and Spin variation=35 for a less uniform ring.

## Random launch direction

Motion

Turns each new particle’s velocity by a random heading and elevation offset, up to the selected number of degrees (0–180). Speed is preserved. This works with preset and aimed movement; ring orbit keeps its tangent direction and uses Spin variation instead. Changes affect new particles.

Try this: Use 10 degrees to loosen a focused jet without scattering it everywhere.

## Random particle angle

Appearance

Gives new particles a random starting visual angle between minus and plus the selected number of degrees (0–180). Use stars, squares, or diamonds to see the difference. Soft dots and round rings look the same when rotated. This changes appearance rather than the travel direction.

Try this: Choose Square and set Random particle angle to 180 for varied confetti orientations.

## Velocity direction and ring motion

Motion

Choose Preset movement for the original effect, Aim the velocity to send particles in a chosen direction, or Spin around a ring to keep particles on a horizontal orbit. Try a spinning ring sets up the selected layer. Orbit uses Ring spin instead of Movement speed, gravity, or turbulence.

Try this: Use Spin around a ring, then set Ring spin to −90 to reverse it.

## Velocity heading

Motion

In Aim the velocity mode, heading turns the starting velocity around the upward Z axis. Zero points along +X, 90 along +Y, and −90 along −Y. Values range from −180 to 180 degrees. Heading keys apply to newly created particles.

Try this: Use heading 0 and elevation 0 for a horizontal jet along X.

## Velocity elevation

Motion

In Aim the velocity mode, elevation tilts starting velocity above or below the horizontal plane. Zero is horizontal, 90 points straight up, and −90 points down. Movement speed controls how fast particles travel. Elevation keys apply to new particles.

Try this: Use elevation 45 to launch particles diagonally upward.

## Ring spin

Motion

Spin around a ring keeps particles on a horizontal orbit around the layer’s starting position. Ring spin is degrees per second, from −360 to 360. Positive values rotate counterclockwise viewed from above; negative values reverse direction; zero pauses orbiting. Spread sets ring radius. Ring spin can be animated and affects existing particles. Animated position changes move the orbit center.

Try this: At 90 degrees per second, a particle completes a circle in four seconds.

## Gravity

Motion

Adds acceleration along Z (−100 to 100). Negative values pull downward, zero adds no acceleration, and positive values accelerate upward.

Try this: Try −65 with a fountain to make particles arc back down.

## Swirl and turbulence

Motion

Adds changing sideways forces (0–100). Higher values produce more erratic motion. Vortex and Mini galaxy also have a preset-specific circular force.

Try this: A small value adds life to smoke without scattering it too far.

## Ground bounce

Motion

When enabled, particles bounce off the flat Z=0 plane and lose some speed. This is floor collision only; arbitrary meshes and scene geometry are not collision targets.

Try this: Combine negative gravity and bounce for sparks hitting the ground.

## Particle animation timeline

Animation

Select a layer and open Animate this layer. Add timed property changes or particle events. Each layer has its own animation, saved with the project and included in Niagara exports. Animated layers play once, keeping their last values; use Restart to replay. Up to 64 property keys and 32 events per layer, over 1–60 seconds.

Try this: Make a fireball turn red at 1 second, add sparks at 2 seconds, and stop emission at 3 seconds.

## Timed property changes

Animation

Change a property schedules a keyframe. At this time switches the value instantly. Gradually blends from the previous keyframe for that property (or its starting value at time zero). Color, size, opacity, gravity, turbulence, and end-of-life settings affect existing particles. Starting position, speed, spread, and lifetime apply to new particles. Amount controls new particles per second.

Try this: Add opacity 0 at time 3 with Gradually to fade the layer out.

## Start and stop new particles

Animation

Animate Emit new particles to Off to stop spawning while existing particles finish their lives. Set it back to On later to resume a continuous flow. Setting Particle amount to zero also stops continuous spawning. Turning emission on in Burst mode does not create another burst; add a timed burst event for that.

Try this: Set emission Off at 0 seconds and On at 1 second for a delayed effect.

## Timed particle bursts and clearing

Animation

Add particles / clear adds an event. A burst creates 1–2000 particles at its scheduled time, even when continuous emission is off. Clear removes all particles already in this layer; it does not disable future spawning. Events fire once per playback. If a clear and burst share a time, clearing happens first.

Try this: Clear the layer at 3 seconds and also turn emission Off there to end it completely.

## Preview an animation time

Animation

Drag Preview time in the playback bar to rebuild all visible layers from the start up to that time. Scrubbing pauses playback. Press Play to continue or Restart to replay from zero. Timeline length controls the scrub range and the latest allowed key or event, not particle lifetime or an automatic stop.

Try this: Scrub just before and after a burst time to check when sparks appear.

## 3D camera

Preview

Left-drag to orbit, scroll to zoom, and right-drag to pan. Perspective, Front, Side, and Top are available in the camera menu. The reset button returns home.

Try this: Inspect the same effect from several angles before exporting.

## Keyboard controls

Preview

Focus the canvas to use arrow keys to orbit, +/− to zoom, and 0 to reset the camera. Ctrl+Z / Ctrl+Shift+Z undo and redo authoring changes when a text field is not focused. F1 opens documentation.

Try this: Camera changes do not alter the saved effect.

## Ground grid

Preview

The grid button shows or hides the ground plane guide and axis helper. Z is the upward axis. This matches the direction used by gravity and floor bounce.

Try this: The grid is a preview guide and is not a spawned particle.

## Preview background

Preview

The background button switches between dark and lighter preview environments. It does not change particles or Unreal assets.

Try this: Check bright and dark backgrounds for readability.

## Playback and restart

Preview

Pause freezes the simulation while allowing camera movement. Restart clears all layer simulations. Speed can be 0.5×, 1×, or 2×. The progress bar shows a repeating ten-second preview cycle.

Try this: Pause to inspect particle placement or capture an image.

## Save and open projects

Files

Save project writes an editable .ember.json file. Open project accepts old v1 and layered v2 files. Automatic recovery is local to this app; explicit files are portable.

Try this: Keep a project file alongside images and Unreal assets.

## New effect

Files

New effect resets the editor to one Blank particle layer. Undo can restore the previous effect in this session. Save a file first if you need a permanent copy.

Try this: Start with Blank particle when learning each control.

## Undo and redo

Files

Undo restores the previous authoring settings; Redo reapplies them. History is limited to 60 changes in this session. Camera movement and elapsed simulation time are not part of history.

Try this: Use toolbar buttons when keyboard shortcuts are unavailable.

## PNG capture

Files

The snapshot button saves the current 3D view as a PNG image. It includes the preview background and visible guides. Hide the grid first for a cleaner capture.

Try this: Pause and frame the camera before taking a snapshot.

## Unreal export

Export

Choose Unreal 5.6 or 5.8 and a destination .uproject. Close that project in Unreal first. Ember installs its companion editor plugin and creates assets under Content/Ember. Existing names receive a suffix.

Try this: Use the exact engine build that matches the bundled plugin.

## Editor plugin

Export

Get editor plugin opens the folder for the selected version. Copy EmberNiagara into your project’s Plugins folder, restart Unreal, then choose Tools → Import Ember VFX.

Try this: The plugin imports saved project JSON without Python.

## Supported features and limits

Export

Ember is a standalone sprite-particle authoring app. It does not implement every Niagara feature: arbitrary node scripting, skeletal-mesh sampling, event graphs, mesh renderers, and arbitrary scene collisions are not available.

Try this: Use the documented controls for this release; do not expect complete Niagara parity.
