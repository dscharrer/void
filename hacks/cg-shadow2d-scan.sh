#!/bin/bash

# 
# Script to run Cg shader sources through the cgc compiler, check the output for
# texture loads with (probably unintended) shadow tests and then suggest changes
# to the source files to fix this.
#
# Usage: sh cg-shadow2d-scan.sh file_or_directory [file_or_directory2…]
# 

errors=''

declare -A change_summary

while IFS= read -r -d '' file; do
	
	# Try to determine the entry point (naively assumes that there are no other void functions)
	entrypoints=(
		$(grep -aoP '^[ \t]*(void|float4)[ \t]+[a-zA-Z0-9_]+' "$file" | sed 's:^[ \t]*\(void|float4\)[ \t]\+::')
	)
	
	if [ ${#entrypoints[@]} = 0 ] ; then
		errors+="$(printf 'Could not determine entry point for %s: %s!\n\n' "$file" "${entrypoints[*]})")"
		continue
	fi
	
	compiled=0
	
	for entrypoint in "${entrypoints[@]}" ; do
		
		arbfp_shader="$(
			cgc "$file" -q -profile arbfp1 -entry "$entrypoint" 2> /dev/null
		)"
		
		[ -z "$arbfp_shader" ] && continue
		compiled=1
		
		bad_instructions="$(
			printf '%s\n' "$arbfp_shader" | grep -aP 'SHADOW2D'
		)"
		
		[ -z "$bad_instructions" ] && continue
		
		printf 'Found shadow instructions in %s:\n' "$file"
		
		while read -r line; do
			
			texunit="$(printf '%s' "$line" | grep -aoP 'texture\[\d+\]' | grep -aoP '\d+')"
			texname="$(
				printf '%s' "$arbfp_shader" \
					| grep -aP '^#var[ \t]+[^:]*:[^:]*:[ \t]*texunit[ \t]+'"$texunit" \
					| sed 's/^#var[ \t]\+\(sampler2D[ \t]\+\)\?//;s/[ \t]*:.*//'
			)"
			
			[ -z "$texname" ] || line+="$(printf '  // texture[%s] == %s' "$texunit" "$texname")"
			
			printf '  %s\n' "$line"
			
			if [ -z "$texname" ] ; then
				printf 'Could not determine texture name!\n'
				continue
			fi
			
			if printf '%s' "$line" | grep -aP "^[ \t]*TXP[ \t]+" > /dev/null
				then func='tex2Dproj' ; swizzle="xyw"
				else func='tex2D'     ; swizzle="xy"
			fi
			
			bad_source="$(
				grep -aP '(^|[^a-zA-Z0-9_])'"$func"'[ \t]*\([ \t]*'"$texname"'[ \t]*\,' "$file"
			)"
			
			if [ -z "$bad_source" ] ; then
				printf 'Could not find corresponding source code line!\n'
				continue
			fi
			
			printf '    Suggested changes:\n'
			
			while read -r bad_source_line; do
				
				change="$(
					printf '%s\n' "$bad_source_line" | sed 's:^:-:'
					printf '%s\n' "$bad_source_line" | sed 's:^:+:' \
						| sed 's/\('"$func"'[ \t]*([^()]*[^() \t]\)\([ \t)]*)\)/\1.'"$swizzle"'\2/'
				)"
				
				printf '%s\n' "$change" | sed 's:^:    :'
				
				change_summary["$change"]+="
- $file"
				
			done < <(printf '%s\n' "$bad_source")
			
		done < <(printf '%s\n' "$bad_instructions")
		
		printf '\n'
		
	done
	
	if [ $compiled = 0 ] ; then
		errors+="$(printf 'Failed to compile entry points %s in %s!\n' "${entrypoints[*]}" "$file")"
		continue
	fi
	
done < <(find "$@" -type f \( -iname '*.ps' -o -iname '*.cgfx' \) -print0 | sort -z)

printf '\n\nSUMMARY:\n'

for change in "${!change_summary[@]}" ; do
	printf '\nChange:\n%s\nin these files:%s\n' "$change" "${change_summary["$change"]}"
done

if [ ! -z "$errors" ] ; then
	printf '\n\nERRORS WHILE SCANNING:\n%s\n' "$errors"
	exit 1
fi
