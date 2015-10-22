#if 0

#/*
# * Fixes for Pillars of Eternity rendering with radeonsi/r600g.
# *
# * Usage: sh pillarsfix.c [path/to/game/binary]
# *
# * You can copy this file to the game's Steam install and then set the
# * launch options under Properties → Set Launch Options... to
# *   sh pillarsfix.c %command%
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
export LD_PRELOAD="$LD_PRELOAD:$soname" # segfaults with steam overlay if prepended
[ -z "$1" ] || exec "$@"

exit

#endif /* 0 */


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

/*****************************************************************************************
 *
 * Bug #0: Textures are blurry in some cases.
 * 
 * This bug has been fixed in Unity/PoE by adding support for GLX_MESA_query_renderer.
 * The fix is no longer needed.
 */

/*****************************************************************************************
 *
 * Bug #1: The fog of war in the area map contains garbage data with Mesa 11.0 and is
 *         accumulated over successive frames.
 *
 * See: https://bugs.freedesktop.org/91656
 *
 * Fix: PoE tries to read from and render to a texture at the same time when
 *      rendering the minimap. This happens directly after clearing the fbo
 *      to (0, 0, 0, 0) so bind a dummy texture with that value instead.
 *
 * Note: This adds some overhead to each glBegin() call, but most draw calls don't use
 *       immediate mode (none should!).
 *
 * Change the following line to "#define PILLARS_FIX_MAP 0" to disable.
 */
#define PILLARS_FIX_MAP 1
#define PILLARS_PLAIN_MAP 0 // Change to 1 to stop rendering after the area map background

#if PILLARS_FIX_MAP

typedef void (*glGFAPiv_t)(GLenum target, GLenum attachment, GLenum pname, GLint * params);

typedef int (*glBegin_t)(GLenum mode);

static GLint old_texture = 0;

#if PILLARS_PLAIN_MAP
static GLint fog_of_war = 0;
#endif

static void redir_glBegin(GLenum mode) {
	
	GLint buffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer);
	if(buffer) {
		
		static glGFAPiv_t real_glGFAPiv = 0;
		if(!real_glGFAPiv) {
			real_glGFAPiv = (glGFAPiv_t)get_proc("glGetFramebufferAttachmentParameteriv");
		}
		real_glGFAPiv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &buffer);
		
		GLint old_texunit;
		glGetIntegerv(GL_ACTIVE_TEXTURE, &old_texunit);
		if(old_texunit != GL_TEXTURE0) {
			glActiveTexture(GL_TEXTURE0);
		}
		
		GLint texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
		if(texture && buffer == texture) {
			
			static GLuint dummy_texture = 0;
			if(dummy_texture) {
				glBindTexture(GL_TEXTURE_2D, dummy_texture);
			} else {
				fprintf(stderr, LOG_PREFIX "texture0 is also color0 buffer, using dummy texture\n");
				glGenTextures(1, &dummy_texture);
				glBindTexture(GL_TEXTURE_2D, dummy_texture);
				char data[] = { 0, 0, 0, 0 };
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
			
			#if PILLARS_PLAIN_MAP
			fog_of_war = texture;
			#endif
			
			old_texture = texture;
		}
		
		if(old_texunit != GL_TEXTURE0) {
			glActiveTexture(old_texunit);
		}
		
	}
	
	static glBegin_t real_glBegin = 0;
	if(!real_glBegin) {
		real_glBegin = (glBegin_t)get_proc("glBegin");
	}
	real_glBegin(mode);
	
}
void glBegin(GLenum mode) {
	redir_glBegin(mode);
}

typedef int (*glEnd_t)();

static void redir_glEnd() {
	
	static glEnd_t real_glEnd = 0;
	if(!real_glEnd) {
		real_glEnd = (glEnd_t)get_proc("glEnd");
	}
	real_glEnd();
	
	if(old_texture) {
		GLint old_texunit;
		glGetIntegerv(GL_ACTIVE_TEXTURE, &old_texunit);
		if(old_texunit != GL_TEXTURE0) {
			glActiveTexture(GL_TEXTURE0);
		}
		glBindTexture(GL_TEXTURE_2D, old_texture);
		if(old_texunit != GL_TEXTURE0) {
			glActiveTexture(old_texunit);
		}
		old_texture = 0;
	}
	
}
void glEnd() {
	redir_glEnd();
}

#if PILLARS_PLAIN_MAP

static GLint dont_render_anything = 0;

typedef void (*glDrawElements_t)(GLenum mode, GLsizei count, GLenum type,
                                 const GLvoid * indices);

static void redir_glDrawElements(GLenum mode, GLsizei count, GLenum type,
                                 const GLvoid * indices) {
	
	if(fog_of_war) {
		
		GLint old_texunit;
		glGetIntegerv(GL_ACTIVE_TEXTURE, &old_texunit);
		if(old_texunit != GL_TEXTURE0) {
			glActiveTexture(GL_TEXTURE0);
		}
		
		GLint texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
		if(texture == fog_of_war) {
			dont_render_anything = 1;
		}
		
		if(old_texunit != GL_TEXTURE0) {
			glActiveTexture(old_texunit);
		}
		
	}
	
	if(dont_render_anything) {
		return;
	}
	
	static glDrawElements_t real_glDrawElements = 0;
	if(!real_glDrawElements) {
		real_glDrawElements = (glDrawElements_t)get_proc("glDrawElements");
	}
	real_glDrawElements(mode, count, type, indices);
	
}
void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices) {
	redir_glDrawElements(mode, count, type, indices);
}

typedef void (*glXSwapBuffers_t)(Display * dpy, GLXDrawable drawable);

static void redir_glXSwapBuffers(Display * dpy, GLXDrawable drawable) {
	
	dont_render_anything = 0;
	fog_of_war = 0;
	
	static glXSwapBuffers_t real_glXSwapBuffers = 0;
	if(!real_glXSwapBuffers) {
		real_glXSwapBuffers = (glXSwapBuffers_t)get_proc("glXSwapBuffers");
	}
	real_glXSwapBuffers(dpy, drawable);
	
}
void glXSwapBuffers(Display * dpy, GLXDrawable drawable) {
	redir_glXSwapBuffers(dpy, drawable);
}

#endif

#endif

/*****************************************************************************************
 *
 * Bug #2: The water has black spots with radeonsi.
 * 
 * Fix: PoE's water shader uses the POW op with a base value that can be negative.
 *      The returned value can be Inf/NaN, which propagates to the cumpute
 *
 * Note: This bug may not exist with older hardware.
 * 
 * Change the following line to "#define PILLARS_FIX_WATER 0" to disable.
 */
#define PILLARS_FIX_WATER 1

#if PILLARS_FIX_WATER

typedef void (*glProgramStringARB_t)(GLenum target, GLenum format, GLsizei len,
                                     const void * string);

static void redir_glProgramStringARB(GLenum target, GLenum format, GLsizei len,
                                     const void * string) {
	
	const char * original = (const char *)string;
	
	const char needle[] = "\n"
		"DP3 R1.x, R1, R6;\n" // The result can be negative.
		"DP3_SAT R1.y, -R2, R4;\n"
		"POW R2.x, R1.x, c[25].w;\n" // Pow with a negative base is undefined!
	;
	const char replacement[] = "\n"
		"DP3_SAT R1.x,R1,R6;\n" // Alternatively use ABS on the result, but this is shorter.
		"DP3_SAT R1.y,-R2,R4;\n"
		"POW R2.x, R1.x, c[25].w;\n"
	;
	
	char * modified = 0;
	if(sizeof(replacement) != sizeof(needle)) {
		fprintf(stderr, LOG_PREFIX "bad shader substitution\n");
	} else {
		int n = sizeof(needle) - 1; // Don't include terminating \0 character
		for(int i = 0; i < (int)len - n; i++) {
			if(memcmp(original + i, needle, n) == 0) {
				fprintf(stderr, LOG_PREFIX "modified water shader\n");
				string = modified = (char *)malloc(len);
				memcpy(modified, original, i);
				memcpy(modified + i, replacement, n);
				memcpy(modified + i + n, original + i + n, len - i - n);
				break;
			}
		}
	}
	
	static glProgramStringARB_t real_glProgramStringARB = 0;
	if(!real_glProgramStringARB) {
		real_glProgramStringARB = (glProgramStringARB_t)get_proc("glProgramStringARB");
	}
	real_glProgramStringARB(target, format, len, string);
	
	if(modified) {
		free(modified);
	}
	
}
void glProgramStringARB(GLenum target, GLenum format, GLsizei len, const void * string) {
	return redir_glProgramStringARB(target, format, len, string);
}

#endif

/*****************************************************************************************
 *
 * Bug #3: Anti-aliasing on characters does not work with Mesa.
 * 
 * Fix: The game never chooses a multisample framebuffer config. This can be fixed by
 *      reordering the available configs to list MSAA ones first.
 *      This does not work for the character creation or inventory screens, but it
 *      does work for in-game character models.
 *      It also only works for outlines - textures on the characters are not mipmapped.
 *
 * Todo: This might be a general unity bug, figure out why it happens.
 * 
 * Change the following line to "#define PILLARS_FIX_MSAA 0" to disable.
 * Change to number to a lower value for different MSAA settings.
 */
#define PILLARS_FIX_MSAA 8

#if PILLARS_FIX_MSAA

typedef GLXFBConfig * (*glXChooseFBConfig_t)(Display * dpy, int screen,
                                             const int * attrib_list, int * nelements);

static int * fix_attrib_list(const int * attrib_list) {
	
	int count = 0;
	while(attrib_list[count] != None) {
		count += 1 + 1;
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
			new_attrib_list[out++] = attrib_list[i++];
			new_attrib_list[out++] = attrib_list[i++];
		}
	}
	new_attrib_list[out++] = GLX_SAMPLE_BUFFERS;
	new_attrib_list[out++] = 1;
	new_attrib_list[out++] = GLX_SAMPLES;
	new_attrib_list[out++] = PILLARS_FIX_MSAA;
	new_attrib_list[out++] = None;
	
	return new_attrib_list;
}

static GLXFBConfig * redir_glXChooseFBConfig(Display * dpy, int screen,
                                             const int * attrib_list, int * nelements) {
	
	int * new_attrib_list = fix_attrib_list(attrib_list);
	if(!new_attrib_list) {
		*nelements = 0;
		return NULL;
	}
	
	fprintf(stderr, LOG_PREFIX "injected %dxMSAA config for glXChooseFBConfig\n",
	                PILLARS_FIX_MSAA);
	
	static glXChooseFBConfig_t real_glXChooseFBConfig = 0;
	if(!real_glXChooseFBConfig) {
		real_glXChooseFBConfig = get_proc("glXChooseFBConfig");
	}
	GLXFBConfig * ret = real_glXChooseFBConfig(dpy, screen, new_attrib_list, nelements);
	
	free(new_attrib_list);
	
	return ret;
}
GLXFBConfig * glXChooseFBConfig(Display * dpy, int screen, const int * attrib_list,
                                int * nelements) {
	return redir_glXChooseFBConfig(dpy, screen, attrib_list, nelements);
}

#endif

/*****************************************************************************************
 * Hooking boilerplate code follows
 */

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
		#if PILLARS_FIX_MAP
		if(!strcmp(name, "glBegin")) {
			fprintf(stderr, LOG_PREFIX "hooked %s via %s\n", name, hook);
			return (void *)redir_glBegin;
		}
		if(!strcmp(name, "glEnd")) {
			fprintf(stderr, LOG_PREFIX "hooked %s via %s\n", name, hook);
			return (void *)redir_glEnd;
		}
		#endif
		#if PILLARS_PLAIN_MAP
		if(!strcmp(name, "glDrawElements")) {
			fprintf(stderr, LOG_PREFIX "hooked %s via %s\n", name, hook);
			return (void *)redir_glDrawElements;
		}
		if(!strcmp(name, "glXSwapBuffers")) {
			fprintf(stderr, LOG_PREFIX "hooked %s via %s\n", name, hook);
			return (void *)redir_glXSwapBuffers;
		}
		#endif
		#if PILLARS_FIX_WATER
		if(!strcmp(name, "glProgramStringARB")) {
			fprintf(stderr, LOG_PREFIX "hooked %s via %s\n", name, hook);
			return (void *)redir_glProgramStringARB;
		}
		#endif
		#if PILLARS_FIX_MSAA
		if(!strcmp(name, "glXChooseFBConfig")) {
			fprintf(stderr, LOG_PREFIX "hooked %s via %s\n", name, hook);
			return (void *)redir_glXChooseFBConfig;
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
