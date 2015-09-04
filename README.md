A not-so-simple snakes game.  

This software is licensed under the ISC license,
See LICENSE for full text.

Current UI is ncurses based, but should be simple to add SDL or Xlib based
interfaces.  More of a snake-like game framework than a game, most behaviors
and values can be easily changed by editing config.h or uiconfig.h

Controls
	hjkl or arrow keys may be used to turn the snake
	p can be used to (un)pause the game
	d for debug mode
	q will quit the game

Installing
	Only dependency is ncurses (and a libc).
	A simple:
		make
		make install
	Is all you need to install it.
