#if 0

#// Library that can be used to prevent (SDL) games or other Xlib clients from
#// actively grabbing the keyboard.
#//
#//
#// Requires: gcc, Xlib + headers
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

#// Select the architectures on multilib systems
case "$(uname -m)" in
	*86*)
		arches=''
		[ -f '/lib/ld-linux.so.2' ]          && arches="$arches 32"
		[ -f '/lib64/ld-linux-x86-64.so.2' ] && arches="$arches 64"
		;;
	*)
		arches='default'
		;;
esac


#// Compile the LD_PRELOAD libraries
for arch in $arches ; do
	if [ $arch = default ] ; then
		dir="$out"
		flag=''
		bitness=''
	else
		dir="$out/$arch"
		flag="-m$arch"
		bitness="$arch-bit "
	fi
	lib="$dir/$name"
	mkdir -p "$dir"
	if [ "$self" -nt "$lib" ] ; then
		echo "Compiling ${bitness}${name}..."
		gcc -shared -fPIC $flag "$self" -o "$lib" \
		    -lX11 -static-libgcc -O3 -Wall -Wextra \
			|| echo "Warning: could not compile ${bitness}library"
	fi
	export LD_LIBRARY_PATH="$dir:$LD_LIBRARY_PATH"
done

#// Run the executable
export LD_PRELOAD="$name:$LD_PRELOAD"
[ $# = 0 ] || exec "$@"


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
