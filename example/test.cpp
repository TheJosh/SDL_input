#include "../src/sdlinput.h"
#include "SDL_picofont.h"
#include <stdio.h>
#include <SDL.h>


#ifdef _WIN32
#include <windows.h>
#include <math.h>
#endif


static bool running = true;

void exithdlr(si::Binding* b)
{
	running = false;
}


int main(int argc, char ** argv)
{
	bool jump = false;
	bool menu = false;
	bool dummy = false;
	float aim = 0.0f;
	float move = 0.0f;
	float x = 0.0f;
	float y = 0.0f;


	// The SDLInput class handles all the state and has the methods which do the gruntwork
	si::SDLInput* input = new si::SDLInput();

	// Create a profile. You can have as many profiles loaded at a time as you want.
	// So you could create a profile for general use, and one for driving vehicles.
	// You can even hoop up all the handlers and pointers. Then just load and unload
	// the profiles when the player enters or leaves a vehicle.
	si::Profile* prof = new si::Profile();
	input->addProfile(prof);

	// Keys. Absolute and relative simulate an axis and need two keys.
	// Use the SDLKey constants.
	prof->onoff(si::BindSrc(si::BindSrcType_Key, SDLK_SPACE), &jump);
	prof->toggle(si::BindSrc(si::BindSrcType_Key, SDLK_LSHIFT), &menu);
	prof->absolute(si::BindSrc(si::BindSrcType_Key, SDLK_o, SDLK_p), &aim)->setLimit(-5.0f, 10.0f);
	prof->relative(si::BindSrc(si::BindSrcType_Key, SDLK_a, SDLK_d), &move);

	// Mouse buttons. Like keys, when simulating an axis, you need two buttons.
	// 1 = left, 2 = center, 3 = right, 4/5 = wheel, others perhaps too (6/7 = side buttons).
	// The wheel didn't work in my testing, but you might get lucky.
	prof->onoff(si::BindSrc(si::BindSrcType_MouseBtn, 1), &jump);
	prof->toggle(si::BindSrc(si::BindSrcType_MouseBtn, 3), &menu);
	prof->absolute(si::BindSrc(si::BindSrcType_MouseBtn, 6, 7), &aim);

	// Mouse axis map really nicely to absolute and relative values.
	// You can't map to binary values at the moment.
	prof->absolute(si::BindSrc(si::BindSrcType_MouseAxis, 1), &x)->setWrap(0, 360);
	prof->relative(si::BindSrc(si::BindSrcType_MouseAxis, 2), &y)->setLimit(-0.5f, 0.5f);

	// We use a changehandler for clicking of the ESC key.
	prof->onoff(si::BindSrc(si::BindSrcType_Key, SDLK_ESCAPE), &dummy, &exithdlr);

	
	// Init the demo window.
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface* screen = SDL_SetVideoMode(430, 200, 32, 0);
	
	// These two lines make relative mouse movement work properly.
	SDL_ShowCursor(0);
	SDL_WM_GrabInput(SDL_GRAB_ON);

	// This is for the status text
	char buf[255];
	SDL_Color c = {255,255,255,0};

	
	while (running) {
		// Some of the code needs to run reguarlly, even when there isn't events.
		input->update();

		// The main method is `handleEvents`. You pass it an event object.
		// You can still feel free to handle events yourself as well, if you want.
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			} else {
				input->handleEvents(&event);
			}
		}

		
		SDL_FillRect(screen,NULL,0);

		// Display the status variables
		sprintf(buf, "jump = %i\n\n\nmenu = %i\n\n\naim = %f\n\n\nmove = %f\n\n\nx = %f\n\ny = %f", (int)jump, (int)menu, aim, move, x, y);
		SDL_Surface* info = FNT_Render(buf, c);
		SDL_Rect r = {10, 10, 0, 0};
		SDL_BlitSurface(info,NULL,screen,&r);
		SDL_FreeSurface(info);

		// Display some info on the controls
		info = FNT_Render(
			"Jump (on/off):\nSpace, Left mouse\n\n"
			"Menu (toggle):\nLeft shift, Right mouse\n\n"
			"Aim (absolute):\nO and P, Mouse sidebuttons\n\n"
			"Move (relative):\nA and D\n\n"
			"X is absolute\n\n"
			"Y is relative\n\n\n"
			"ESC to quit", c);
		r.x = 200;
		SDL_BlitSurface(info,NULL,screen,&r);
		SDL_FreeSurface(info);

		// Flip and delay so we use less cpu
		SDL_Flip(screen);
		SDL_Delay(50);
	}

	SDL_Quit();
	
	
	// Don't forget to clean up!
	delete(prof);
	delete(input);

	return 0;
}
