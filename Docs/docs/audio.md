# Audio

Polly provides an easy-to-use, safe audio API.
Compared to full-fledged audio engines like [FMOD](https://www.fmod.com) and [WWise](https://www.audiokinetic.com/en/wwise/overview/),
Polly's audio API is rather simple, but powerful enough for most cases.

The central type for audio playback is the `AudioDevice` class and can be obtained via `Game::audioDevice()`.

## Sounds

The `Sound` class encapsulates a sound, including its data.

Sounds are assets and can therefore be loaded from the asset storage, like e.g. images.
Since assets are reference-counted objects, loading the same sound multiple times will only increment its reference count.

When all references to a sound vanish, the sound is automatically unloaded.

Playing a sound is done using `AudioDevice::playSoundOnce()`:

```cpp
auto sound = Sound("Boom.wav");

audio().playSoundOnce(sound, /*volume: */ 1.0f, /*pan: */ 0.0f, /*delay: */ none );
```

## Channels

The playback of a sound can be controlled by capturing its virtual channel, represented by the `SoundChannel` class.

The `AudioDevice::playSound()` method does exactly the same as `playSoundOnce()`, but returns the sound's channel. Since a sound channel is reference-counted, the sound will play as long as the channel is referenced. Capturing a channel allows you, for example, to control the sound's volume, fade, playback speed, panning and looping.

## Inaudible Behavior

The audio device has a fixed limit of sounds that can be played simultaneously.

If there are too many sounds playing at once, it will kill some sounds based on a heuristic, for example whether a sound
is inaudible among all of the other sounds.

The `SoundInaudibleBehavior` enum defines what should happen with a channel in such a case and is set on a sound channel by using `SoundChannel::setInaudibleBehavior()`.

## Music

`playSoundOnce()` is typically used to play fire-and-forget sounds such as explosions or UI navigation.

To play a continuous sound, such as music, a convenience method `AudioDevice::playSoundInBackground()` is provided.

::: callout info
Polly makes no distinction between sounds and songs. All types are represented by the `Sound` class.
:::

Example:

```cpp
auto song = Sound("Level1Song.mp3");

// Start playing the song in the background.
auto channel = audio.playSoundInBackground(song);
```

The song will play until it's finished, or if the channel object is not referenced anymore, or if it's programmatically stopped, for example by using `SoundChannel::setIsPaused()` or `AudioDevice::pauseAllSounds()`.

You could also let it loop using `channel.setIsLooping(true)`.

## Disabling the Audio Device

When creating a game instance, the `GameInitArgs` struct may be used to control whether an audio device should be created at all.

If `false` is specified for `enableAudio`, a valid `AudioDevice` object will still be created, but the internal audio backend will remain uninitialized. Playing sounds through such a "null" device, or using it otherwise, will be a no-op.

### Why disable audio?

Disabling Polly's audio device allows you to mute the entire game if your game has no sound to begin with. This may help to avoid audio processing overhead. Additionally, it helps you to integrate a standalone audio engine into your game instead of using Polly's audio API.

