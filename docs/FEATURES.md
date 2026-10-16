# Ember feature guide

Available inside the app through Documentation, F1, and the ? buttons beside controls.

## Small mesh creator

Appearance

Open Shape & motion → Create a mesh. Choose Sphere, Cube, Cone, Cylinder, Ring, Crystal or Plane. Width, Height and Depth stretch the shape; Taper narrows its top and Twist turns the geometry along its height. Detail controls roundness. Drag the preview to orbit and scroll to zoom; Wireframe shows its triangles. Save mesh as GLB writes a reusable mesh without Blender. Use on active layer converts and caches the mesh using your installed Blender, then assigns it to that layer only. The particle layer supplies color, glow, size and animation; preview shading is only for inspecting geometry. Saved projects and presets retain a mesh reference, so keep the cached mesh on this Windows PC or export a GLB and import it on another PC. Mac/Linux/mobile can save a GLB but direct mesh-particle assignment is Windows-only.

Try this: Choose Cone, increase Height, set Taper to 0.5 and Twist to 90 degrees for a curved shard.

## Mesh emission, scene collisions and attachments

Unreal integration

Open Whole effect & Unreal scene. Read connected project assets lists static meshes, skeletal character meshes and levels from the connected Unreal project. Save and close that project before reading it. Choose a mesh surface to store a reference snapshot for Ember and Blender; native Niagara samples the actual asset. Skeletal emission uses animated character geometry when attached to a character mesh in Unreal. Allow CPU Access may need enabling on the mesh in Unreal for CPU surface sampling. Choose a level, actor and optional socket to make the whole effect follow it. Exports create a companion Blueprint under Content/Ember/Actors; place it in that level. Its Target Actor property lets you choose a different actor in Unreal. The Ember runtime plugin is required for this companion actor. Level inventory includes loaded actors in the persistent level, not unloaded World Partition cells. Scene collisions use Unreal geometry: CPU traces or GPU screen-depth queries. GPU depth collisions depend on visible scene depth. Ember and Blender retain the saved reference surface and analytic obstacles; they do not simulate an Unreal level. Ring-locked motion cannot use mesh emission or scene collisions.

Try this: Choose a character mesh and hand socket, export, and place the companion actor in your Unreal level.

## Whole-effect intensity, scale and tint

Appearance

Whole effect controls adjust all current layers together and are retained when you add or apply presets. Intensity multiplies new particle emission; zero stops new source particles. Scale enlarges positions, sizes and trails around the effect origin without changing lifetime. Tint multiplies existing particle and gradient colors; white preserves them. Reset whole-effect controls restores 1× intensity, 1× scale and white tint. Projects and recovery preserve these controls. Presets store them, but applying preset layers keeps the current whole-effect controls and attachment so other layers stay unchanged. Niagara exports include User.EmberIntensity, User.EmberScale and User.EmberColor controls. Initial and scheduled burst counts are baked using the chosen intensity; continuous emission follows the exposed intensity. Blender bakes the authored values.

Try this: Try Scale 2× and a pale blue tint on an effect with several layers.

## GPU particle simulation

Unreal integration

Choose GPU under Whole effect & Unreal scene for the selected layer. Set GPU particle amount from 5 to 100,000. Continuous flow uses this amount per second; Burst emits twice this amount initially. Niagara runs this layer on the graphics card with fixed effect-local bounds. Ember previews at most 2,000 particles per second and 16,000 source particles; Blender bakes that preview rather than running Niagara GPU simulation. Event response rules require CPU mode, so remove them before selecting GPU. GPU scene collisions use scene depth and depend on visible geometry. Adjust fixed bounds in Unreal for effects that travel beyond the generated bounds. A fast GPU can handle more particles, but the actual limit depends on effects, materials and hardware.

Try this: Try 5,000 GPU particles per second before increasing the count.

## Use a preset or layer as an event response

Animation

In Particle event rules, choose a preset or layer in Custom response source. Built-in presets, example presets, personal presets and current particle layers are available. Ember copies that layer’s particle shape, renderer, forces and lifetime curves, then lets you adjust response count, color, size, speed, lifetime, Glow and Trail. The response starts at the source particle’s first contact or death position and uses its own local motion. Enable Keep source animated properties to replay the chosen source’s property keys from zero for each response burst. The snapshot is taken when you choose the source; choose it again to capture later edits. Leave this off for fixed response settings. Source emission/count keys and timed burst/clear steps are excluded, along with scene attachments, surface bindings and nested event rules. Each event creates a fixed burst; it does not recursively emit other responses. Imported mesh responses need their cached mesh files, just like mesh particle layers. Native Niagara uses CPU event handlers; Blender bakes separate response layers.

Try this: Use a ribbon preset as a collision response, then give it a longer lifetime.

## Example presets

Getting started

Open Examples in the preset library. Impact responses, Fireball path, Flowing ribbon, Sphere deflection, Tornado, Black hole, Drifting smoke, Ricochet sparks, Spinning ring, Grow & dissolve and Hot spark gradient are built-in presets. Each has a distinct glowing illustrated icon matching the original library style. Click a card to replace only the active layer. Add to effect creates a new layer alongside your existing particles. Search finds examples by name. These presets are available after reopening Ember and do not use your 100 personal preset slots. Edit one and choose Save as preset to keep your own version.

Try this: Open Examples, then add Impact responses to your effect.

## Collision and death response bursts

Animation

Open Particle event rules. On first collision creates one response burst at the first contact of each source particle with its enabled floor, sphere or box. Enable Collide with a surface and choose movement other than locked ring orbit. On death responds when a particle reaches its lifetime or is removed by collision. Clear timeline events, Restart, and deleting a layer do not create death responses. Choose Sparks, Smoke or Explosion, or pick your own preset or layer below, then adjust particles per event (1–32), size, speed, lifetime, color, Glow and Trail. Responses inherit the source anime look, have their own motion and lifetime, and do not recursively trigger rules. Custom responses can keep their source’s animated properties; its property timeline restarts for each response burst. Changing a rule restarts the selected layer. Burst layers with rules play once; Restart replays them. The preview limits each response pool to 4,000 particles; native Niagara processes up to 128 events per frame per rule. These are effect-local analytic collisions, not arbitrary Unreal scene collisions. Niagara exports create CPU event generators and response emitters; Blender bakes the response particles as separate layers. Rules are saved in projects, presets and recovery.

Try this: Load Impact responses from Examples to create sparks and smoke when falling particles hit the floor.

## Emitter motion paths

Animation

Open Emitter motion path. Straight route, Arc route and Closed route create editable starting-position keys for the active layer. Other property keys and timed bursts or clears stay unchanged. Select a waypoint to edit its time and X/Y/Z offsets in centimeters. The first time stays at zero; later times must increase. Add waypoint inserts after the selected point. Remove deletes a middle or final point when at least two remain. Drag waypoints in preview enables 3D handles; select the X/Y, X/Z or Y/Z drag plane, then drag a handle. Hold Shift to snap to 5 centimeters. Change the camera or drag plane if viewing it edge-on. A purple line shows the route and a gold marker shows the current emitter center. Hide the grid to hide the guide unless drag mode is on. Routes use straight segments between points, play once, and hold their final position; Restart replays them. Existing emitted particles keep traveling independently, while locked ring motion carries existing orbits with its moving center. Lifetime controls visible trail duration; emission keys can stop new particles at the end. Remove path, keep start clears only position keys and retains the first waypoint as the static starting offset. Routes use three ordinary timeline keys per point, sharing the 64-key budget, up to 21 waypoints. Editing converts existing position keys into linear route segments. Projects, presets, Blender baking and Niagara exports already preserve these position keys. Find Fireball path in the Examples preset tab.

Try this: Load Fireball path from Examples, enable drag mode, choose X/Z, and drag the middle waypoint upward.

## Particle PNG textures and flipbooks

Appearance

Open Particle textures & flipbooks and import a PNG up to 2048 by 2048 pixels and 256 KB. Transparent images work best. A single image uses one row and one column. For an animation sheet, enter its columns and rows; frames read left to right then top to bottom. Set Frames per second and Loop animation. Without looping, playback holds on the last frame. Every particle starts at frame zero when it is born. Texture replaces the sprite shape and is tinted by particle color and lifetime gradients. Glow brightens the texture; anime mode quantizes texture shading. Imported textures select sprite rendering; mesh particles retain their mesh appearance. Remove texture restores procedural sprites. Images are embedded in saved projects and presets and exported as texture assets to Niagara. Blender bakes textured planes using a fixed viewing direction; they do not follow a moved Blender camera. Connected ribbons keep their own strip material.

Try this: Use a 4 by 4 smoke sheet at 12 frames per second and turn looping off.

## Connected curved ribbons

Appearance

Open Curved ribbons and choose Connected ribbon. This enables trails and joins live particles of the layer in birth order into a camera-facing strip. Ribbon width is measured in centimeters; Taper toward older particles shrinks the strip as particles age. Each layer has its own ribbon. Use continuous emission, a narrow point and low direction randomness for smooth curves. Lifetime sets the visible stream length; Straight streaks still uses Trail length instead. Lifetime opacity and color gradients also shade the ribbon. Find Flowing ribbon in the Examples preset tab. Exports use a native Niagara ribbon renderer, while Blender bakes ribbon geometry using a fixed viewing direction. A ribbon joins particles; it does not record a separate historical trail behind every particle.

Try this: Load Flowing ribbon from Examples, then change wind to bend it.

## Lifetime curves and color gradients

Animation

Open Lifetime curves & color gradient and choose Size multiplier, Opacity or Color gradient. Enable Use this curve to edit up to eight stops from birth (0 percent) to death (100 percent). Interior stop ages and values are editable; Remove deletes a middle stop, and Add stop divides the largest gap. Size multiplies particle size from 0 to 5. Opacity is absolute from 0 to 1, then multiplied by the layer opacity. Color gradients blend in linear color space. Enabled curves replace the matching end-size, end-opacity or two-color lifetime fade. Curves apply separately to each particle based on its age, while the layer timeline continues to animate global properties. Projects and presets preserve curves. Sprite and mesh preview, Blender baking and Niagara exports use the curves. Choose Glow intensity in the lifetime dropdown to fade glow from 1 (full) to 0 (none), or add stops for pulses. Enable Glow in Appearance; opacity remains independent.

Try this: Load Grow & dissolve or Hot spark gradient from Examples.

## Sphere and box collision obstacles

Dynamics

Enable Collide with a surface, then open 3D collision obstacles. Choose Horizontal floor, Sphere or Box. Sphere and box centers use effect-local X/Y/Z centimeters. Set a sphere radius or full box dimensions. Bounce reflects incoming velocity using Bounciness and Surface friction; Remove particle deletes particles that enter the shape. A blue wireframe shows the selected layer’s current obstacle when the grid is on, including animated sizes and positions. Move and resize obstacles through timeline property keys. Locked ring movement ignores collisions. Find Sphere deflection in the Examples preset tab. These shapes export as Niagara collision calculations and baked Blender particle motion; they do not create visible Unreal meshes or sample scene geometry. Fast particles can cross thin obstacles between simulation steps.

Try this: Load Sphere deflection from Examples, change its radius, then switch On collision to Remove particle.

## Wind, drag, attraction and swirl

Dynamics

Open Forces, collisions & lifetime. Wind X/Y/Z adds acceleration in centimeters per second squared. Air resistance slows velocity exponentially per second. Positive Pull / push attracts particles to Force center; negative pushes away. Swirl force adds tangential acceleration around the vertical axis through Force center; its sign reverses direction. Swirl lift adds vertical acceleration. These forces combine with aimed or preset movement, gravity and turbulence. Locked ring motion ignores forces and collisions. Force values can be animated through Change a property. Force recipes are presets in the Examples library tab. Click a card to use it on the active layer, or Add to effect to create a separate layer.

Try this: Load Tornado from Examples, then increase Air resistance to make the swirl settle.

## Collision surface

Dynamics

Enable Collide with a surface, then choose Bounce or Remove particle in Forces, collisions & lifetime. Collision floor height places a horizontal plane in effect-local centimeters. Bounciness controls retained vertical speed (0 to 1). Surface friction reduces sideways speed per impact (0 to 1). Remove particle deletes a particle when it crosses below the plane. Locked ring movement ignores collisions. These settings export to Niagara and baked Blender animation. This surface is not arbitrary Unreal scene geometry.

Try this: Use high bounciness and low friction for ricochet sparks.

## Particle spin and color over lifetime

Dynamics

Particle spin rotates each sprite or mesh continuously in degrees per second; negative spins in reverse. Spin is independent of the direction particles travel and can be animated. Enable Color over lifetime and choose Fade toward to blend from the layer color to a final color based on each particle’s own age. Layer color animation still works, and the result blends in linear color space. Projects, presets, Niagara export and Blender baking preserve these controls.

Try this: Fade hot yellow sparks toward red while spinning them at 180 degrees per second.

## Anime filter

Appearance

Enable Anime filter under Look & feel for the selected layer. Sprites gain sharp silhouettes, flat shading bands, and dark ink edges. Imported meshes use view-based stepped shading and an ink rim. Shading steps ranges from 2 to 6; fewer steps give a flatter cel look. Ink outline ranges from 0 to 25 percent; set 0 for no ink. Connected ribbons and straight trails also use stepped shading across their width and dark ink edges. Shading steps and Ink outline control them, and zero outline removes the ink. Glow boosts body shading while keeping ink dark; lifetime colors and opacity continue to apply. Turn Glow off for a flatter look. Unreal scene bloom controls the exported halo. Particle motion and animation stay unchanged. Each layer can use a different style. Apply this look to all layers copies the current filter switch, shading steps and outline across the whole effect. With the switch off, it removes the filter from every layer. The settings save with projects and presets and export into generated Niagara materials and Blender materials. Blender uses view-based mesh shading, so its look differs from camera-facing sprites. Native project systems are not changed by the builder filter.

Try this: Try Campfire with 3 shading steps and 8 percent outline, then add a softer smoke layer.

## Project Niagara workspace

Native editor

Choose Project Niagara systems to browse your connected project. Save and close that Unreal project before reading assets or saving a copy. Read project effects lists native Niagara systems. Select a system to see its emitters, CPU or GPU simulation, renderer types, and exposed User Parameters. Float, integer, boolean, vector and linear RGBA color parameters can be changed here. Save a new system duplicates the whole native asset and applies those controls without replacing its graphs. The source stays unchanged. Other parameter types remain editable in Unreal. Open full editor & preview launches Unreal with the selected system. Native GPU, skeletal, event, collision, ribbon, stage and script features execute in Unreal, not in the Ember particle preview. Builder projects and native systems remain separate; the native workspace does not translate graphs into builder layers.

Try this: Expose User Parameters in a complex Niagara effect to give it a small set of easy controls.

## Save native system copies

Native editor

Copies are saved under Content/Ember/Systems with unique names. Use letters, numbers and underscores. A copy retains the full Niagara system and references to its existing dependencies. It belongs to the connected project; Ember JSON saves and presets do not include it. Do not move the system to another project without its dependencies. Close Unreal before saving a copy, then open the new system to preview and continue editing.

Try this: Make Fireball_Blue from an existing native fireball and adjust its exposed color.

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

Adds a bright core and halo in the preview. Glow strength ranges from 0 to 5×, with 1× preserving the original look. The lifetime Glow intensity curve multiplies this strength, so a 2× strength and 0.5 lifetime value give 1× glow. Turn on Glow to use the slider. In Unreal it uses emissive particle color; scene bloom affects the visible halo.

Try this: Turn glow off for leaves, confetti, and opaque-looking debris.

## Trail length

Appearance

Enable Trail, then adjust Trail length from 0.1× to 10×. 1× preserves the original streak length, 3× triples it, and 10× makes a much longer streak. Each layer has its own length. Trail length can change over time using a property animation, and is saved in projects and presets. Preview streaks extend behind the particle along its current velocity; they do not record a curved movement history. Niagara export draws a separate fading streak behind each particle using its current velocity and the same length multiplier. Re-export older Unreal effects to get this trail renderer.

Try this: Try a laser or spark effect with Trail on and Trail length at 4×.

## Trails

Appearance

Draws a fading streak behind moving particles. Unreal export adds a separate velocity-aligned trail renderer and fading material, keeping the main particle shape intact. Use Trail length to extend it. Re-export older Unreal effects to get the corrected renderer.

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

Choose Preset movement for the original effect, Aim the velocity to send particles in a chosen direction, or Spin around a ring to keep particles on a horizontal orbit. Spinning ring preset sets up the selected layer. Orbit uses Ring spin instead of Movement speed, gravity, or turbulence.

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

Select a layer and open Animate this layer. Add timed property changes or particle events. Each layer has its own animation, saved with the project and included in Niagara exports. Animated layers play once, keeping their last values; use Restart to replay. Up to 64 property keys and 32 events per layer, over 1–60 seconds. Timed Add particles events have Keep animated properties enabled by default: their particles follow the layer timeline. Turn it off to use base property settings for that burst; lifetime curves still apply and Clear removes both kinds.

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

## Blender effects and files

Files

Open Blender & files in the top bar. Ember detects Blender or lets you choose blender.exe. Save effect as .blend bakes visible particle layers over 1–20 seconds at 1–30 frames per second. Each layer has its own collection, geometry-node particle shapes, trail geometry, and frame visibility animation. The scene replays after reopening without running scripts. Original Ember settings are embedded in a text block; Open Ember effect from .blend restores those settings. Later manual Blender edits are not converted back into Ember animation keys. Units are converted from Ember centimeters to Blender meters. Large bakes are limited; lower the frame rate or duration if needed.

Try this: Export a one-second test at 12 fps first, then open it in Blender and play the timeline.

## Imported mesh particles

Appearance

Click Import Blender mesh under Shape & motion, or use Blender & files. Choose a .blend, .glb, or .fbx file. Visible mesh objects are combined as a static particle shape. Use Render particles as to switch between sprites and the imported mesh. Meshes use the layer color and lifetime opacity in Ember; positions, randomness, animation, and trails still work. The local mesh cache persists after closing, and projects and presets remember its reference. Keep the original mesh file for portability; another computer needs it imported again. Each mesh is normalized so particle size remains useful.

Try this: Import a small Blender cube, choose Imported 3D mesh, and add random particle angle for varied mesh orientations.

## Blender and Unreal mesh transfers

Export

Connect an Unreal project first. Send selected mesh to Unreal imports the selected Blender mesh as a new static-mesh asset under Content/Ember/Meshes. Exporting an effect with mesh particles also imports the referenced meshes automatically and creates Niagara mesh renderers. Unreal mesh → Blender lets you pick a static-mesh .uasset inside that project’s Content folder and saves a .blend scene. Close the project in Unreal before a transfer. Skeletal rigs, Blueprint behavior, and Niagara graphs are not translated; transfer is for static mesh geometry and compatible materials. Particle effects are baked for Blender, while their original Ember settings remain available for return to Niagara.

Try this: Model a shard in Blender, import it in Ember, then export a burst of mesh particles to the connected Unreal project.

## Connect an Unreal project

Export

Click Connect Unreal project in the top bar, choose Unreal 5.6 or 5.8, then click Choose project. Select a .uproject file in the file picker. Ember reads the project’s engine version and remembers your selection after closing. Exports go to the connected project, while a final confirmation still tells you what will be created. Use the same button to change projects or disconnect. Connecting does not change any Unreal project files.

Try this: Connect your game project once so you can export multiple effects without selecting its file each time.

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

Ember is a standalone sprite and mesh particle authoring app. It does not implement every Niagara feature: arbitrary node scripting, skeletal-mesh sampling, event graphs, skeletal animations, and arbitrary scene collisions are not available.

Try this: Use the documented controls for this release; do not expect complete Niagara parity.
