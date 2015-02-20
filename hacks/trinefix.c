#if 0

#//  The Steam overlay breaks this :(

self="$(readlink -f "$(which "$0")")"
name="$(basename "$self" .c)"
out="$(dirname "$self")/$name"
soname="$name.so"

#// Compile the LD_PRELOAD library
for arch in 32 64 ; do
	mkdir -p "$out/$arch"
	if [ "$self" -nt "$out/$arch/$soname" ] ; then
		echo "Compiling $arch-bit $soname..."
		gcc -shared -std=c99 -fPIC -m$arch -x c "$self" -o "$out/$arch/$soname" \
		    -lGL -O3 -Wall -Wextra \
			|| exit 1
	fi
	export LD_LIBRARY_PATH="$out/$arch:$LD_LIBRARY_PATH"
done

#// Run the executable
export LD_PRELOAD="$LD_PRELOAD:$soname" # segfaults with steam overlay if prepended
[ -z "$1" ] || exec "$@"

exit

#endif // 0


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <dlfcn.h>
#include <GL/gl.h>


// "Fix" ARB shaders

typedef void(*funcptr_t)();
typedef funcptr_t (*glXGetProcAddress_t)(const GLubyte * procName);
static glXGetProcAddress_t real_glXGetProcAddress;

typedef void (*glProgramStringARB_t)(GLenum target, GLenum format, GLsizei len,
                                     const void * string);

void glProgramStringARB(GLenum target, GLenum format, GLsizei len, const void * string) {
	
	char * str = strdup((const char *)string);
	
	// The interesting part: replace all SHADOW2D samplers with 2D samplers
	for(char * result = str; (result = strstr(result, " SHADOW2D;")) != NULL; ++result) {
		memset(result, ' ', 7);
	}
	
	if(real_glXGetProcAddress) {
		glProgramStringARB_t real_glProgramStringARB
			= (glProgramStringARB_t)real_glXGetProcAddress((const GLubyte *)"glProgramStringARB");
		if(real_glProgramStringARB) {
			real_glProgramStringARB(target, format, len, str);
		}
	}
	
	free(str);
}


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

void hook_init(void) __attribute__((constructor));
void hook_init(void) {
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
	printf("[hook] attached %p %p %p %p\n", real_dlsym, real_dlvsym,
		real_glXGetProcAddress, real_glXGetProcAddressARB);
}

void * my_dlsym(const char * name, const char * hook);

void * dlsym(void * handle, const char * name) {
	
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


void * dlvsym(void * handle, const char * symbol, const char * version) {
	
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

funcptr_t glXGetProcAddress(const GLubyte * procName) {
	
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

funcptr_t glXGetProcAddressARB(const GLubyte * procName) {
	
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

void * my_dlsym(const char * name, const char * hook) {
	
	if(name) {
		if(!strcmp(name, "glProgramStringARB")) {
			printf("[hook] hooked %s via %s\n", name, hook);
			return (void *)glProgramStringARB;
		} else if(!strcmp(name, "glXGetProcAddress")) {
			return (void *)glXGetProcAddress;
		} else if(!strcmp(name, "glXGetProcAddressARB")) {
			return (void *)glXGetProcAddressARB;
		}
	}
	
	return NULL;
}
