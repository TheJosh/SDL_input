SDL_input
=========

A basic SDL input wrapper for supportinf dynamic control reassignments


When you start a game, you think about controls in a ver direct way - A = move left, D = move right, for example. This is a problem if you would like to have more complcated control schemes and/or dynamic controls.

Instead of implementing dynamic controls as a big (even more compelx) switch statement, I tried to solve the problem in a more abstract manner, thus SDL_input.

It's a C++ wrapper around the event structures.

Intead of thinking mouse moves and key presses, you instead think the final variables you care about, such as
  float player_x
  bool player_jumping
and then allow SDL_input to map the controls to the values for you.

