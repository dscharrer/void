#if 0

#/*
# * Fix adio stutter in Grim Fandango Remastered with ALSA.
# *
# * Usage: sh grimfix.c [path/to/GrimFandango]
# *
# * You can copy this file to the Grim Fandango Steam in stall and then set the
# * launch options under Properties → Set Launch Options... to
# *   sh grimfix.c %command%
# *
# * Requires:
# *  - GCC with support for compiling 32-bit binaries
# *  - 32-bit SDL2 development files
# *  - pkg-config
# *
# * Ubuntu users may or may not be able to install these using
# *   sudo apt-get install gcc-multilib libsdl2-dev:i386 pkg-config
# */

[ $# = 0 ] && set -- ./GrimFandango

self="$(command -v "$0")" ; self="${self:-$0}" ; self="$(readlink -f "$self")"
name="${self##*/}" ; name="${name%.c}"
out="${self%/*}/$name"
soname="$name.so"

#/* Compile the LD_PRELOAD library */
for arch in 32 ; do
	mkdir -p "$out/$arch"
	if [ ! -f "$out/$arch/$soname" ] || [ "$self" -nt "$out/$arch/$soname" ] ; then
		echo "Compiling $arch-bit $soname..."
		export GRIMFIX_SOURCE="$self"
		export GRIMFIX_LIB="$out/$arch/$soname"
		export PKG_CONFIG_PATH="/usr/lib$arch/pkgconfig:/usr/lib/pkgconfig"
		case $arch in
			32) export PKG_CONFIG_PATH="/usr/lib/i386-linux-gnu/pkgconfig/:$PKG_CONFIG_PATH" ;;
			64) export PKG_CONFIG_PATH="/usr/lib/x86_64-linux-gnu/pkgconfig/:$PKG_CONFIG_PATH" ;;
		esac
		command="${CC:-gcc} -shared -std=c99 -fPIC -m$arch -x c -O3 -Wall -Wextra"
		command="$command \"\$GRIMFIX_SOURCE\" -o \"\$GRIMFIX_LIB\""
		command="$command $(pkg-config --cflags --libs sdl2)"
		eval "$command" || exit 1
	fi
	export LD_LIBRARY_PATH="$out/$arch:$LD_LIBRARY_PATH"
done

#/* Run the executable */
export LD_PRELOAD="$soname:$LD_PRELOAD"
[ -z "$1" ] || exec "$@"

exit

#endif /* 0 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <dlfcn.h>
#include <SDL.h>

typedef SDL_AudioDeviceID (*SDL_OpenAudioDevice_t)(const char *, int,
                                                   const SDL_AudioSpec *,
                                                   SDL_AudioSpec *, int);
static SDL_OpenAudioDevice_t real_SDL_OpenAudioDevice;

SDL_AudioDeviceID SDL_OpenAudioDevice(const char * device, int iscapture,
                                      const SDL_AudioSpec * desired,
                                      SDL_AudioSpec * obtained, int allowed_changes) {
	
	SDL_AudioSpec fixed;
	memcpy(&fixed, desired, sizeof(SDL_AudioSpec));
	
	/*
	 * "Fix" sample rate
	 * SDL's pulse backend halves the requested sample count and GrimFandango depends on this
	 * https://github.com/spurious/SDL-mirror/blob/master/src/audio/pulseaudio/SDL_pulseaudio.c#L413
	 */
	fixed.samples /= 2;
	
	if(!real_SDL_OpenAudioDevice) {
		void * func = dlsym(RTLD_NEXT, "SDL_OpenAudioDevice");
		real_SDL_OpenAudioDevice = (SDL_OpenAudioDevice_t)func;
	}
	
	return real_SDL_OpenAudioDevice(device, iscapture, &fixed, obtained, allowed_changes);
}
