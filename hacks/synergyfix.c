#if 0

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
		   -O3 -Wall -Wextra \
			|| exit 1
	fi
	export LD_LIBRARY_PATH="$out/$arch:$LD_LIBRARY_PATH"
done

#// Run the executable
export LD_PRELOAD="$soname:$LD_PRELOAD"
[ -z "$1" ] || exec "$@"

exit

#endif // 0

#include <stdarg.h>
#include <wchar.h>

int __swprintf_chk(wchar_t * __restrict s, size_t n, int flag, size_t slen, const wchar_t * format, ...) {
	(void)flag, (void)slen;
	va_list args;
	va_start(args, format);
	int ret = __vswprintf_alias(s, n, format, args);
	va_end(args);
	return ret;
}
