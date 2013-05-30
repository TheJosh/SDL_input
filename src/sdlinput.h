#pragma once
#include <SDL.h>
#include <list>
#include <string>


#ifdef _WIN32
	#ifndef SDL_INPUT_DLL
	#define SDL_INPUT_API __declspec( dllimport )
	#else
	#define SDL_INPUT_API __declspec( dllexport )
	#endif
#else
	#define SDL_INPUT_API
#endif


namespace si {
	
	class SDL_INPUT_API Binding;
	class SDL_INPUT_API Profile;
	class SDL_INPUT_API SDLInput;
	class SDL_INPUT_API BindSrc;

	enum BindSrcType {
		BindSrcType_Key = 1,
		BindSrcType_MouseAxis = 2,
		BindSrcType_MouseBtn = 3,
	};
	
	enum BindDest {
		BindDest_Undefined = 0,
		BindDest_Absolute = 1,
		BindDest_Relative = 2,
		BindDest_OnOff = 3,
		BindDest_Toggle = 4,
	};
	
	typedef void (*ValueChanged)(si::Binding* binding);

	class SDL_INPUT_API SDLInput
	{
		protected:
			std::list<Profile*> profiles;

		private:
			void handleEventsProfile(SDL_Event* e, Profile *p);
			void updateProfile(Profile *p);
			public:
			SDLInput() {}
			~SDLInput() {}
			
		public:
			void addProfile(Profile* value) { this->profiles.push_back(value); }
			void removeProfile(Profile* value) { this->profiles.remove(value); }
			
		public:
			/**
			* This is the main event handler method.
			*
			* It takes SDL events and dispatches them to the bound handlers
			* of the currently connected profiles.
			**/
			void handleEvents(SDL_Event* e);

			/**
			* To keep keyboard analog events working correctly,
			* their state needs to be updated reguarlly.
			* Call this method at regular intervals
			**/
			void update();
	};
	
	class SDL_INPUT_API Profile
	{
		friend class SDLInput;

		protected:
			void* userptr;
			std::list<Binding*> bindings;
			float relativePixels;

		public:
			Profile() : userptr(NULL), relativePixels(100) {}
			~Profile() {}
			void* getUserptr() { return this->userptr; }
			void setUserptr(void* value) { this->userptr = value; }
			float getRelativePixels() { return this->relativePixels; }

			/**
			* For mouse relative and absolute, the number of pixels for a value of 1.0
			**/
			void setRelativePixels(float value) { this->relativePixels = value; }

		public:
			Binding* absolute(BindSrc src, float* destination, ValueChanged changefunc = NULL, void* userptr = NULL);
			Binding* relative(BindSrc src, float* destination, ValueChanged changefunc = NULL, void* userptr = NULL);
			Binding* onoff(BindSrc src, bool* destination, ValueChanged changefunc = NULL, void* userptr = NULL);
			Binding* toggle(BindSrc src, bool* destination, ValueChanged changefunc = NULL, void* userptr = NULL);
	};


	class SDL_INPUT_API BindSrc
	{
		friend class Binding;
		friend class SDLInput;

		protected:
			BindSrcType type;			// The source type (keyboard, mouse axis, etc)
			int param1;					// First (only?) param of the source
			int param2;					// Some sources have two params

		public:
			BindSrc(BindSrcType type, int param1, int param2 = 0) : type(type), param1(param1), param2(param2) {}
			BindSrc(std::string spec);
			~BindSrc() {}
	};

	class SDL_INPUT_API Binding
	{
		friend class Profile;
		friend class SDLInput;
		
		protected:
			void* userptr;				// For whatever the user want to do with
			BindSrc src;				// The source of the binding
			BindDest desttype;			// Destination type (digital, analog)
			float* destfloat;			// Pointer to float to store analog values in
			bool* destbool;				// Pointer to fload to store digital values in
			int down;					// Used by analog keyboard presses; -1 = down, 0 = neither, 1 = up
			ValueChanged changefunc;	// Called every time the state changes
			float min, max;				// For absolute & relative, the min and max values
			int limit;					// Limit mode for abs and rel; 0 = none, 1 = limited, 2 = wrap around

		protected:
			Binding(BindSrc src, ValueChanged changefunc, void* userptr)
				: userptr(userptr), src(src), desttype(BindDest_Undefined),
				  destfloat(NULL), destbool(NULL), down(0), changefunc(changefunc),
				  min(0.0f), max(0.0f), limit(0)
				{}
			~Binding() {}

		public:
			void* getUserptr() { return this->userptr; }
			void setUserptr(void* value) { this->userptr = value; }

			bool getValueBool() { return *destbool; }
			bool* getRefBool() { return destbool; }
			void setRefBool(bool* ref) { destbool = ref; }

			float getValueFloat() { return *destfloat; }
			float* getRefFloat() { return destfloat; }
			void setRefFloat(float* ref) { destfloat = ref; }

			void setFree() { this->limit = 0; }
			void setLimit(float min, float max) { this->limit = 1; this->min = min; this->max = max; }
			void setWrap(float min, float max) { this->limit = 2; this->min = min; this->max = max; }
	};

}
