/*
 * Copyright (C) 2012 Daniel Scharrer
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the author(s) be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/*
 * Library that can be used to prevent (SDL) games or other Xlib clients from
 * actively grabbing the keyboard.
 *
 * Compile:
 *  $ gcc -shared -fPIC nokeygrab.c -o nokeygrab.so
 * Add -m32 for use with 32-bit binaries
 *
 * Use:
 *  $ LD_PRELOAD="$(pwd)/nokeygrab.so" sdl-program
 */

#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>

#define GRAB_POINTER 0

/*! XGrabKeyboard replacement that ignores all requests */
int XGrabKeyboard(Display * display, Window grab_window, Bool owner_events,
                  int pointer_mode, int keyboard_mode, Time time) {
	fprintf(stderr, "[nokeygrab] prevented keyboard grab\n");
	return GrabSuccess;
}

#if GRAB_POINTER
int XGrabPointer(Display * display, Window grab_window, Bool owner_events,
                 unsigned int event_mask, int pointer_mode, int keyboard_mode,
                 Window confine_to, Cursor cursor, Time time) {
	fprintf(stderr, "[nokeygrab] prevented pointer grab\n");
	return GrabSuccess;
}
#endif

/*
 * SDL loads all Xlib functions dynamically
 * We need to intercept those dlsym calls and inject our own XGrabKeyboard
 */

/*! internal libc function for the original behavior of dlsym */
void * __libc_dlsym(void * handle, const char * name);

void * dlsym(void * handle, const char * name) {
	
	if(!strcmp(name, "XGrabKeyboard")) {
		fprintf(stderr, "[nokeygrab] dlsym XGrabKeyboard\n");
		return &XGrabKeyboard;
	}
	
#if GRAB_POINTER
	if(!strcmp(name, "XGrabPointer")) {
		fprintf(stderr, "[nokeygrab] dlsym XGrabPointer\n");
		return &XGrabPointer;
	}
#endif
	
	return __libc_dlsym(handle, name);
}

/* Let the user know that the library was LD_PRELOADed successfully */

void init_nokeygrab() __attribute__((constructor));

void init_nokeygrab() {
	fprintf(stderr, "[nokeygrab] attached to process\n");
}
