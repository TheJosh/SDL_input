SDL_input
=========

A basic SDL input wrapper for supporting dynamic control reassignments


When you start a game, you think about controls in a very direct way - A = move left, D = move right, for example. This is a problem if you would like to have more complcated control schemes and/or dynamic controls.

Instead of implementing dynamic controls as a big (even more complex) switch statement, I tried to solve the problem in a more abstract manner, thus SDL_input.

It's a C++ wrapper around the event structures.

Intead of thinking mouse moves and key presses, you instead think the final variables you care about, such as
`float player_x` or `bool player_jumping`
and then allow SDL_input to map the controls to the values for you.


How it works
------------
You define one or more `Profile`s. These can be enabled and disabled at will.

Each profile contains one or more `Binding`s. These bindings have a type and a source.

The type can be one of:
* onoff - binary, is the key/mouse button currently pressed
* toggle - binary, toggles on each key press/mouse button press
* absolute value - increments/decrements according to movement
* relative value - represents how much movement has happened since last update

The binding sources can be key presses, mouse button clicks and mouse axises. Joystick and gamepad support is planned.

Interestingly, some odd bindings are possible, such as binding a pair of keys to an absolute or relative value. They will then behave as a simulated mouse axis.
