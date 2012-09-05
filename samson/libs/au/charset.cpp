

#include "charset.h"  // Own interface


namespace au {
#define T true
#define F false

// Statatic table to iso_8859

const bool _iso_8859_is_letter[] =                                      \
{ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,      \
  F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,    \
  F, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F, F, F, F, F,    \
  F, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F, F, F, F, F,    \
  F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,    \
  F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,    \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F, T, T, T, T, T, T, T, T,    \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F, T, T, T, T, T, T, T, T };


const bool _iso_8859_is_printable[] =                                   \
{ F, F, F, F, F, F, F, F, F, T, T, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,      \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,    \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,    \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F,    \
  F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,    \
  F, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,    \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,    \
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T };


// To upper
const unsigned char _iso_8859_to_upper[] = {
  << << << < HEAD
  0,                                     1,                                      2,
  3,                                     4,                                      5,
  6,       7,      8,
  9,                                     10,                                     11,
  12,      13,     14,    15,      16,     17,
  18,
  19,
  20,
  21,
  22,                                    23,                                     24,
  25,
  26,
  27,
  28,                                    29,                                     30,
  31,     \
  32,                                    33,                                     34,
  35,      36,     37,    38,      39,     40,
  41,                                    42,                                     43,
  44,      45,     46,    47,      48,     49,
  50,
  51,
  52,                                    53,
  54,                                    55,                                     56,
  57,
  58,                                    59,
  60,                                    61,                                     62,
  63,     \
  64,                                    'A',                                    'B',
  'C',     'D',    'E',   'F',     'G',    'H',
  'I',                                   'J',                                    'K',
  'L',     'M',    'N',   'O',     'P',    'Q',
  'R',
  'S',
  'T',
  'U',                                   'V',                                    'W',
  'X',     'Y',
  'Z',                                   91,
  92,                                    93,                                     94,
  95,     \
  96,                                    'A',                                    'B',
  'C',     'D',    'E',   'F',     'G',    'H',
  'I',                                   'J',                                    'K',
  'L',     'M',    'N',   'O',     'P',    'Q',
  'R',
  'S',
  'T',
  'U',                                   'V',                                    'W',
  'X',     'Y',
  'Z',
  123,                                   124,                                    125,
  126,     127,     \
  128,                                   129,                                    130,
  131,     132,    133,   134,     135,    136,
  137,                                   138,                                    139,
  140,     141,    142,   143,     144,    145,
  146,
  147,
  148,
  149,                                   150,                                    151,
  152,     153,
  154,
  155,                                   156,                                    157,
  158,     159,     \
  160,                                   161,                                    162,
  163,     164,    165,   166,     167,    168,
  169,                                   170,                                    171,
  172,     173,    174,   175,     176,    177,
  178,
  179,
  180,
  181,                                   182,                                    183,
  184,     185,
  186,
  187,                                   188,                                    189,
  190,     191,     \
  '�',                                   '�',                                    '�',
  '�',     '�',    '�',   '�',     '�',    '�',
  '�',                                   '�',                                    '�',
  '�',     '�',    '�',   '�',     '�',    '�',
  '�',
  '�',
  '�',
  '�',                                   '�',                                    215,
  '�',     '�',
  '�',
  '�',                                   '�',                                    '�',
  '�',     '�',     \
  '�',                                   '�',                                    '�',
  '�',     '�',    '�',   '�',     '�',    '�',
  '�',                                   '�',                                    '�',
  '�',     '�',    '�',   '�',     '�',    '�',
  '�',
  '�',
  '�',
  '�',                                   '�',                                    215,
  '�',     '�',
  '�',
  '�',                                   '�',                                    '�',
  '�',     '�',     \
  == == == =
    static_cast<const unsigned char>(0), static_cast<const unsigned char>(1), \
  static_cast<const unsigned char>(2),   static_cast<const unsigned char>(3), \
  static_cast<const unsigned char>(4),   static_cast<const unsigned char>(5), \
  static_cast<const unsigned char>(6),   static_cast<const unsigned char>(7), \
  static_cast<const unsigned char>(8),   static_cast<const unsigned char>(9), \
  static_cast<const unsigned char>(10),  static_cast<const unsigned char>(11), \
  static_cast<const unsigned char>(12),  static_cast<const unsigned char>(13), \
  static_cast<const unsigned char>(14),  static_cast<const unsigned char>(15), \
  static_cast<const unsigned char>(16),  static_cast<const unsigned char>(17), \
  static_cast<const unsigned char>(18),  static_cast<const unsigned char>(19), \
  static_cast<const unsigned char>(20),  static_cast<const unsigned char>(21), \
  static_cast<const unsigned char>(22),  static_cast<const unsigned char>(23), \
  static_cast<const unsigned char>(24),  static_cast<const unsigned char>(25), \
  static_cast<const unsigned char>(26),  static_cast<const unsigned char>(27), \
  static_cast<const unsigned char>(28),  static_cast<const unsigned char>(29), \
  static_cast<const unsigned char>(30),  static_cast<const unsigned char>(31), \
  static_cast<const unsigned char>(32),  static_cast<const unsigned char>(33), \
  static_cast<const unsigned char>(34),  static_cast<const unsigned char>(35), \
  static_cast<const unsigned char>(36),  static_cast<const unsigned char>(37), \
  static_cast<const unsigned char>(38),  static_cast<const unsigned char>(39), \
  static_cast<const unsigned char>(40),  static_cast<const unsigned char>(41), \
  static_cast<const unsigned char>(42),  static_cast<const unsigned char>(43), \
  static_cast<const unsigned char>(44),  static_cast<const unsigned char>(45), \
  static_cast<const unsigned char>(46),  static_cast<const unsigned char>(47), \
  static_cast<const unsigned char>(48),  static_cast<const unsigned char>(49), \
  static_cast<const unsigned char>(50),  static_cast<const unsigned char>(51), \
  static_cast<const unsigned char>(52),  static_cast<const unsigned char>(53), \
  static_cast<const unsigned char>(54),  static_cast<const unsigned char>(55), \
  static_cast<const unsigned char>(56),  static_cast<const unsigned char>(57), \
  static_cast<const unsigned char>(58),  static_cast<const unsigned char>(59), \
  static_cast<const unsigned char>(60),  static_cast<const unsigned char>(61), \
  static_cast<const unsigned char>(62),  static_cast<const unsigned char>(63), \
  static_cast<const unsigned char>(64),  static_cast<const unsigned char>('A'), \
  static_cast<const unsigned char>('B'), static_cast<const unsigned char>('C'), \
  static_cast<const unsigned char>('D'), static_cast<const unsigned char>('E'), \
  static_cast<const unsigned char>('F'), static_cast<const unsigned char>('G'), \
  static_cast<const unsigned char>('H'), static_cast<const unsigned char>('I'), \
  static_cast<const unsigned char>('J'), static_cast<const unsigned char>('K'), \
  static_cast<const unsigned char>('L'), static_cast<const unsigned char>('M'), \
  static_cast<const unsigned char>('N'), static_cast<const unsigned char>('O'), \
  static_cast<const unsigned char>('P'), static_cast<const unsigned char>('Q'), \
  static_cast<const unsigned char>('R'), static_cast<const unsigned char>('S'), \
  static_cast<const unsigned char>('T'), static_cast<const unsigned char>('U'), \
  static_cast<const unsigned char>('V'), static_cast<const unsigned char>('W'), \
  static_cast<const unsigned char>('X'), static_cast<const unsigned char>('Y'), \
  static_cast<const unsigned char>('Z'), static_cast<const unsigned char>(91), \
  static_cast<const unsigned char>(92),  static_cast<const unsigned char>(93), \
  static_cast<const unsigned char>(94),  static_cast<const unsigned char>(95), \
  static_cast<const unsigned char>(96),  static_cast<const unsigned char>('A'), \
  static_cast<const unsigned char>('B'), static_cast<const unsigned char>('C'), \
  static_cast<const unsigned char>('D'), static_cast<const unsigned char>('E'), \
  static_cast<const unsigned char>('F'), static_cast<const unsigned char>('G'), \
  static_cast<const unsigned char>('H'), static_cast<const unsigned char>('I'), \
  static_cast<const unsigned char>('J'), static_cast<const unsigned char>('K'), \
  static_cast<const unsigned char>('L'), static_cast<const unsigned char>('M'), \
  static_cast<const unsigned char>('N'), static_cast<const unsigned char>('O'), \
  static_cast<const unsigned char>('P'), static_cast<const unsigned char>('Q'), \
  static_cast<const unsigned char>('R'), static_cast<const unsigned char>('S'), \
  static_cast<const unsigned char>('T'), static_cast<const unsigned char>('U'), \
  static_cast<const unsigned char>('V'), static_cast<const unsigned char>('W'), \
  static_cast<const unsigned char>('X'), static_cast<const unsigned char>('Y'), \
  static_cast<const unsigned char>('Z'), static_cast<const unsigned char>(123), \
  static_cast<const unsigned char>(124), static_cast<const unsigned char>(125), \
  static_cast<const unsigned char>(126), static_cast<const unsigned char>(127), \
  static_cast<const unsigned char>(128), static_cast<const unsigned char>(129), \
  static_cast<const unsigned char>(130), static_cast<const unsigned char>(131), \
  static_cast<const unsigned char>(132), static_cast<const unsigned char>(133), \
  static_cast<const unsigned char>(134), static_cast<const unsigned char>(135), \
  static_cast<const unsigned char>(136), static_cast<const unsigned char>(137), \
  static_cast<const unsigned char>(138), static_cast<const unsigned char>(139), \
  static_cast<const unsigned char>(140), static_cast<const unsigned char>(141), \
  static_cast<const unsigned char>(142), static_cast<const unsigned char>(143), \
  static_cast<const unsigned char>(144), static_cast<const unsigned char>(145), \
  static_cast<const unsigned char>(146), static_cast<const unsigned char>(147), \
  static_cast<const unsigned char>(148), static_cast<const unsigned char>(149), \
  static_cast<const unsigned char>(150), static_cast<const unsigned char>(151), \
  static_cast<const unsigned char>(152), static_cast<const unsigned char>(153), \
  static_cast<const unsigned char>(154), static_cast<const unsigned char>(155), \
  static_cast<const unsigned char>(156), static_cast<const unsigned char>(157), \
  static_cast<const unsigned char>(158), static_cast<const unsigned char>(159), \
  static_cast<const unsigned char>(160), static_cast<const unsigned char>(161), \
  static_cast<const unsigned char>(162), static_cast<const unsigned char>(163), \
  static_cast<const unsigned char>(164), static_cast<const unsigned char>(165), \
  static_cast<const unsigned char>(166), static_cast<const unsigned char>(167), \
  static_cast<const unsigned char>(168), static_cast<const unsigned char>(169), \
  static_cast<const unsigned char>(170), static_cast<const unsigned char>(171), \
  static_cast<const unsigned char>(172), static_cast<const unsigned char>(173), \
  static_cast<const unsigned char>(174), static_cast<const unsigned char>(175), \
  static_cast<const unsigned char>(176), static_cast<const unsigned char>(177), \
  static_cast<const unsigned char>(178), static_cast<const unsigned char>(179), \
  static_cast<const unsigned char>(180), static_cast<const unsigned char>(181), \
  static_cast<const unsigned char>(182), static_cast<const unsigned char>(183), \
  static_cast<const unsigned char>(184), static_cast<const unsigned char>(185), \
  static_cast<const unsigned char>(186), static_cast<const unsigned char>(187), \
  static_cast<const unsigned char>(188), static_cast<const unsigned char>(189), \
  static_cast<const unsigned char>(190), static_cast<const unsigned char>(191), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>(215), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>(215), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�')  \
  >> >> >> > samson_0 .7
};


// To lower
const unsigned char _iso_8859_to_lower[] = {
  << << << < HEAD
  0,                                     1,                                      2,
  3,                                     4,                                      5,
  6,       7,      8,
  9,                                     10,                                     11,
  12,      13,     14,    15,      16,     17,
  18,
  19,
  20,
  21,
  22,                                    23,                                     24,
  25,
  26,
  27,
  28,                                    29,                                     30,
  31,     \
  32,                                    33,                                     34,
  35,      36,     37,    38,      39,     40,
  41,                                    42,                                     43,
  44,      45,     46,    47,      48,     49,
  50,
  51,
  52,                                    53,
  54,                                    55,                                     56,
  57,
  58,                                    59,
  60,                                    61,                                     62,
  63,     \
  64,                                    'a',                                    'b',
  'c',     'd',    'e',   'f',     'g',    'h',
  'i',                                   'j',                                    'k',
  'l',     'm',    'n',   'o',     'p',    'q',
  'r',
  's',
  't',
  'u',                                   'v',                                    'w',
  'x',     'y',
  'z',                                   91,
  92,                                    93,                                     94,
  95,     \
  96,                                    'a',                                    'b',
  'c',     'd',    'e',   'f',     'g',    'h',
  'i',                                   'j',                                    'k',
  'l',     'm',    'n',   'o',     'p',    'q',
  'r',
  's',
  't',
  'u',                                   'v',                                    'w',
  'x',     'y',
  'z',
  123,                                   124,                                    125,
  126,     127,     \
  128,                                   129,                                    130,
  131,     132,    133,   134,     135,    136,
  137,                                   138,                                    139,
  140,     141,    142,   143,     144,    145,
  146,
  147,
  148,
  149,                                   150,                                    151,
  152,     153,
  154,
  155,                                   156,                                    157,
  158,     159,     \
  160,                                   161,                                    162,
  163,     164,    165,   166,     167,    168,
  169,                                   170,                                    171,
  172,     173,    174,   175,     176,    177,
  178,
  179,
  180,
  181,                                   182,                                    183,
  184,     185,
  186,
  187,                                   188,                                    189,
  190,     191,     \
  '�',                                   '�',                                    '�',
  '�',     '�',    '�',   '�',     '�',    '�',
  '�',                                   '�',                                    '�',
  '�',     '�',    '�',   '�',     '�',    '�',
  '�',
  '�',
  '�',
  '�',                                   '�',                                    247,
  '�',     '�',
  '�',
  '�',                                   '�',                                    '�',
  '�',     '�',     \
  '�',                                   '�',                                    '�',
  '�',     '�',    '�',   '�',     '�',    '�',
  '�',                                   '�',                                    '�',
  '�',     '�',    '�',   '�',     '�',    '�',
  '�',
  '�',
  '�',
  '�',                                   '�',                                    247,
  '�',     '�',
  '�',
  '�',                                   '�',                                    '�',
  '�',     '�',     \
  == == == =
    static_cast<const unsigned char>(0), static_cast<const unsigned char>(1), \
  static_cast<const unsigned char>(2),   static_cast<const unsigned char>(3), \
  static_cast<const unsigned char>(4),   static_cast<const unsigned char>(5), \
  static_cast<const unsigned char>(6),   static_cast<const unsigned char>(7), \
  static_cast<const unsigned char>(8),   static_cast<const unsigned char>(9), \
  static_cast<const unsigned char>(10),  static_cast<const unsigned char>(11), \
  static_cast<const unsigned char>(12),  static_cast<const unsigned char>(13), \
  static_cast<const unsigned char>(14),  static_cast<const unsigned char>(15), \
  static_cast<const unsigned char>(16),  static_cast<const unsigned char>(17), \
  static_cast<const unsigned char>(18),  static_cast<const unsigned char>(19), \
  static_cast<const unsigned char>(20),  static_cast<const unsigned char>(21), \
  static_cast<const unsigned char>(22),  static_cast<const unsigned char>(23), \
  static_cast<const unsigned char>(24),  static_cast<const unsigned char>(25), \
  static_cast<const unsigned char>(26),  static_cast<const unsigned char>(27), \
  static_cast<const unsigned char>(28),  static_cast<const unsigned char>(29), \
  static_cast<const unsigned char>(30),  static_cast<const unsigned char>(31), \
  static_cast<const unsigned char>(32),  static_cast<const unsigned char>(33), \
  static_cast<const unsigned char>(34),  static_cast<const unsigned char>(35), \
  static_cast<const unsigned char>(36),  static_cast<const unsigned char>(37), \
  static_cast<const unsigned char>(38),  static_cast<const unsigned char>(39), \
  static_cast<const unsigned char>(40),  static_cast<const unsigned char>(41), \
  static_cast<const unsigned char>(42),  static_cast<const unsigned char>(43), \
  static_cast<const unsigned char>(44),  static_cast<const unsigned char>(45), \
  static_cast<const unsigned char>(46),  static_cast<const unsigned char>(47), \
  static_cast<const unsigned char>(48),  static_cast<const unsigned char>(49), \
  static_cast<const unsigned char>(50),  static_cast<const unsigned char>(51), \
  static_cast<const unsigned char>(52),  static_cast<const unsigned char>(53), \
  static_cast<const unsigned char>(54),  static_cast<const unsigned char>(55), \
  static_cast<const unsigned char>(56),  static_cast<const unsigned char>(57), \
  static_cast<const unsigned char>(58),  static_cast<const unsigned char>(59), \
  static_cast<const unsigned char>(60),  static_cast<const unsigned char>(61), \
  static_cast<const unsigned char>(62),  static_cast<const unsigned char>(63), \
  static_cast<const unsigned char>(64),  static_cast<const unsigned char>('a'), \
  static_cast<const unsigned char>('b'), static_cast<const unsigned char>('c'), \
  static_cast<const unsigned char>('d'), static_cast<const unsigned char>('e'), \
  static_cast<const unsigned char>('f'), static_cast<const unsigned char>('g'), \
  static_cast<const unsigned char>('h'), static_cast<const unsigned char>('i'), \
  static_cast<const unsigned char>('j'), static_cast<const unsigned char>('k'), \
  static_cast<const unsigned char>('l'), static_cast<const unsigned char>('m'), \
  static_cast<const unsigned char>('n'), static_cast<const unsigned char>('o'), \
  static_cast<const unsigned char>('p'), static_cast<const unsigned char>('q'), \
  static_cast<const unsigned char>('r'), static_cast<const unsigned char>('s'), \
  static_cast<const unsigned char>('t'), static_cast<const unsigned char>('u'), \
  static_cast<const unsigned char>('v'), static_cast<const unsigned char>('w'), \
  static_cast<const unsigned char>('x'), static_cast<const unsigned char>('y'), \
  static_cast<const unsigned char>('z'), static_cast<const unsigned char>(91), \
  static_cast<const unsigned char>(92),  static_cast<const unsigned char>(93), \
  static_cast<const unsigned char>(94),  static_cast<const unsigned char>(95), \
  static_cast<const unsigned char>(96),  static_cast<const unsigned char>('a'), \
  static_cast<const unsigned char>('b'), static_cast<const unsigned char>('c'), \
  static_cast<const unsigned char>('d'), static_cast<const unsigned char>('e'), \
  static_cast<const unsigned char>('f'), static_cast<const unsigned char>('g'), \
  static_cast<const unsigned char>('h'), static_cast<const unsigned char>('i'), \
  static_cast<const unsigned char>('j'), static_cast<const unsigned char>('k'), \
  static_cast<const unsigned char>('l'), static_cast<const unsigned char>('m'), \
  static_cast<const unsigned char>('n'), static_cast<const unsigned char>('o'), \
  static_cast<const unsigned char>('p'), static_cast<const unsigned char>('q'), \
  static_cast<const unsigned char>('r'), static_cast<const unsigned char>('s'), \
  static_cast<const unsigned char>('t'), static_cast<const unsigned char>('u'), \
  static_cast<const unsigned char>('v'), static_cast<const unsigned char>('w'), \
  static_cast<const unsigned char>('x'), static_cast<const unsigned char>('y'), \
  static_cast<const unsigned char>('z'), static_cast<const unsigned char>(123), \
  static_cast<const unsigned char>(124), static_cast<const unsigned char>(125), \
  static_cast<const unsigned char>(126), static_cast<const unsigned char>(127), \
  static_cast<const unsigned char>(128), static_cast<const unsigned char>(129), \
  static_cast<const unsigned char>(130), static_cast<const unsigned char>(131), \
  static_cast<const unsigned char>(132), static_cast<const unsigned char>(133), \
  static_cast<const unsigned char>(134), static_cast<const unsigned char>(135), \
  static_cast<const unsigned char>(136), static_cast<const unsigned char>(137), \
  static_cast<const unsigned char>(138), static_cast<const unsigned char>(139), \
  static_cast<const unsigned char>(140), static_cast<const unsigned char>(141), \
  static_cast<const unsigned char>(142), static_cast<const unsigned char>(143), \
  static_cast<const unsigned char>(144), static_cast<const unsigned char>(145), \
  static_cast<const unsigned char>(146), static_cast<const unsigned char>(147), \
  static_cast<const unsigned char>(148), static_cast<const unsigned char>(149), \
  static_cast<const unsigned char>(150), static_cast<const unsigned char>(151), \
  static_cast<const unsigned char>(152), static_cast<const unsigned char>(153), \
  static_cast<const unsigned char>(154), static_cast<const unsigned char>(155), \
  static_cast<const unsigned char>(156), static_cast<const unsigned char>(157), \
  static_cast<const unsigned char>(158), static_cast<const unsigned char>(159), \
  static_cast<const unsigned char>(160), static_cast<const unsigned char>(161), \
  static_cast<const unsigned char>(162), static_cast<const unsigned char>(163), \
  static_cast<const unsigned char>(164), static_cast<const unsigned char>(165), \
  static_cast<const unsigned char>(166), static_cast<const unsigned char>(167), \
  static_cast<const unsigned char>(168), static_cast<const unsigned char>(169), \
  static_cast<const unsigned char>(170), static_cast<const unsigned char>(171), \
  static_cast<const unsigned char>(172), static_cast<const unsigned char>(173), \
  static_cast<const unsigned char>(174), static_cast<const unsigned char>(175), \
  static_cast<const unsigned char>(176), static_cast<const unsigned char>(177), \
  static_cast<const unsigned char>(178), static_cast<const unsigned char>(179), \
  static_cast<const unsigned char>(180), static_cast<const unsigned char>(181), \
  static_cast<const unsigned char>(182), static_cast<const unsigned char>(183), \
  static_cast<const unsigned char>(184), static_cast<const unsigned char>(185), \
  static_cast<const unsigned char>(186), static_cast<const unsigned char>(187), \
  static_cast<const unsigned char>(188), static_cast<const unsigned char>(189), \
  static_cast<const unsigned char>(190), static_cast<const unsigned char>(191), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>(247), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>(247), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
  static_cast<const unsigned char>('�'), static_cast<const unsigned char>('�'), \
    \
  >> >> >> > samson_0 .7
};

bool iso_8859_is_letter(unsigned char c) {
  return _iso_8859_is_letter[ c ];
}

bool iso_8859_is_printable(unsigned char c) {
  return _iso_8859_is_printable[ c ];
}

unsigned char iso_8859_to_lower(unsigned char c) {
  return _iso_8859_to_lower[c];
}

unsigned char iso_8859_to_upper(unsigned char c) {
  return _iso_8859_to_upper[c];
}

// Static tables for UTF8 processing

static char UTF8_2_ISO_8859_1_len[] =
{
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* erroneous */
  2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 5, 6
};

static char UTF8_2_ISO_8859_1_mask[] = { 0x3F, 0x7F, 0x1F, 0x0F, 0x07,
                                         0x03, 0x01 };



unsigned long get_utf8_symbol(const char *str, int *length) {
  int len = UTF8_2_ISO_8859_1_len[(*str >> 2) & 0x3F];
  unsigned long u   = *str & UTF8_2_ISO_8859_1_mask[len];

  if (len == 0) {
    // Error
    if (length) {
      *length = -1;
    }
    return 0;                  // What to return?
  }

  for (++str; --len > 0 && (*str != '\0'); ++str) {
    if ((*str & 0xC0) != 0x80) {
      // Error
      if (length) {
        *length = -1;
      }
      return 0;                  // What to return?
    }

    u = (u << 6) | (*str & 0x3F);
  }

  // Return length
  if (length) {
    *length = len;  // Return unicode value
  }
  return u;
}

int serialize_utf8_symbol(unsigned long s, char *str) {
  if (s <= 127) {
    // One byte
    str[0] = (char)s;
    return 1;
  }

  if (s < 2047) {
    // Two bytes
    str[0] = 0xC0 | (s >> 6);
    str[1] = 0x80 | (s & 0x3F);
    return 2;
  }

  if (s < 65535) {
    // three bytes
    str[0] = 0xE0 | (s >> 12);
    str[1] = 0x80 | (s & 0xFFF) >> 6;
    str[2] = 0x80 | (s & 0x3F);
    return 3;
  }

  if (s < 1114111) {
    // four bytes
    str[0] = 0xF0 | (s >> 18);
    str[1] = 0x80 | (s & 0xFFFF) >> 12;
    str[2] = 0x80 | (s & 0x03FF) >> 6;
    str[2] = 0x80 | (s & 0x3F);
    return 4;
  }

  // Error
  return -1;
}

size_t strlenUTF8(const char *cad_utf) {
  char *p_utf;
  size_t lengthUTF8 = 0;

  if (cad_utf == NULL) {
    return 0;
  }

  p_utf = (char *)cad_utf;
  while (*p_utf != '\0') {
    int len = UTF8_2_ISO_8859_1_len[(*p_utf >> 2) & 0x3F];
    unsigned long u   = *p_utf & UTF8_2_ISO_8859_1_mask[len];

    // Try to skip latter...
    if (len == 0) {
      len = 5;
    }
    for (++p_utf; --len > 0 && (*p_utf != '\0'); ++p_utf) {
      if ((*p_utf & 0xC0) != 0x80) {
        break;
      }
      u = (u << 6) | (*p_utf & 0x3F);
    }

    lengthUTF8++;
  }
  return(lengthUTF8);
}
}




