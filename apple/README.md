# Ember on Apple devices

## iPhone and iPad

Open [Ember for iPhone/iPad](https://thecrusaderoffiction.github.io/Ember-VFX-Studio/) in Safari. Tap **Share → Add to Home Screen → Add**; keep **Open as Web App** enabled if it appears. Open the new Ember icon and let its first online load finish before going offline. This is an installable Home Screen web app, not an App Store or TestFlight app. It does not require an Apple Developer account. Apple documents this installation flow [here](https://support.apple.com/guide/iphone/iphea86e5236/ios).

Install first, then create your presets. Safari tabs and installed Home Screen apps can use separate storage. Presets and working recovery stay in that browser/app’s local storage; save important projects as files before removing the app or clearing website data. Offline availability depends on the device retaining its cached app files.

Use **Save project** or **Share / export** to save/share an editable `.ember.json` file. Where file sharing is supported, choose **Save to Files** in the share sheet. Otherwise Safari downloads the file. **Open** reads projects from Files. PNG textures are included in projects. A current Safari version with WebGL 2 is needed for the 3D preview.

## Mac

- [Apple Silicon / M-series Mac download](https://github.com/TheCrusaderOfFiction/Ember-VFX-Studio/releases/download/apple-v1.22.1/Ember-Mac-AppleSilicon.zip)
- Intel Macs: use the [browser editor](https://thecrusaderoffiction.github.io/Ember-VFX-Studio/). Native Intel packages are not published yet because their 3D startup verification remains unresolved.

Unzip the download and move **Ember.app** to Applications. These builds use ad-hoc signatures and are not notarized by Apple. If macOS blocks the first launch, its **System Settings → Privacy & Security → Open Anyway** flow can approve a downloaded app that you trust. Follow [Apple’s guidance](https://support.apple.com/102445); no global security setting needs to be disabled.

The Mac app includes the particle editor, 3D preview, presets, layers, animation, documentation and native project/image dialogs. Closing its main window quits Ember and flushes local recovery data. Cmd+S and Cmd+O open the native save/open dialogs.

## Across devices

Both Apple versions preserve ordinary Ember projects, including existing Unreal scene bindings and desktop mesh references. Open saved files in Ember on Windows for native Niagara asset creation, connected Unreal scenes, Blender execution and mesh importing. External mesh geometry stays in the Windows cache and is not previewed or bundled with an Apple project file. Sprite textures and flipbooks are embedded and supported.

## Build and verify

`node scripts/build-apple-web.cjs` builds the offline phone app into `release-apple-web/`. Host that folder over HTTPS; service workers cannot run from a local file opened directly. GitHub Pages deployment is handled by the **Apple apps** workflow.

On a Mac, install the locked dependencies with pnpm, then run `node scripts/package-macos.cjs`. It builds for the host architecture (or `--arch=arm64` / `--arch=x64`), verifies the ad-hoc signature and produces a ZIP plus SHA-256 checksum in `release-macos/`. `node scripts/verify-macos-ui.cjs` tests the packaged editor with a separate profile. The workflow publishes the verified Apple Silicon build and runs WebKit phone/iPad layout, file round-trip, offline reload and preset-persistence checks before publishing.

A future native iPhone/iPad TestFlight or App Store build needs an Apple Developer Program account and signing setup. See [Apple’s distribution requirements](https://developer.apple.com/documentation/xcode/distributing-your-app-for-beta-testing-and-releases).
