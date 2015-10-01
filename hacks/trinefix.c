#if 0

#// See https://bugs.freedesktop.org/show_bug.cgi?id=66067

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
		command+=" -DDUMP_SHADERS=${TRINEFIX_DUMP_SHADERS:-0}"
		eval "$command" || exit 1
	fi
	export LD_LIBRARY_PATH="$out/$arch:$LD_LIBRARY_PATH"
done

#/* Run the executable */
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


#ifndef DUMP_SHADERS
#define DUMP_SHADERS 0
#endif

// "Fix" ARB shaders

typedef void(*funcptr_t)();
typedef funcptr_t (*glXGetProcAddress_t)(const GLubyte * procName);
static glXGetProcAddress_t real_glXGetProcAddress;

typedef void (*glProgramStringARB_t)(GLenum target, GLenum format, GLsizei len,
                                     const void * string);

void glProgramStringARB(GLenum target, GLenum format, GLsizei len, const void * string) {
	
	char * str = strdup((const char *)string);
	
	#if DUMP_SHADERS
	if(strstr(str, " SHADOW2D;") != NULL) {
		fprintf(stderr, "------------------------------------------------------\n");
		GLint name = 0;
		glGetProgramivARB(target, GL_PROGRAM_BINDING_ARB, &name);
		fprintf(stderr, "program %d:\n", name);
		fprintf(stderr, "%s\n", str);
		fprintf(stderr, "------------------------------------------------------\n");
	}
	#endif
	
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

extern char * program_invocation_short_name; // provided by glibc

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
	if(!real_dlsym || !real_dlvsym || !real_glXGetProcAddress || !real_glXGetProcAddressARB) {
		fprintf(stderr, LOG_PREFIX "attached to %s: %p %p %p %p\n",
		               program_invocation_short_name, real_dlsym, real_dlvsym,
		               real_glXGetProcAddress, real_glXGetProcAddressARB);
	}
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
			fprintf(stderr, LOG_PREFIX "hooked %s via %s\n", name, hook);
			return (void *)glProgramStringARB;
		} else if(!strcmp(name, "glXGetProcAddress")) {
			return (void *)glXGetProcAddress;
		} else if(!strcmp(name, "glXGetProcAddressARB")) {
			return (void *)glXGetProcAddressARB;
		}
	}
	
	return NULL;
}
