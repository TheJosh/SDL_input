#include "sdlinput.h"
#include <SDL.h>
#include <list>


namespace si {

	static void limit(float* val, int mode, float min, float max)
	{
		if (mode == 0) return;
		if (mode == 1) {
			if (*val < min) { *val = min; } else if (*val > max) { *val = max; }
		} else {
			float d = max - min;
			while (*val > max) { *val -= d; }
			while (*val < min) { *val += d; }
		}
	}


	/**
	* Handle SDL events. This just calls `handleEventsProfile` once for each loaded profile.
	**/
	SDL_INPUT_API void SDLInput::handleEvents(SDL_Event* e)
	{
		for (std::list<Profile*>::iterator it = this->profiles.begin(); it != this->profiles.end(); it++) {
			this->handleEventsProfile(e, (*it));
		}
	}
	
	/**
	* Keep stuff updated. This just calls `updateProfile` once for each loaded profile.
	**/
	SDL_INPUT_API void SDLInput::update()
	{
		for (std::list<Profile*>::iterator it = this->profiles.begin(); it != this->profiles.end(); it++) {
			this->updateProfile((*it));
		}
	}


	/**
	* Handle the events for a given profile.
	* This method is the crux of this whole library. Expect it to be a bit of a mess!
	**/
	void SDLInput::handleEventsProfile(SDL_Event* e, Profile *p)
	{
		// Key handler
		if (e->type == SDL_KEYDOWN || e->type == SDL_KEYUP) {
			bool down = (e->type == SDL_KEYDOWN);

			for (std::list<Binding*>::iterator it = p->bindings.begin(); it != p->bindings.end(); it++) {
				Binding* b = (*it);
				if (b->src.type != BindSrcType_Key) continue;
				
				if (b->src.param1 == e->key.keysym.sym) {
					if (b->desttype == BindDest_OnOff) {
						if (b->changefunc) (*b->changefunc)(b);
						*b->destbool = down;

					} else if (b->desttype == BindDest_Absolute || b->desttype == BindDest_Relative) {
						b->down = (down ? -1 : 0);

					} else if (b->desttype == BindDest_Toggle) {
						if (!down) {
							if (b->changefunc) (*b->changefunc)(b);
							*b->destbool = !(*b->destbool);
						}
					}

				} else if (b->src.param2 == e->key.keysym.sym) {
					b->down = (down ? 1 : 0);
				}
			}


		// Mouse button handler
		} else if (e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
			bool down = (e->type == SDL_MOUSEBUTTONDOWN);

			for (std::list<Binding*>::iterator it = p->bindings.begin(); it != p->bindings.end(); it++) {
				Binding* b = (*it);
				if (b->src.type != BindSrcType_MouseBtn) continue;
				
				if (b->src.param1 == e->button.button) {
					if (b->desttype == BindDest_OnOff) {
						if (b->changefunc) (*b->changefunc)(b);
						*b->destbool = down;

					} else if (b->desttype == BindDest_Absolute || b->desttype == BindDest_Relative) {
						b->down = (down ? -1 : 0);

					} else if (b->desttype == BindDest_Toggle) {
						if (!down) {
							if (b->changefunc) (*b->changefunc)(b);
							*b->destbool = !(*b->destbool);
						}
					}

				} else if (b->src.param2 == e->button.button) {
					b->down = (down ? 1 : 0);
				}
			}


		// Mouse moves
		} else if (e->type == SDL_MOUSEMOTION) {
			for (std::list<Binding*>::iterator it = p->bindings.begin(); it != p->bindings.end(); it++) {
				Binding* b = (*it);
				if (b->src.type != BindSrcType_MouseAxis) continue;

				float val = (b->src.param1 == 1 ? e->motion.xrel : e->motion.yrel) / p->relativePixels;

				if (b->desttype == BindDest_Absolute) {
					if (b->changefunc) (*b->changefunc)(b);
					*b->destfloat = *b->destfloat + val;
					limit(b->destfloat, b->limit, b->min, b->max);

				} else if (b->desttype == BindDest_Relative) {
					if (b->changefunc) (*b->changefunc)(b);
					*b->destfloat = val;
					limit(b->destfloat, b->limit, b->min, b->max);
					b->down = 1;
				}
			}
		}

	}


	/**
	* Keep stuff updated
	**/
	void SDLInput::updateProfile(Profile *p)
	{
		// Iterate through bindings and update as needed
		for (std::list<Binding*>::iterator it = p->bindings.begin(); it != p->bindings.end(); it++) {
			Binding* b = (*it);

			// Absolute and relative key and button bindings get updated based on their `down` figure
			if (b->src.type == BindSrcType_Key || b->src.type == BindSrcType_MouseBtn) {
				if (b->desttype == BindDest_Absolute) {
					if (b->down == -1) {
						if (b->changefunc) (*b->changefunc)(b);
						*b->destfloat = *b->destfloat - 1.0f;
						limit(b->destfloat, b->limit, b->min, b->max);

					} else if (b->down == 1) {
						if (b->changefunc) (*b->changefunc)(b);
						*b->destfloat = *b->destfloat + 1.0f;
						limit(b->destfloat, b->limit, b->min, b->max);
					}

				} else if (b->desttype == BindDest_Relative) {
					*b->destfloat = (float) b->down;
					limit(b->destfloat, b->limit, b->min, b->max);
				}


			// Relative mouse needs to be reset if not moving
			} else if (b->src.type == BindSrcType_MouseAxis && b->desttype == BindDest_Relative) {
				if (b->down == 1) {
					b->down = 2;
				} else if (b->down == 2) {
					b->down = 0;
					if (b->changefunc) (*b->changefunc)(b);
					*b->destfloat = 0.0f;
				}
			}
		}
	}


	SDL_INPUT_API BindSrc::BindSrc(std::string spec)
	{
		// TODO: change this API bit, I don't like how it's in this constructor
		// use some sort of intermediate class or something.
		// It would also be good if we could manually spec things instead of only string specs
		int type;
		if (sscanf(spec.c_str(), "%i", &type) != 1) goto specfail;

		if (type == 1) {
			char key1, key2;
			if (sscanf(spec.c_str(), "%*i %c %c", &key1, &key2) != 2) goto specfail;

			this->type = BindSrcType_Key;
			this->param1 = key1;
			this->param2 = key2;

		} else if (type == 2) {
			int axis;
			if (sscanf(spec.c_str(), "%*i %i", &axis) != 1) goto specfail;

			this->type = BindSrcType_MouseAxis;
			this->param1 = axis;

		} else if (type == 3) {
			int btn;
			if (sscanf(spec.c_str(), "%*i %i", &btn) != 1) goto specfail;

			this->type = BindSrcType_MouseBtn;
			this->param1 = btn;
		}

		return;

specfail:
	;
		//throw new std::exception("Invalid spec");
	}

	SDL_INPUT_API Binding* Profile::absolute(BindSrc src, float* destination, ValueChanged changefunc, void* userptr)
	{
		Binding* b = new Binding(src, changefunc, userptr);
		b->desttype = BindDest_Absolute;
		b->destfloat = destination;
		this->bindings.push_back(b);
		return b;
	}

	SDL_INPUT_API Binding* Profile::relative(BindSrc src, float* destination, ValueChanged changefunc, void* userptr)
	{
		Binding* b = new Binding(src, changefunc, userptr);
		b->desttype = BindDest_Relative;
		b->destfloat = destination;
		this->bindings.push_back(b);
		return b;
	}

	SDL_INPUT_API Binding* Profile::onoff(BindSrc src, bool* destination, ValueChanged changefunc, void* userptr)
	{
		Binding* b = new Binding(src, changefunc, userptr);
		b->desttype = BindDest_OnOff;
		b->destbool = destination;
		this->bindings.push_back(b);
		return b;
	}

	SDL_INPUT_API Binding* Profile::toggle(BindSrc src, bool* destination, ValueChanged changefunc, void* userptr)
	{
		Binding* b = new Binding(src, changefunc, userptr);
		b->desttype = BindDest_Toggle;
		b->destbool = destination;
		this->bindings.push_back(b);
		return b;
	}

}
