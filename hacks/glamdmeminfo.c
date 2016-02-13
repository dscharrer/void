#if 0

#/*
# * Fix for games/engines that require NV/AMD OpenGL extensions to query VRAM
# * size and behave stupidly when those are not available.
# * This implementation requires the GLX_MESA_query_renderer extension.
# *
# * This hack may not work perfectly: The Mesa extension only provides the total VRAM
# * size while the AMD and NVidia extensions (additionally) report free VRAM as well
# * as other information. We pretend free VRAM == total VRAM, which works well enough
# * as most games only query for the memory size on startup anyway.
# *
# * Known uses:
# *  - Meltdown: blurry UI elements after level change
# *
# * Fixed issues:
# *  - Pillars of Eternity: fixes blurry textures after leveling up
# *    https://forums.obsidian.net/topic/71852-linux-crashes-after-character-creation/
# *    This hack is no longer needed, PoE can use GLX_MESA_query_renderer directly!
# *
# * Usage: sh glamdmeminfo.c [path/to/game/binary]
# *
# * You can copy this file to the game's Steam install and then set the
# * launch options under Properties → Set Launch Options... to
# *   sh glamdmeminfo.c %command%
# *
# * Requires:
# *  - GCC with support for compiling 32-bit binaries
# *  - 32-bit OpenGL development files (mesa)
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
		command="$command"' "$self" -o "$out/$arch/$soname" -DLOG_PREFIX="\"[$name] \""'
		command="$command $(PKG_CONFIG_PATH="$pkg_config_path" pkg-config --cflags --libs gl)"
		eval "$command" || exit 1
	fi
	export LD_LIBRARY_PATH="$out/$arch:$LD_LIBRARY_PATH"
done

#/* Run the executable */
export LD_PRELOAD="$LD_PRELOAD:$soname"
[ -z "$1" ] || exec "$@"

exit

#endif /* 0 */



#define EXPOSE_GL_ATI_meminfo          1
#define EXPOSE_GL_NVX_gpu_memory_info  0



#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <dlfcn.h>
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

#define GLX_RENDERER_VIDEO_MEMORY_MESA                   0x8187

#define GL_VBO_FREE_MEMORY_ATI                           0x87FB
#define GL_TEXTURE_FREE_MEMORY_ATI                       0x87FC
#define GL_RENDERBUFFER_FREE_MEMORY_ATI                  0x87FD

#define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
#define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
#define GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B

static GLint extension_hijack_index = 0;

static const char * extension_hijack[] = {
	#if EXPOSE_GL_ATI_meminfo
	"GL_ATI_meminfo",
	#endif
	#if EXPOSE_GL_NVX_gpu_memory_info
	"GL_NVX_gpu_memory_info",
	#endif
};

typedef int (*glXQCRIMESA_t)(int attribute, unsigned int * value);

static GLint get_total_vram() {
	unsigned total_vram = 0;
	glXQCRIMESA_t real_glXQCRIMESA
		= (glXQCRIMESA_t)get_proc("glXQueryCurrentRendererIntegerMESA");
	if(real_glXQCRIMESA) {
		(void)real_glXQCRIMESA(GLX_RENDERER_VIDEO_MEMORY_MESA, &total_vram);
	}
	return total_vram * 1024; // MiB -> KiB
}

typedef void (*glGetIntegerv_t)(GLenum  pname,  GLint *  params);

static void redir_glGetIntegerv(GLenum pname, GLint * params) {
	
	switch(pname) {
		
		#if EXPOSE_GL_ATI_meminfo
		
		case GL_VBO_FREE_MEMORY_ATI:
		case GL_TEXTURE_FREE_MEMORY_ATI:
		case GL_RENDERBUFFER_FREE_MEMORY_ATI: {
			GLint total_vram = get_total_vram();
			// WARNING: assuming free VRAM = total VRAM
			params[0] = total_vram; // total memory free in the pool
			params[1] = total_vram; // largest available free block in the pool
			params[2] = total_vram; // total auxiliary memory free
			params[3] = total_vram; // largest auxiliary free block
			fprintf(stderr, LOG_PREFIX "fixed ATI_meminfo 0x%08x = %d\n", (int)pname, (int)params[0]);
			break;
		}
		
		#endif
		
		#if EXPOSE_GL_NVX_gpu_memory_info
		
		case GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX:
		case GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX:
		case GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX: {
			// WARNING: assuming free VRAM = total VRAM
			params[0] = get_total_vram();
			break;
		}
		
		case GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX:
		case GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX: {
			// WARNING: assuming VRAM was never full
			params[0] = 0;
			break;
		}
		
		#endif
		
		case GL_NUM_EXTENSIONS: {
			glGetIntegerv_t real_glGetIntegerv = (glGetIntegerv_t)get_proc("glGetIntegerv");
			if(real_glGetIntegerv) {
				real_glGetIntegerv(GL_NUM_EXTENSIONS, &extension_hijack_index);
			}
			unsigned n = sizeof(extension_hijack)/sizeof(*extension_hijack);
			params[0] = extension_hijack_index + n;
			fprintf(stderr, LOG_PREFIX "fixed GL_NUM_EXTENSIONS = %d\n", (int)params[0]);
			break;
		}
		
		default: {
			glGetIntegerv_t real_glGetIntegerv = (glGetIntegerv_t)get_proc("glGetIntegerv");
			if(real_glGetIntegerv) {
				real_glGetIntegerv(pname, params);
			}
		}
		
	}
	
}
void glGetIntegerv(GLenum pname, GLint * params) {
	redir_glGetIntegerv(pname, params);
}

typedef const GLubyte * (*glGetStringi_t)(GLenum name, GLuint index);

static const GLubyte * redir_glGetStringi(GLenum name, GLuint index) {
	
	unsigned n = sizeof(extension_hijack)/sizeof(*extension_hijack);
	if(name == GL_EXTENSIONS && index >= (GLuint)extension_hijack_index
	   && index < (GLuint)extension_hijack_index + n) {
		const char * str = extension_hijack[index - extension_hijack_index];
		fprintf(stderr, LOG_PREFIX "fixed GL_EXTENSIONS[%d] = %s\n", (int)index, str);
		return (const GLubyte *)str;
	}
	
	glGetStringi_t real_glGetStringi = (glGetStringi_t)get_proc("glGetStringi");
	return real_glGetStringi ? real_glGetStringi(name, index) : NULL;
}
const GLubyte * glGetStringi(GLenum name, GLuint index) {
	return redir_glGetStringi(name, index);
}

typedef const GLubyte * (*glGetString_t)(GLenum name);

static const GLubyte * redir_glGetString(GLenum name) {
	
	glGetString_t real_glGetString = (glGetString_t)get_proc("glGetString");
	const GLubyte * str = (real_glGetString ? real_glGetString(name) : NULL);
	
	if(str && name == GL_EXTENSIONS) {
		unsigned n = sizeof(extension_hijack)/sizeof(*extension_hijack);
		unsigned size = 0;
		for(unsigned i = 0; i < n; i++) {
			size += 2 + strlen(extension_hijack[i]);
		}
		if(size) {
			unsigned m = strlen((const char *)str);
			GLubyte * fixed = malloc(m + size + 1);
			memcpy(fixed, str, m);
			for(unsigned i = 0; i < n; i++) {
				fixed[m++] = ',';
				fixed[m++] = ' ';
				unsigned en = strlen(extension_hijack[i]);
				memcpy(fixed + m, extension_hijack[i], en);
				m += en;
			}
			fixed[m] = '\0';
			str = fixed; // WARNING: memory leak
		}
	}
	
	return str;
}
const GLubyte * glGetString(GLenum name) {
	return redir_glGetString(name);
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
		real_dlvsym = (dlvsym_t)real_dlsym(RTLD_NEXT, "dlvsym");
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
		if(!strcmp(name, "glGetIntegerv")) {
			fprintf(stderr, LOG_PREFIX "hooked %s via %s\n", name, hook);
			return (void *)redir_glGetIntegerv;
		} else if(!strcmp(name, "glGetStringi")) {
			fprintf(stderr, LOG_PREFIX "hooked %s via %s\n", name, hook);
			return (void *)redir_glGetStringi;
		} else if(!strcmp(name, "glGetString")) {
			fprintf(stderr, LOG_PREFIX "hooked %s via %s\n", name, hook);
			return (void *)redir_glGetString;
		}
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
