#if 0

#/*
# * Force MSAA for the backbuffer in games that either don't support MSAA at all or don't
# * know how to select the correct GLXFBConfig.
# *
# * R.I.P. GALLIUM_MSAA :(  http://cgit.freedesktop.org/mesa/mesa/commit/?h=844912
# *
# * Known uses:
# *  - Neverwinter Nights
# *  - Pillars of Eternity
# *  - Thomas was Alone
# *
# * Usage: sh forcemsaa.c [path/to/game/binary]
# *
# * You can copy this file to the game's Steam install and then set the
# * launch options under Properties → Set Launch Options... to
# *   sh forcemsaa.c %command%
# *
# * Requires:
# *  - GCC with support for compiling 32-bit binaries
# *  - 64-bit and 32-bit OpenGL development files (mesa)
# *  - pkg-config
# *
# * Ubuntu users may or may not be able to install these using
# *   sudo apt-get install gcc-multilib libgl1-mesa-dev libgl1-mesa-dev:i386 pkg-config
# */

self="$(command -v "$0")" ; self="${self:-$0}" ; self="$(readlink -f "$self")"
name="${self##*/}" ; name="${name%.c}"
out="${self%/*}/$name"
soname="$name.so"

#/* Compile the LD_PRELOAD library */
for arch in 32 64 ; do
	if [ ! -f "$out/$arch/$soname" ] || [ "$self" -nt "$out/$arch/$soname" ] ; then
		echo "Compiling $arch-bit $soname..."
		mkdir -p "$out/$arch"
		pkg_config_path="/usr/lib$arch/pkgconfig:/usr/lib/pkgconfig"
		case $arch in
			32) pkg_config_path="/usr/lib/i386-linux-gnu/pkgconfig/:$PKG_CONFIG_PATH" ;;
			64) pkg_config_path="/usr/lib/x86_64-linux-gnu/pkgconfig/:$PKG_CONFIG_PATH" ;;
		esac
		command='"${CC:-gcc}" -shared -std=c99 -fPIC -m$arch -x c -O3 -Wall -Wextra'
		command+=' "$self" -o "$out/$arch/$soname" -DLOG_PREFIX="\"[$name] \""'
		command+=" $(PKG_CONFIG_PATH="$pkg_config_path" pkg-config --cflags --libs gl)"
		eval "$command" || exit 1
	fi
	export LD_LIBRARY_PATH="$out/$arch:$LD_LIBRARY_PATH"
done

#/* Run the executable */
export LD_PRELOAD="$LD_PRELOAD:$soname"
[ -z "$1" ] || exec "$@"

exit

#endif /* 0 */



#define FORCE_MSAA_SAMPLES 8
// #define PREVENT_MSAA_GLDISABLE



#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <dlfcn.h>
#include <GL/glx.h>
#include <GL/gl.h>

typedef void(*funcptr_t)();
typedef funcptr_t (*glXGetProcAddress_t)(const GLubyte * procName);
static glXGetProcAddress_t real_glXGetProcAddress;

static void * get_proc(const char * name) {
	
	if(!real_glXGetProcAddress) {
		return NULL;
	}
	
	return real_glXGetProcAddress((const GLubyte *)name);
}

typedef XVisualInfo * (*glXChooseVisual_t)(Display * dpy, int screen, int * attrib_list);

typedef GLXFBConfig * (*glXChooseFBConfig_t)(Display * dpy, int screen,
                                             const int * attrib_list, int * nelements);

#ifdef FORCE_MSAA_SAMPLES

static int has_value(int fbconfig, int attrib) {
	
	if(fbconfig) {
		return 1;
	}
	
	switch(attrib) {
		case GLX_USE_GL:
		case GLX_RGBA:
		case GLX_DOUBLEBUFFER:
		case GLX_STEREO:
		case GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB:
			return 0;
		case GLX_BUFFER_SIZE:
		case GLX_LEVEL:
		case GLX_AUX_BUFFERS:
		case GLX_RED_SIZE:
		case GLX_GREEN_SIZE:
		case GLX_BLUE_SIZE:
		case GLX_ALPHA_SIZE:
		case GLX_DEPTH_SIZE:
		case GLX_STENCIL_SIZE:
		case GLX_ACCUM_RED_SIZE:
		case GLX_ACCUM_GREEN_SIZE:
		case GLX_ACCUM_BLUE_SIZE:
		case GLX_ACCUM_ALPHA_SIZE:
		case GLX_X_VISUAL_TYPE:
		case GLX_CONFIG_CAVEAT:
		case GLX_TRANSPARENT_TYPE:
		case GLX_TRANSPARENT_INDEX_VALUE:
		case GLX_TRANSPARENT_RED_VALUE:
		case GLX_TRANSPARENT_GREEN_VALUE:
		case GLX_TRANSPARENT_BLUE_VALUE:
		case GLX_TRANSPARENT_ALPHA_VALUE:
		case GLX_DRAWABLE_TYPE:
		case GLX_RENDER_TYPE:
		case GLX_SAMPLE_BUFFERS:
		case GLX_SAMPLES:
			return 1;
	}
	
	fprintf(stderr, LOG_PREFIX "!! unsupported glXChooseVisual attrib: 0x%x\n", attrib);
	return 0; // lets hope that if there is a value, it doesn match a known attrib
}

static int * fix_attrib_list(const int * attrib_list, int fbconfig) {
	
	int count = 0;
	while(attrib_list[count] != None) {
		count += 1 + has_value(fbconfig, attrib_list[count]);
	}
	
	int * new_attrib_list = malloc(sizeof(int) * (count + 1 + 4));
	if(!new_attrib_list) {
		return NULL;
	}
	
	int out = 0;
	for(int i = 0; attrib_list[i] != None; ) {
		if(attrib_list[i] == GLX_SAMPLE_BUFFERS || attrib_list[i] == GLX_SAMPLES) {
			// Ignore values requested by the program.
			// (Unity sets GLX_DONT_CARE for both of these.)
			i += 2;
		} else {
			if(has_value(fbconfig, attrib_list[i])) {
				new_attrib_list[out++] = attrib_list[i++];
			}
			new_attrib_list[out++] = attrib_list[i++];
		}
	}
	new_attrib_list[out++] = GLX_SAMPLE_BUFFERS;
	new_attrib_list[out++] = 1;
	new_attrib_list[out++] = GLX_SAMPLES;
	new_attrib_list[out++] = FORCE_MSAA_SAMPLES;
	new_attrib_list[out++] = None;
	
	return new_attrib_list;
}

// This is only needed for older (pre-GLX 1.4) clients
static XVisualInfo * redir_glXChooseVisual(Display * dpy, int screen, int * attrib_list) {
	
	glXChooseVisual_t real_glXChooseVisual = get_proc("glXChooseVisual");
	if(!real_glXChooseVisual) {
		fprintf(stderr, LOG_PREFIX "!! could not get real glXChooseVisual\n");
		return NULL;
	}
	
	int * new_attrib_list = fix_attrib_list(attrib_list, 0);
	if(!new_attrib_list) {
		return NULL;
	}
	
	fprintf(stderr, LOG_PREFIX "injected MSAA config for glXChooseVisual\n");
	
	XVisualInfo * ret = real_glXChooseVisual(dpy, screen, new_attrib_list);
	
	free(new_attrib_list);
	
	return ret;
}
XVisualInfo * glXChooseVisual(Display * dpy, int screen, int * attrib_list) {
	return redir_glXChooseVisual(dpy, screen, attrib_list);
}

static GLXFBConfig * redir_glXChooseFBConfig(Display * dpy, int screen,
                                             const int * attrib_list, int * nelements) {
	
	glXChooseFBConfig_t real_glXChooseFBConfig = get_proc("glXChooseFBConfig");
	if(!real_glXChooseFBConfig) {
		fprintf(stderr, LOG_PREFIX "!! could not get real glXChooseFBConfig\n");
		*nelements = 0;
		return NULL;
	}
	
	int * new_attrib_list = fix_attrib_list(attrib_list, 1);
	if(!new_attrib_list) {
		*nelements = 0;
		return NULL;
	}
	
	fprintf(stderr, LOG_PREFIX "injected MSAA config for glXChooseFBConfig\n");
	
	GLXFBConfig * ret = real_glXChooseFBConfig(dpy, screen, new_attrib_list, nelements);
	
	free(new_attrib_list);
	
	return ret;
}
GLXFBConfig * glXChooseFBConfig(Display * dpy, int screen, const int * attrib_list,
                                int * nelements) {
	return redir_glXChooseFBConfig(dpy, screen, attrib_list, nelements);
}

#else

static int msaa_samples(Display * dpy, GLXFBConfig config) {
	int msaa = 0;
	if(glXGetFBConfigAttrib(dpy, config, GLX_SAMPLES, &msaa)) {
		return 0;
	}
	return msaa;
}

static int compare_fbconfig(const void * a, const void * b, void * dpy) {
	
	int a_samples = msaa_samples((Display *)dpy, *(GLXFBConfig *)a);
	int b_samples = msaa_samples((Display *)dpy, *(GLXFBConfig *)b);
	
	return b_samples - a_samples;
}

static GLXFBConfig * redir_glXChooseFBConfig(Display * dpy, int screen,
                                             const int * attrib_list, int * nelements) {
	
	glXChooseFBConfig_t real_glXChooseFBConfig = get_proc("glXChooseFBConfig");
	if(!real_glXChooseFBConfig) {
		*nelements = 0;
		return NULL;
	}
	
	GLXFBConfig * ret = real_glXChooseFBConfig(dpy, screen, attrib_list, nelements);
	
	/* sort the config list inversely by msaa samples */
	qsort_r(ret, *nelements, sizeof(GLXFBConfig), compare_fbconfig, dpy);
	
	fprintf(stderr, LOG_PREFIX "sorted configs from glXChooseFBConfig\n");
	
	return ret;
}
GLXFBConfig * glXChooseFBConfig(Display * dpy, int screen, const int * attrib_list,
                                int * nelements) {
	return redir_glXChooseFBConfig(dpy, screen, attrib_list, nelements);
}

#endif

#ifdef PREVENT_MSAA_GLDISABLE

typedef void (*glDisable_t)(GLenum value);

// This is only needed for older (pre-GLX 1.4) clients
static void redir_glDisable(GLenum value) {
	
	if(value == GL_MULTISAMPLE) {
		return;
	}
	
	glDisable_t real_glDisable = get_proc("glDisable");
	if(real_glDisable) {
		real_glDisable(value);
	}
}
void glDisable(GLenum value) {
	redir_glDisable(value);
}

#endif

// Hooking boilerplate code follows

typedef void * (*dlsym_t)(void * handle, const char * name);
static dlsym_t real_dlsym;

typedef void * (*dlvsym_t)(void *handle, const char * symbol, const char * version);
static dlvsym_t real_dlvsym;

static glXGetProcAddress_t real_glXGetProcAddressARB;

/*! internal libc function for the original behavior of dlsym */
#define __RTLD_DLOPEN 0x80000000
#define __libc_dlopen(name) \
	__libc_dlopen_mode (name, RTLD_LAZY | __RTLD_DLOPEN)
void * __libc_dlopen_mode(const char *__name, int __mode);
void * __libc_dlsym(void * handle, const char * name);

extern char * program_invocation_short_name; // provided by glibc

static void hook_init(void) __attribute__((constructor));
static void hook_init(void) {
	// Get the original dlsym (__libc_dlsym doesn't support RTLD_NEXT)
	void * libdl = __libc_dlopen("libdl.so.2");
	if(libdl) {
		real_dlsym = (dlsym_t)__libc_dlsym(libdl, "dlsym");
	}
	if(real_dlsym) {
		// Get the dlsym from any LD_PRELOAD library loaded before us
		void * ptr = real_dlsym(RTLD_NEXT, "dlsym");
		if(ptr) {
			real_dlsym = (dlsym_t)ptr;
		}
		// Get other overwritten symbols
		real_dlvsym = real_dlsym(RTLD_NEXT, "dlvsym");
		real_glXGetProcAddress
			= (glXGetProcAddress_t)real_dlsym(RTLD_NEXT, "glXGetProcAddress");
		real_glXGetProcAddressARB
			= (glXGetProcAddress_t)real_dlsym(RTLD_NEXT, "glXGetProcAddressARB");
		if(!real_glXGetProcAddress) {
			real_glXGetProcAddress = real_glXGetProcAddressARB;
		}
	}
	/*
	fprintf(stderr, LOG_PREFIX "attached to %s: %p %p %p %p\n",
		program_invocation_short_name, real_dlsym, real_dlvsym,
		real_glXGetProcAddress, real_glXGetProcAddressARB);
	*/
}

static void * my_dlsym(const char * name, const char * hook);

static void * redir_dlsym(void * handle, const char * name) {
	
	void * ptr = my_dlsym(name, "dlsym");
	if(ptr) {
		return ptr;
	}
	
	if(real_dlsym) {
		return real_dlsym(handle, name);
	} else {
		return NULL;
	}
}
void * dlsym(void * handle, const char * name) {
	return redir_dlsym(handle, name);
}

static void * redir_dlvsym(void * handle, const char * symbol, const char * version) {
	
	void * ptr = my_dlsym(symbol, "dlvsym");
	if(ptr) {
		return ptr;
	}
	
	if(real_dlvsym) {
		return real_dlvsym(handle, symbol, version);
	} else {
		return NULL;
	}
}
void * dlvsym(void * handle, const char * symbol, const char * version) {
	return redir_dlvsym(handle, symbol, version);
}

static funcptr_t redir_glXGetProcAddress(const GLubyte * procName) {
	
	void * ptr = my_dlsym((const char *)procName, "glXGetProcAddress");
	if(ptr) {
		return (funcptr_t)ptr;
	}
	
	if(real_glXGetProcAddress) {
		return real_glXGetProcAddress(procName);
	} else {
		return NULL;
	}
}
funcptr_t glXGetProcAddress(const GLubyte * procName) {
	return redir_glXGetProcAddress(procName);
}

static funcptr_t redir_glXGetProcAddressARB(const GLubyte * procName) {
	
	void * ptr = my_dlsym((const char *)procName, "glXGetProcAddressARB");
	if(ptr) {
		return (funcptr_t)ptr;
	}
	
	if(real_glXGetProcAddressARB) {
		return real_glXGetProcAddressARB(procName);
	} else if(real_glXGetProcAddress) {
		return real_glXGetProcAddress(procName);
	} else {
		return NULL;
	}
}
funcptr_t glXGetProcAddressARB(const GLubyte * procName) {
	return redir_glXGetProcAddressARB(procName);
}

static void * my_dlsym(const char * name, const char * hook) {
	
	if(name) {
		#ifdef FORCE_MSAA_SAMPLES
		if(!strcmp(name, "glXChooseVisual")) {
			fprintf(stderr, LOG_PREFIX "hooked %s via %s\n", name, hook);
			return (void *)redir_glXChooseVisual;
		}
		#endif
		if(!strcmp(name, "glXChooseFBConfig")) {
			fprintf(stderr, LOG_PREFIX "hooked %s via %s\n", name, hook);
			return (void *)redir_glXChooseFBConfig;
		}
		#ifdef PREVENT_MSAA_GLDISABLE
		if(!strcmp(name, "glDisable")) {
			fprintf(stderr, LOG_PREFIX "hooked %s via %s\n", name, hook);
			return (void *)redir_glDisable;
		}
		#endif
		if(!strcmp(name, "dlsym")) {
			return (void *)redir_dlsym;
		}
		if(!strcmp(name, "dlvsym")) {
			return (void *)redir_dlvsym;
		}
		if(!strcmp(name, "glXGetProcAddress")) {
			return (void *)redir_glXGetProcAddress;
		}
		if(!strcmp(name, "glXGetProcAddressARB")) {
			return (void *)redir_glXGetProcAddressARB;
		}
	}
	
	return NULL;
}
