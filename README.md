# FMOD for Flax Engine

This is a port of the FMOD audio engine into the Flax Game Engine.

## Licensing
Licensing and terms of agreement need to follow the FMOD license and terms of agreement found on the [FMOD licensing webpage](https://www.fmod.com/licensing?target=_blank)

## Version
FMOD Library used is version 2.03.08

## Limitation
- Only windows has been tested.
- FMOD plugins are not supported yet.

## Installation
To add this plugin project to your game, follow the instructions in the [Flax Engine documentation](https://docs.flaxengine.com/manual/scripting/plugins/plugin-project.html#automated-git-cloning) for adding a plugin project automatically using git or manually.

## Setup
1. Install FMOD studio 2.03.08 from the [FMOD website](https://www.fmod.com/)
2. Install this plugin project into your game project.
3. Setup an environment variable for the path to where FMOD studo is intalled. Standard is named FMOD_STUDIO, but you can change what the environment variables name is in the `FmodAudioSettings` in the project.
4. Create an FMOD project. The standard location to store the FMOD project is in the game project's root directory in a folder called FMOD. If it is desirable to store the project elsewhere, in the `FmodAudioSettings` a relative path to the project can be adjusted.
5. Create a folder in the game's `Content` directory specific for FMOD. The standard is a folder located under the `Content` folder named "FMOD".
6. In `FmodAudioSettings`, set the editor build location to the folder created in step 5. This path will be relative to the game project's directory.
7. Set the same build directory in the FMOD project.
8. Once you have events in your FMOD project, in the Flax Editor top menu click the "Plugins->Build Fmod Project option". This will automatically build the FMOD project and generate `FmodEvent`s in the editor. Note: this will take a little bit. Once it is completed a FMOD alert will popup saying that the files have been generated.
9. Create an `FmodAudioSource` and an `FmodAudioListener` in a scene by right clicking in the scene heiarchy window and going to "New->Audio" context menu.. You can adjust any settings needed.
10. Assign the `FmodEvent` to the `FmodAudioSource`.
11. Disable the Flax Engine audio in the `AudioSettings`, unless you want to use both...
12. Play the scene and hear the music!

## Important classes
- `FmodAudioSource` - the Actor used to create an FMOD event instance and play the instance in the world.
- `FmodAudioListenter` - the Actor used to hear the event's play in the world. As of right now, only one audio listener is recommeded in the world.
- `FmodAudio` - a static class that can be used to interface with global parameters and devices.
- `FmodAudioSystem` - the backend system that handles creating the FMOD systems.
- `FmodAudioDevice` - a class that contains the name of an audio device.