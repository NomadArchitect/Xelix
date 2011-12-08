#pragma once

/* Copyright © 2011 Lukas Martini
 *
 * This file is part of Xelix.
 *
 * Xelix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xelix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xelix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <lib/generic.h>

// A generic keymap for qwerty keyboards. To be replaced later by files.
char keymap_en[512] = {
 0 ,//0x0
 0 ,//0x1
'1',//0x2
'2',//0x3
'3',//0x4
'4',//0x5
'5',//0x6
'6',//0x7
'7',//0x8
'8',//0x9
'9',//0xa
'0',//0xb
 0 ,//0xc
 0 ,//0xd
'\b',//0xe
'\t',//0xf
'q',//0x10
'w',//0x11
'e',//0x12
'r',//0x13
't',//0x14
'z',//0x15
'u',//0x16
'i',//0x17
'o',//0x18
'p',//0x19
 0 ,//0x1a
 0 ,//0x1b
'\n',//0x1c
 0 ,//0x1d
'a',//0x1e
's',//0x1f
'd',//0x20
'f',//0x21
'g',//0x22
'h',//0x23
'j',//0x24
'k',//0x25
'l',//0x26
 0 ,//0x27
 0 ,//0x28
 0 ,//0x29
 0 ,//0x2a
'#',//0x2b
'y',//0x2c
'x',//0x2d
'c',//0x2e
'v',//0x2f
'b',//0x30
'n',//0x31
'm',//0x32
',',//0x33
'.',//0x34
'-',//0x35
 0 ,//0x36
 0 ,//0x37
 0 ,//0x38
' ',//0x39
 0 ,//0x3a
 0 ,//0x3b
 0 ,//0x3c
 0 ,//0x3d
 0 ,//0x3e
 0 ,//0x3f
 0 ,//0x40
 0 ,//0x41
 0 ,//0x42
 0 ,//0x43
 0 ,//0x44
 0 ,//0x45
 0 ,//0x46
 0 ,//0x47
 0 ,//0x48
 0 ,//0x49
 0 ,//0x4a
 0 ,//0x4b
 0 ,//0x4c
 0 ,//0x4d
 0 ,//0x4e
 0 ,//0x4f
 0 ,//0x50
 0 ,//0x51
 0 ,//0x52
 0 ,//0x53
 0 ,//0x54
 0 ,//0x55
 0 ,//0x56
 0 ,//0x57
 0 ,//0x58
 0 ,//0x59
 0 ,//0x5a
 0 ,//0x5b
 0 ,//0x5c
 0 ,//0x5d
 0 ,//0x5e
 0 ,//0x5f
 0 ,//0x60
 0 ,//0x61
 0 ,//0x62
 0 ,//0x63
 0 ,//0x64
 0 ,//0x65
 0 ,//0x66
 0 ,//0x67
 0 ,//0x68
 0 ,//0x69
 0 ,//0x6a
 0 ,//0x6b
 0 ,//0x6c
 0 ,//0x6d
 0 ,//0x6e
 0 ,//0x6f
 0 ,//0x70
 0 ,//0x71
 0 ,//0x72
 0 ,//0x73
 0 ,//0x74
 0 ,//0x75
 0 ,//0x76
 0 ,//0x77
 0 ,//0x78
 0 ,//0x79
 0 ,//0x7a
 0 ,//0x7b
 0 ,//0x7c
 0 ,//0x7d
 0 ,//0x7e
 0 ,//0x7f
 0 ,//0x80
 0 ,//0x81
 0 ,//0x82
 0 ,//0x83
 0 ,//0x84
 0 ,//0x85
 0 ,//0x86
 0 ,//0x87
 0 ,//0x88
 0 ,//0x89
 0 ,//0x8a
 0 ,//0x8b
 0 ,//0x8c
 0 ,//0x8d
 0 ,//0x8e
 0 ,//0x8f
 0 ,//0x90
 0 ,//0x91
 0 ,//0x92
 0 ,//0x93
 0 ,//0x94
 0 ,//0x95
 0 ,//0x96
 0 ,//0x97
 0 ,//0x98
 0 ,//0x99
 0 ,//0x9a
 0 ,//0x9b
 0 ,//0x9c
 0 ,//0x9d
 0 ,//0x9e
 0 ,//0x9f
 0 ,//0xa0
 0 ,//0xa1
 0 ,//0xa2
 0 ,//0xa3
 0 ,//0xa4
 0 ,//0xa5
 0 ,//0xa6
 0 ,//0xa7
 0 ,//0xa8
 0 ,//0xa9
 0 ,//0xaa
 0 ,//0xab
 0 ,//0xac
 0 ,//0xad
 0 ,//0xae
 0 ,//0xaf
 0 ,//0xb0
 0 ,//0xb1
 0 ,//0xb2
 0 ,//0xb3
 0 ,//0xb4
 0 ,//0xb5
 0 ,//0xb6
 0 ,//0xb7
 0 ,//0xb8
 0 ,//0xb9
 0 ,//0xba
 0 ,//0xbb
 0 ,//0xbc
 0 ,//0xbd
 0 ,//0xbe
 0 ,//0xbf
 0 ,//0xc0
 0 ,//0xc1
 0 ,//0xc2
 0 ,//0xc3
 0 ,//0xc4
 0 ,//0xc5
 0 ,//0xc6
 0 ,//0xc7
 0 ,//0xc8
 0 ,//0xc9
 0 ,//0xca
 0 ,//0xcb
 0 ,//0xcc
 0 ,//0xcd
 0 ,//0xce
 0 ,//0xcf
 0 ,//0xd0
 0 ,//0xd1
 0 ,//0xd2
 0 ,//0xd3
 0 ,//0xd4
 0 ,//0xd5
 0 ,//0xd6
 0 ,//0xd7
 0 ,//0xd8
 0 ,//0xd9
 0 ,//0xda
 0 ,//0xdb
 0 ,//0xdc
 0 ,//0xdd
 0 ,//0xde
 0 ,//0xdf
 0 ,//0xe0
 0 ,//0xe1
 0 ,//0xe2
 0 ,//0xe3
 0 ,//0xe4
 0 ,//0xe5
 0 ,//0xe6
 0 ,//0xe7
 0 ,//0xe8
 0 ,//0xe9
 0 ,//0xea
 0 ,//0xeb
 0 ,//0xec
 0 ,//0xed
 0 ,//0xee
 0 ,//0xef
 0 ,//0xf0
 0 ,//0xf1
 0 ,//0xf2
 0 ,//0xf3
 0 ,//0xf4
 0 ,//0xf5
 0 ,//0xf6
 0 ,//0xf7
 0 ,//0xf8
 0 ,//0xf9
 0 ,//0xfa
 0 ,//0xfb
 0 ,//0xfc
 0 ,//0xfd
 0 ,//0xfe
 0 ,//0xff

// Upper case

0 ,//0x0
 0 ,//0x1
'!',//0x2
'"',//0x3
 0 ,//0x4
'$',//0x5
'%',//0x6
'&',//0x7
'/',//0x8
'(',//0x9
')',//0xa
'=',//0xb
 0 ,//0xc
 0 ,//0xd
'\b',//0xe
 0 ,//0xf
'Q',//0x10
'W',//0x11
'E',//0x12
'R',//0x13
'T',//0x14
'Z',//0x15
'U',//0x16
'I',//0x17
'O',//0x18
'P',//0x19
 0 ,//0x1a
 0 ,//0x1b
'\n',//0x1c
 0 ,//0x1d
'A',//0x1e
'S',//0x1f
'D',//0x20
'F',//0x21
'G',//0x22
'H',//0x23
'J',//0x24
'K',//0x25
'L',//0x26
 0 ,//0x27
 0 ,//0x28
 0 ,//0x29
 0 ,//0x2a
'\'' ,//0x2b
'Y',//0x2c
'X',//0x2d
'C',//0x2e
'V',//0x2f
'B',//0x30
'N',//0x31
'M',//0x32
';',//0x33
':',//0x34
'_',//0x35
 0 ,//0x36
 0 ,//0x37
 0 ,//0x38
' ',//0x39
 0 ,//0x3a
 0 ,//0x3b
 0 ,//0x3c
 0 ,//0x3d
 0 ,//0x3e
 0 ,//0x3f
 0 ,//0x40
 0 ,//0x41
 0 ,//0x42
 0 ,//0x43
 0 ,//0x44
 0 ,//0x45
 0 ,//0x46
 0 ,//0x47
 0 ,//0x48
 0 ,//0x49
 0 ,//0x4a
 0 ,//0x4b
 0 ,//0x4c
 0 ,//0x4d
 0 ,//0x4e
 0 ,//0x4f
 0 ,//0x50
 0 ,//0x51
 0 ,//0x52
 0 ,//0x53
 0 ,//0x54
 0 ,//0x55
 0 ,//0x56
 0 ,//0x57
 0 ,//0x58
 0 ,//0x59
 0 ,//0x5a
 0 ,//0x5b
 0 ,//0x5c
 0 ,//0x5d
 0 ,//0x5e
 0 ,//0x5f
 0 ,//0x60
 0 ,//0x61
 0 ,//0x62
 0 ,//0x63
 0 ,//0x64
 0 ,//0x65
 0 ,//0x66
 0 ,//0x67
 0 ,//0x68
 0 ,//0x69
 0 ,//0x6a
 0 ,//0x6b
 0 ,//0x6c
 0 ,//0x6d
 0 ,//0x6e
 0 ,//0x6f
 0 ,//0x70
 0 ,//0x71
 0 ,//0x72
 0 ,//0x73
 0 ,//0x74
 0 ,//0x75
 0 ,//0x76
 0 ,//0x77
 0 ,//0x78
 0 ,//0x79
 0 ,//0x7a
 0 ,//0x7b
 0 ,//0x7c
 0 ,//0x7d
 0 ,//0x7e
 0 ,//0x7f
 0 ,//0x80
 0 ,//0x81
 0 ,//0x82
 0 ,//0x83
 0 ,//0x84
 0 ,//0x85
 0 ,//0x86
 0 ,//0x87
 0 ,//0x88
 0 ,//0x89
 0 ,//0x8a
 0 ,//0x8b
 0 ,//0x8c
 0 ,//0x8d
 0 ,//0x8e
 0 ,//0x8f
 0 ,//0x90
 0 ,//0x91
 0 ,//0x92
 0 ,//0x93
 0 ,//0x94
 0 ,//0x95
 0 ,//0x96
 0 ,//0x97
 0 ,//0x98
 0 ,//0x99
 0 ,//0x9a
 0 ,//0x9b
 0 ,//0x9c
 0 ,//0x9d
 0 ,//0x9e
 0 ,//0x9f
 0 ,//0xa0
 0 ,//0xa1
 0 ,//0xa2
 0 ,//0xa3
 0 ,//0xa4
 0 ,//0xa5
 0 ,//0xa6
 0 ,//0xa7
 0 ,//0xa8
 0 ,//0xa9
 0 ,//0xaa
 0 ,//0xab
 0 ,//0xac
 0 ,//0xad
 0 ,//0xae
 0 ,//0xaf
 0 ,//0xb0
 0 ,//0xb1
 0 ,//0xb2
 0 ,//0xb3
 0 ,//0xb4
 0 ,//0xb5
 0 ,//0xb6
 0 ,//0xb7
 0 ,//0xb8
 0 ,//0xb9
 0 ,//0xba
 0 ,//0xbb
 0 ,//0xbc
 0 ,//0xbd
 0 ,//0xbe
 0 ,//0xbf
 0 ,//0xc0
 0 ,//0xc1
 0 ,//0xc2
 0 ,//0xc3
 0 ,//0xc4
 0 ,//0xc5
 0 ,//0xc6
 0 ,//0xc7
 0 ,//0xc8
 0 ,//0xc9
 0 ,//0xca
 0 ,//0xcb
 0 ,//0xcc
 0 ,//0xcd
 0 ,//0xce
 0 ,//0xcf
 0 ,//0xd0
 0 ,//0xd1
 0 ,//0xd2
 0 ,//0xd3
 0 ,//0xd4
 0 ,//0xd5
 0 ,//0xd6
 0 ,//0xd7
 0 ,//0xd8
 0 ,//0xd9
 0 ,//0xda
 0 ,//0xdb
 0 ,//0xdc
 0 ,//0xdd
 0 ,//0xde
 0 ,//0xdf
 0 ,//0xe0
 0 ,//0xe1
 0 ,//0xe2
 0 ,//0xe3
 0 ,//0xe4
 0 ,//0xe5
 0 ,//0xe6
 0 ,//0xe7
 0 ,//0xe8
 0 ,//0xe9
 0 ,//0xea
 0 ,//0xeb
 0 ,//0xec
 0 ,//0xed
 0 ,//0xee
 0 ,//0xef
 0 ,//0xf0
 0 ,//0xf1
 0 ,//0xf2
 0 ,//0xf3
 0 ,//0xf4
 0 ,//0xf5
 0 ,//0xf6
 0 ,//0xf7
 0 ,//0xf8
 0 ,//0xf9
 0 ,//0xfa
 0 ,//0xfb
 0 ,//0xfc
 0 ,//0xfd
 0 ,//0xfe
 0 //0xff
};

 
