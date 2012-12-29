#if 0

#// Library that can be used to prevent (SDL) games or other Xlib clients from
#// actively grabbing the keyboard.
#//
#//
#// Requires: gcc with support for x86 targets, Xlib + headers
#//
#//
#// Use:
#// $ chmod +x nokeygrab.c
#// and then
#//
#//
#// $ ./nokeygrab.c <program>
#//
#//
#// This program is free software. It comes without any warranty, to
#// the extent permitted by applicable law. You can redistribute it
#// and/or modify it under the terms of the Do What The Fuck You Want
#// To Public License, Version 2, as published by Sam Hocevar. See
#// http://sam.zoy.org/wtfpl/COPYING for more details.
#//

self="$(readlink -f "$(which "$0")")"
out="$(dirname "$self")/build"
name="$(basename "$self" .c).so"


#// Compile the LD_PRELOAD library
for arch in 32 64 ; do
	dir="$out/$arch"
	lib="$dir/$name"
	mkdir -p "$dir"
	if [ "$self" -nt "$lib" ] ; then
		echo "Compiling $arch-bit $name..."
		gcc -shared -fPIC -m$arch "$self" -o "$lib" \
		    -lX11 -static-libgcc -O3 -Wall -Wextra \
			|| echo "Warning: could not compile $arch-bit library"
	fi
done

#// Run the executable
export LD_LIBRARY_PATH="$out/32:$out/64"
export LD_PRELOAD="$name"
[ -z "$1" ] || exec "$@"


exit

#endif // 0

#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>

#define PREVENT_POINTER_GRAB 0

extern char * program_invocation_short_name; // provided by glibc

/*! XGrabKeyboard replacement that ignores all requests */
int XGrabKeyboard(Display * display, Window grab_window, Bool owner_events,
                  int pointer_mode, int keyboard_mode, Time time) {
	(void)display, (void)grab_window, (void) owner_events;
	(void)pointer_mode, (void)keyboard_mode, (void)time;
	fprintf(stderr, "[nokeygrab] prevented keyboard grab in %s\n",
	        program_invocation_short_name);
	return GrabSuccess;
}

#if PREVENT_POINTER_GRAB
/*! XGrabPointer replacement that ignores all requests */
int XGrabPointer(Display * display, Window grab_window, Bool owner_events,
                 unsigned int event_mask, int pointer_mode, int keyboard_mode,
                 Window confine_to, Cursor cursor, Time time) {
	(void)display, (void)grab_window, (void) owner_events;
	(void)event_mask, (void)pointer_mode, (void)keyboard_mode;
	(void)confine_to, (void)cursor, (void)time;
	fprintf(stderr, "[nokeygrab] prevented pointer grab in %s\n",
	        program_invocation_short_name);
	return GrabSuccess;
}
#endif

/*
 * Some programs (e.g. those using SDL) load all Xlib functions dynamically
 * We need to intercept those dlsym calls and inject our own XGrabKeyboard
 */

/*! internal libc function for the original behavior of dlsym */
void * __libc_dlsym(void * handle, const char * name);

void * dlsym(void * handle, const char * name) {
	
	if(!strcmp(name, "XGrabKeyboard")) {
		fprintf(stderr, "[nokeygrab] dlsym XGrabKeyboard in %s\n",
		        program_invocation_short_name);
		return XGrabKeyboard;
	}
	
	#if PREVENT_POINTER_GRAB
	if(!strcmp(name, "XGrabPointer")) {
		fprintf(stderr, "[nokeygrab] dlsym XGrabPointer in %s\n",
		        program_invocation_short_name);
		return XGrabPointer;
	}
	#endif
	
	return __libc_dlsym(handle, name);
}

/* Let the user know that the library was LD_PRELOADed successfully */

void init_nokeygrab(void) __attribute__((constructor));

void init_nokeygrab(void) {
	fprintf(stderr, "[nokeygrab] attached to %s\n", program_invocation_short_name);
}
