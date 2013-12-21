
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

## Fraktur and Script

Additionally, the key map changes `RightWin` to switch letters to Fraktur symbols and digits to double-stroked digits. You can use `Shift` with `RightWin` to lock to Fraktur symbols.

Further, once in Fraktur mode `AltGr` changes letters once again, this time to to script. Again, you can use `Shift` with `AltGr` to lock to script symbols.

| Key      | RightWin | RightWin + AltGr |
|--------- | -------- | ---------------- |
| `a` `A`  | 𝖆 𝕬      | 𝓪 𝓐
| `b` `B`  | 𝖇 𝕭      | 𝓫 𝓑
| `c` `C`  | 𝖈 𝕮      | 𝓬 𝓒
| `d` `D`  | 𝖉 𝕯      | 𝓭 𝓓
| `e` `E`  | 𝖊 𝕰      | 𝓮 𝓔
| `f` `F`  | 𝖋 𝕱      | 𝓯 𝓕
| `g` `G`  | 𝖌 𝕲      | 𝓰 𝓖
| `h` `H`  | 𝖍 𝕳      | 𝓱 𝓗
| `i` `I`  | 𝖎 𝕴      | 𝓲 𝓘
| `j` `J`  | 𝖏 𝕵      | 𝓳 𝓙
| `k` `K`  | 𝖐 𝕶      | 𝓴 𝓚
| `l` `L`  | 𝖑 𝕷      | 𝓵 𝓛
| `m` `M`  | 𝖒 𝕸      | 𝓶 𝓜
| `n` `N`  | 𝖓 𝕹      | 𝓷 𝓝
| `o` `O`  | 𝖔 𝕺      | 𝓸 𝓞
| `p` `P`  | 𝖕 𝕻      | 𝓹 𝓟
| `q` `Q`  | 𝖖 𝕼      | 𝓺 𝓠
| `r` `R`  | 𝖗 𝕽      | 𝓻 𝓡
| `s` `S`  | 𝖘 𝕾      | 𝓼 𝓢
| `t` `T`  | 𝖙 𝕿      | 𝓽 𝓣
| `u` `U`  | 𝖚 𝖀      | 𝓾 𝓤
| `v` `V`  | 𝖛 𝖁      | 𝓿 𝓥
| `w` `W`  | 𝖜 𝖂      | 𝔀 𝓦
| `x` `X`  | 𝖝 𝖃      | 𝔁 𝓧
| `y` `Y`  | 𝖞 𝖄      | 𝔂 𝓨
| `z` `Z`  | 𝖟 𝖅      | 𝔃 𝓩
| `0` `)`  | 𝟘 )      | 0 )
| `1` `!`  | 𝟙 !      | 1 !
| `2` `@`  | 𝟚 @      | 2 @
| `3` `#`  | 𝟛 #      | 3 #
| `4` `$`  | 𝟜 $      | 4 $
| `5` `%`  | 𝟝 %      | 5 %
| `6` `^`  | 𝟞 ^      | 6 ^
| `7` `&`  | 𝟟 &      | 7 &
| `8` `*`  | 𝟠 *      | 8 *
| `9` `(`  | 𝟡 (      | 9 (


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
| ∩      | `U+2229`               | `inter`                                 |
| ⋂      | `U+22C2`               | `Inter` `INTER`                         |
| ∪      | `U+222A`               | `union`                                 |
| ⋃      | `U+22C3`               | `Union` `UNION`                         |
| ∈      | `U+2208`               | `ele`                                   |
| ∉      | `U+2209`               | `nele`                                  |
| ∃      | `U+2203`               | `exist`                                 |
| ∄      | `U+2204`               | `nexist`                                |
| ∀      | `U+2200`               | `forall`                                |
| ∨      | `U+2228`               | `lor`                                   |
| ⋁      | `U+22C1`               | `Lor` `LOR`                             |
| ∧      | `U+2227`               | `and`                                   |
| ⋀      | `U+22C0`               | `And` `AND`                             |
| ≠      | `U+2260`               | `neq`                                   |
| ℝ      | `U+211D`               | `real`                                  |
| ℚ      | `U+211A`               | `rational`                              |
| ℤ      | `U+2124`               | `integer`                               |
| ℕ      | `U+2115`               | `naural`                                |
| ℂ      | `U+2102`               | `complex`                               |
| ಠ_ಠ    | `U+0CA0 U+005F U+0CA0` | `lod`                                   |
| ಠ◡ಠ    | `U+0CA0 U+25E1 U+0CA0` | `slod`                                  |
| (╯°□°）╯︵ ┻━┻  | ...           | `table`                                 |
| ┬─┬ノ( º _ ºノ) | ...           | `unflip`                                |
| ༼ つ ◕_◕ ༽つ    | ...           | `give`                                  |
| ☢      | `U+2622`               | `rad`                                   |
| ⚛      | `U+269B`               | `atom`                                  |
| ☣      | `U+2623`               | `bio`                                   |
| ☠      | `U+2620`               | `skull`                                 |
| ✓      | `U+2713`               | `yes`                                   |
| ✗      | `U+2717`               | `no`                                    |
| ℃      | `U+2103`               | `dc`                                    |
| ⚛⃝      | `U+269B U+20DD`        | `aperture`                              |
| ⚠      | `U+26A0`               | `warn`                                  |
| ℹ      | `U+2139`               | `iinfo`                                 |
| ↑      | `U+2191`               | `up` `^^`                               |
| ฿      | `U+0E3F`               | `btc`                                   |
| 🐧      | `U+1F427`              | `tux` `penguin`                         |

To enable them, link/copy the `XCompose` file to `~/.XCompose`.

gtk apps like to do their own thing as usual so youll need to force them by setting `GTK_IM_MODULE=xim` (for example in `~/.bashrc`)
