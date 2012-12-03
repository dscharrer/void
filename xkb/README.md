
This is a custom X11/xkb key map for US/International keyboards that adds umlauts/diaereses to letters as well as other common symbols and dead keys using the Right Alt key (Alt Gr) as a modifier. Base keys (without Right Alt) are kept unmodified. Also, the menu key is reinterpreted as the multi/compose key.

## Modified Keys

The following table lists the modified keys:


| Key      |  AltGr     |  AltGr + Shift |
|--------- | ---------- | -------------- |
| `,` `<`  | dead ¸     | «              |
| `.` `>`  | dead ˙     | »              |
| `/` `?`  | ¿          | dead ?         |
| `a` `A`  | ä          | Ä              |
| `s` `S`  | ß          | ß              |
| `g` `G`  | dead Greek | dead Greek     |
| `l` `L`  | £          | ¢              |
| `;` `:`  | dead ¨     | dead ¨         |
| `'` `"`  | dead ´     | ´              |
| `e` `E`  | €          | ¢              |
| `y` `Y`  | ÿ          | Ÿ              |
| `u` `U`  | ü          | Ü              |
| `i` `I`  | ï          | Ï              |
| `o` `O`  | ö          | Ö              |
| `p` `P`  | §          | ¶              |
| `1` `!`  | ¡          | ¡              |
| `6` `^`  | dead ^     | dead ˇ         |
| `9` `(`  | dead ˘     | dead ˘         |
| `0` `)`  | dead ˚     | dead ˚         |
| `-` `_`  | dead .     | dead .         |
| <code>\`</code> `~` | dead `     | dead ~         |

Dead keys don't print anything, but instead modify the next input by composing the symbol and key if possible.

## Compose Sequences

Additionally, the menu key is now the multi/compose key. After pressing the menu key, the following key combinations will produce special characters. (for the en_US.UTF-8 locale)
These sequences are not defined by the keyboard layout, but by the locale (not modified here). This table lists only those sequences that can be typed on a US key map and omits those symbols that are available already.

| Symbol | Unicode  | Sequences                               |
| ------ | -------- | --------------------------------------- |
| ´      | `U+00B4` | `''`                                    |
| ¯      | `U+00AF` | `-^` `^-` `__` `_^`                     |
| ˘      | `U+02D8` | ` (` `( `                               |
| ¨      | `U+00A8` | `""`                                    |
| ˇ      | `U+02C7` | ` <` `< `                               |
| ¸      | `U+00B8` | ` ,` `, `                               |
| ©      | `U+00A9` | `oc` `oC` `Oc` `OC` `Co` `CO`           |
| ®      | `U+00AE` | `or` `oR` `Or` `OR` `RO`                |
| ›      | `U+203A` | `.>`                                    |
| ‹      | `U+2039` | `.<`                                    |
| …      | `U+2026` | `..`                                    |
| ·      | `U+00B7` | `.-` `.^` `^.`                          |
| •      | `U+2022` | `.=`                                    |
| ¦      | `U+00A6` | `!^`                                    |
| ¡      | `U+00A1` | `!!`                                    |
| ¶      | `U+00B6` | `p!` P!` `PP`                           |
| ±      | `U+00B1` | `+-` `-+`                               |
| ¿      | `U+00BF` | `??`                                    |
| ß      | `U+00DF` | `ss`                                    |
| œ      | `U+0153` | `oe`                                    |
| Œ      | `U+0152` | `OE`                                    |
| æ      | `U+00E6` | `ae`                                    |
| Æ      | `U+00C6` | `AE`                                    |
| °      | `U+00B0` | `oo`                                    |
| «      | `U+00AB` | `<<`                                    |
| »      | `U+00BB` | `>>`                                    |
| ‘      | `U+2018` | `<'` `'<`                               |
| ’      | `U+2019` | `>'` `'>`                               |
| ‚      | `U+201A` | `,'` `',`                               |
| “      | `U+201C` | `<"` `"<`                               |
| ”      | `U+201D` | `>"` `">`                               |
| „      | `U+201E` | `,"` `",`                               |
| ‰      | `U+2030` | `%o`                                    |
| ₠      | `U+20A0` | `CE`                                    |
| ₤      | `U+20A4` | `L=` `=L`                               |
| ₧      | `U+20A7` | `Pt`                                    |
| ₨      | `U+20A8` | `Rs`                                    |
| €      | `U+20AC` | `C=` `=C` `c=` `=c` `E=` `=E` `e=` `=e` |
| ¢      | `U+00A2` | <code>&#124;C</code> <code>&#124;c</code> <code>C&#124;</code> <code>c&#124;</code> `c/` `/c` |
| £      | `U+00A3` | `L-` `l-` `-L` `-l`                     |
| ¥      | `U+00A5` | `Y=` `y=` `=Y` `=y` `Y-` `-Y` `y-` `-y` |
| –      | `U+2013` | `--.`                                   |
| —      | `U+2014` | `---`                                   |
| ♩      | `U+2669` | `#q`                                    |
| ♪      | `U+266A` | `#e`                                    |
| ♫      | `U+266B` | `#E`                                    |
| ♬      | `U+266C` | `#S`                                    |
| ♭      | `U+266D` | `#b`                                    |
| ♮      | `U+266E` | `#f`                                    |
| ♯      | `U+266F` | `##`                                    |
| §      | `U+00A7` | `so` `os` `SO` `OS` `s!` `S!`           |
| ¤      | `U+00A4` | `ox` `xo` `oX` `Xo` `OX` `XO` `Ox` `xO` |
| №      | `U+2116` | `No` `NO`                               |
| ‽      | `U+203D` | `!?`                                    |
| ☭      | `U+262D` | `CCCP`                                  |
| ♥      | `U+2665` | `<3`                                    |
| ☺      | `U+263A` | `:)`                                    |
| ☹      | `U+2639` | `:(`                                    |
| ¬      | `U+00AC` | `,-` `-,`                               |
| ª      | `U+00AA` | `^_a`                                   |
| µ      | `U+00B5` | `mu` `/u` `u/`                          |
| º      | `U+00BA` | `^_o`                                   |
| ¼      | `U+00BC` | `14`                                    |
| ½      | `U+00BD` | `12`                                    |
| ¾      | `U+00BE` | `34`                                    |
| (more) |          | `xy` where `x` and `y` are digits       |
| ⅒      | `U2152` | `110`                                   |
| ×      | `U+00D7` | `xx`                                    |
| ÷      | `U+00F7` | `:-` `-:`                               |
| ⁰      | `U+2070` | `^0`                                    |
| ¹      | `U+00B9` | `^1` `1^`                               |
| ²      | `U+00B2` | `^2` `2^`                               |
| ³      | `U+00B3` | `^3` `3^`                               |
| ⁴      | `U+2074` | `^4`                                    |
| ⁵      | `U+2075` | `^5`                                    |
| ⁶      | `U+2076` | `^6`                                    |
| ⁷      | `U+2077` | `^7`                                    |
| ⁸      | `U+2078` | `^8`                                    |
| ⁹      | `U+2079` | `^9`                                    |
| ⁱ      | `U+2071` | `^_i`                                   |
| ⁿ      | `U+207F` | `^_n`                                   |
| ⁺      | `U+207A` | `^+`                                    |
| ⁼      | `U+207C` | `^=`                                    |
| ⁽      | `U+207D` | `^(`                                    |
| ⁾      | `U+207E` | `^)`                                    |
| ₀      | `U+2080` | `_0`                                    |
| ₁      | `U+2081` | `_1`                                    |
| ₂      | `U+2082` | `_2`                                    |
| ₃      | `U+2083` | `_3`                                    |
| ₄      | `U+2084` | `_4`                                    |
| ₅      | `U+2085` | `_5`                                    |
| ₆      | `U+2086` | `_6`                                    |
| ₇      | `U+2087` | `_7`                                    |
| ₈      | `U+2088` | `_8`                                    |
| ₉      | `U+2089` | `_9`                                    |
| ₊      | `U+208A` | `_+`                                    |
| ₌      | `U+208C` | `_=`                                    |
| ₍      | `U+208D` | `_(`                                    |
| ₎      | `U+208E` | `_)`                                    |
| ℠      | `U+2120` | `SM` `sM` `Sm` `sm`                     |
| ™      | `U+2122` | `TM` `tM` `Tm` `tm`                     |
| ←      | `U+2190` | `<-`                                    |
| →      | `U+2192` | `->`                                    |
| ∅      | `U+2205` | `{}`                                    |
| ≠      | `U+2260` | `/=` `=/`                               |
| ≤      | `U+2264` | `<=` `<_` `_<`                          |
| ≥      | `U+2265` | `>=` `>_` `_>`                          |
| ⌀      | `U+2300` | `di`                                    |
| ⓪      | `U+24EA` | `(0)`                                   |
| ①      | `U+2460` | `(1)`                                   |
| ②      | `U+2461` | `(2)`                                   |
| ③      | `U+2462` | `(3)`                                   |
| ④      | `U+2463` | `(4)`                                   |
| ⑤      | `U+2464` | `(5)`                                   |
| ⑥      | `U+2465` | `(6)`                                   |
| ⑦      | `U+2466` | `(7)`                                   |
| ⑧      | `U+2467` | `(8)`                                   |
| ⑨      | `U+2468` | `(9)`                                   |
| ⑩      | `U+2469` | `(10)`                                  |
| Ⓐ      | `U+24B6` | `(A)` `OA`                              |
| ⓐ      | `U+24D0` | `(a)`                                   |
| (more) |          | 0-20+, A-Z, a-z                         |
| √      | `U+221A` | `v/` `/v`                               |
| ∞      | `U+221E` | `88`                                    |
| ≡      | `U+2261` | `=_`                                    |
| ⋄      | `U+22C4` | `<>` `><`                               |
| ⌷      | `U+2337` | `[]`                                    |
| ⌿      | `U+233F` | `/-` `-/`                               |
| ⍀      | `U+2340` | `\-` `-\`                               |
| ⍬      | `U+236C` | `0~` `~0`                               |
| ⍭      | `U+236D` | <code>&#124;~</code> <code>~&#124;</code> |


## Installing

Install by copying `us_de` to `/usr/share/X11/xkb/symbols/` (on Gentoo) - if the directory doesn't exist, try `/usr/X11R6/lib/X11/xkb/symbols/`.

You can then switch to the key map using

    $ setxkbmap us_de

This will only set the keymap for the current session. To select it permanently, add the above command to `~/.xinitrc`.

This might not work if your desktop environment also sets the keymap. For KDE 4 edit `~/.kde4/share/config/kxkbrc` and change the line containing `LayoutList` to

    LayoutList=us_de

# Custom compose sequenes

The `XCompose` file provides a few custom compose sequences:

| Symbol | Unicode                | Sequences                               |
| ------ | ---------------------- | --------------------------------------- |
| ∞      | `U+221E`               | `inf`                                   |
| π      | `U+03C0`               | `pi`                                    |
| Ω      | `U+2126`               | `ohm`                                   |
| √      | `U+221A`               | `sqrt`                                  |
| ಠ_ಠ    | `U+0CA0 U+005F U+0CA0` | `lod`                                   |
| ಠ◡ಠ    | `U+0CA0 U+25E1 U+0CA0` | `slod`                                  |
| ☢      | `U+2622`               | `rad`                                   |
| ⚛      | `U+269B`               | `atom`                                  |
| ☣      | `U+2623`               | `bio`                                   |
| ☠      | `U+2620`               | `skull`                                 |
| ✓      | `U+2713`               | `yes`                                   |
| ✗      | `U+2717`               | `no`                                    |
| ℃      | `U+2103`               | `dc`                                    |
| ⚛⃝      | `U+269B U+20DD`        | `aperture`                              |

To enable them, link/copy the `XCompose` file to `~/.XCompose`.

gtk apps like to do their own thing as usual so youll need to force them by setting `GTK_IM_MODULE=xim` (for example in `~/.profile`)
