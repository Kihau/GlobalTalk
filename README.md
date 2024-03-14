# GlobalTalk
Cross platform Push-To-Talk and Push-To-Mute/Unmute

## Notice
Currently the app works only with X11 and alsa, as I wrote the program for my use case.  
For now, you cannot change keybinds and the sound file is hard-coded and embedded into the executable.  
The only way to change the behaviour of the program is to edit the source code itself.  
In the future I might expand upon the codebase if I feel a need for it.

## (maybe) TODO:
- Proper configuration file that lets you change:
    - Keybinds.
    - Input devices used.
    - Capture and playback devices used.

- Graphical interface for configuration purposes:  
    - Running the program with the "--menu" flag will open a window with a ImGui interface.
    - The GUI will save state to the implemented config format.

- Proper implementation for different platforms, such as:
    - Linux on Wayland.
    - Windows.
    - OSX.
