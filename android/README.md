# Ember for Android

Ember runs offline as an installed Android app. The same particle editor, presets, layers, animation, anime look, glow and documentation are bundled inside the APK. Android 9 or later and an up-to-date Android System WebView with WebGL 2 are required. Phone performance determines practical particle counts.

## Install

1. On your phone, download [Ember-Android.apk](https://github.com/TheCrusaderOfFiction/Ember-VFX-Studio/releases/download/android-v1.22.0/Ember-Android.apk).
2. Open the download. If Android asks, allow your browser or file manager to install this app, then choose **Install**.
3. Open **Ember VFX**. Use **Presets & layers**, **Preview**, and **Controls** along the bottom. Drag with one finger to orbit, pinch to zoom, and drag with two fingers to pan.

Save projects with **Save project** and choose a folder in Android’s file picker. **Open** reads the same `.ember.json` files as the desktop app. Presets and working recovery remain on this device after closing the app. Explicitly saved files can be moved to your PC for Niagara and Blender export. Uninstalling clears app-local presets and recovery, so save important projects as files first.

Niagara asset creation, connected Unreal scenes, Blender execution, mesh importing and the native Niagara workspace require the Windows desktop app. Existing scene bindings and mesh references remain in phone project files, but external mesh geometry is not included and is not previewed on Android. PNG textures and flipbooks are embedded and supported.

## Build

Install JDK 17 and an Android SDK with build tools and an API 36 platform. Set `JAVA_HOME` and `ANDROID_HOME` if they are not in the standard Windows locations. Run `node scripts/build-android.cjs`. The SDK’s AAPT2, D8, zipalign and apksigner produce `release-android/Ember-Android.apk`; Gradle and online dependencies are not needed.

`node scripts/build-android.cjs --debug` produces a separate `com.embervfx.studio.qa` package with WebView debugging enabled. The release package is not debuggable. Testing uses a separate emulator and never wipes a personal device.

The first build creates a private signing key and password under `.openai/android-signing/`, excluded from Git. Keep a secure backup of that entire directory. Future APK updates must use the same key to update existing installs without removing their local presets. Neither the key nor its password belongs in the repository or a release asset.

The native shell serves bundled assets from an isolated HTTPS origin, blocks external navigation and network content, disables file URL access, and uses Android’s document picker for explicit file reads/writes. It requests no storage or network permissions. See the Android documentation for [local WebView content](https://developer.android.com/develop/ui/views/layout/webapps/load-local-content), [document pickers](https://developer.android.com/training/data-storage/shared/documents-files), and [APK signatures](https://developer.android.com/tools/apksigner).
