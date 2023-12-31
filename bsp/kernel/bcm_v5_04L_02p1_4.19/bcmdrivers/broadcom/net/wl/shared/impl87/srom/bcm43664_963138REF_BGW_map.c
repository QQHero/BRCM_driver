#include <typedefs.h>

uint16 wl_srom_map_4365[590] = {
/* NOTE: */
/* BCM94366EMCM5 (p145) (5G band, 4X4 11ac, medium tx power)*/
/* 4366E0 */
/* PA values MUST be calibrated and refilled for the actual EEPROM/SROM-less design*/
/* MAC address are zero'ed */
/* CRC here is not recalculated, do not program this to EEPROM/SROM, this is for EEPROM/SROM-less use only */

/* 4366E C0 */
/* srom[000]:*/    0x3801,    0x3000,    0x077d,    0x14e4,    0x0218,    0x1b7e,    0x0a00,    0x2bc4,
/* srom[008]:*/    0x2a64,    0x2964,    0x2c64,    0x3ce7,    0x480a,    0x113c,    0x2132,    0x3164,
/* srom[016]:*/    0x1852,    0x0005,    0x1f2e,    0x4df7,    0x8080,    0x000b,    0x8630,    0x0190,
/* srom[024]:*/    0x5f00,    0x41f6,    0x8630,    0x0191,    0x8300,    0x01eb,    0x9f01,    0x65f5,
/* srom[032]:*/    0x8200,    0xd800,    0x8010,    0x007c,    0x1f00,    0x0004,    0x8000,    0x200c,
/* srom[040]:*/    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,
/* srom[048]:*/    0x43c5,    0x8000,    0x0002,    0x0000,    0x3ff7,    0x1800,    0x0005,    0xffff,
/* srom[056]:*/    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,
/* srom[064]:*/    0x4d55,    0x1145,    0x0000,    0x1000,    0x0004,    0x0000,    0x0000,    0x0000,
/* srom[072]:*/    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0xffff,    0xffff,    0xffff,
/* srom[080]:*/    0x0f0f,    0x0202,    0x0202,    0x0202,    0x00ff,    0x1001,    0x0031,    0x78ff,
/* srom[088]:*/    0xffff,    0xffff,    0x9c40,    0xffff,    0x5a28,    0x7f7f,    0xffff,    0xffff,
/* srom[096]:*/    0x0000,    0xabab,    0x0000,    0x0000,    0x0202,    0x000a,    0x6558,    0x6e66,
/* srom[104]:*/    0xffff,    0x0000,    0x0000,    0x0005,    0xffff,    0x3142,    0x3131,    0x3142,
/* srom[112]:*/    0x3131,    0x3142,    0x3131,    0x3142,    0x3131,    0x3142,    0x3131,    0x7654,
/* srom[120]:*/    0xdb98,    0x7654,    0xdb98,    0x7654,    0xdb98,    0x7654,    0xdb98,    0x7654,
/* srom[128]:*/    0xdb98,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0x0000,
/* srom[136]:*/    0x0000,    0x0000,    0x0000,    0xfefe,    0xfefe,    0xfefe,    0xfefe,    0xffff,
/* srom[144]:*/    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,
/* srom[152]:*/    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,
/* srom[160]:*/    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,    0xffff,
/* srom[168]:*/    0x0000,    0x0000,    0x7222,    0x9977,    0x7222,    0x9977,    0x7722,    0x0022,
/* srom[176]:*/    0x7530,    0xca98,    0x7652,    0xca98,    0x7650,    0xdb98,    0x2f25,    0xd75f,
/* srom[184]:*/    0x7530,    0xca98,    0x7652,    0xca98,    0x7651,    0xdb98,    0xce57,    0xdc5f,
/* srom[192]:*/    0x7530,    0xca98,    0x7653,    0xca98,    0x7652,    0xdb98,    0xd563,    0x0000,
/* srom[200]:*/    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,
/* srom[208]:*/    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,
/* srom[216]:*/    0x0000,    0x0000,    0x0000,    0x0000,    0xffff,    0x0f12,    0xffff,    0xffff,
/* srom[224]:*/    0xffff,    0xffff,    0xffff,    0x1122,    0x31ae,    0x422a,    0x31b0,    0x7b2,
/* srom[232]:*/    0xffff,    0xffff,    0x7530,    0xca98,    0x7653,    0xca98,    0x7653,    0xdb98,
/* srom[240]:*/    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x7531,    0xca98,    0x7654,
/* srom[248]:*/    0xca98,    0x7654,    0xdb98,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,
/* srom[256]:*/    0x5e54,    0x1b27,    0xe53b,    0x3acf,    0x26fb,    0xabab,    0x5e5e,    0x5e5e,
/* srom[264]:*/    0x1892,    0xd237,    0x10bf,    0x211d,    0x16de,    0xeba6,    0x50e2,    0x2aaf,
/* srom[272]:*/    0x17f7,    0xd8ea,    0x15c0,    0x21b9,    0x184c,    0xe595,    0x4e0f,    0x2bec,
/* srom[280]:*/    0x18cb,    0xe46e,    0x50e5,    0x2c7c,    0x1ec1,    0xc1e4,    0x0cf4,    0x2139,
/* srom[288]:*/    0x185c,    0xddf4,    0x22c2,    0x248f,    0x17c4,    0xe4b0,    0x3719,    0x2747,
/* srom[296]:*/    0x186a,    0xe413,    0x3d66,    0x290b,    0x18a2,    0xe18f,    0x2b6f,    0x2604,
/* srom[304]:*/    0x18a6,    0xe1f2,    0x287d,    0x251a,    0x180f,    0xf27c,    0x3edb,    0x26a9,
/* srom[312]:*/    0x17c6,    0xebdf,    0x3bc9,    0x272f,    0x17ea,    0xe909,    0x2645,    0x2392,
/* srom[320]:*/    0x1ae6,    0xcb20,    0x09ef,    0x1fe1,    0x192f,    0xe76c,    0x36a1,    0x26f8,
/* srom[328]:*/    0x5e54,    0x1b47,    0xea0d,    0x4488,    0x27e6,    0xabab,    0x5e5e,    0x5e5e,
/* srom[336]:*/    0x191f,    0xcebb,    0x1382,    0x227b,    0x17ea,    0xdb1d,    0x367d,    0x2a7d,
/* srom[344]:*/    0x182e,    0xe04e,    0x3dd0,    0x2a74,    0x1875,    0xdc17,    0x2779,    0x25a3,
/* srom[352]:*/    0x18d5,    0xd571,    0x18e8,    0x2345,    0xffff,    0xffff,    0xffff,    0xffff,
/* srom[360]:*/    0x17c4,    0xe9f2,    0x3b10,    0x271a,    0x1828,    0xda14,    0x1e7f,    0x23b9,
/* srom[368]:*/    0x17aa,    0xe6cd,    0x324a,    0x2667,    0x18a4,    0xe110,    0x2b82,    0x2592,
/* srom[376]:*/    0x17fb,    0xf18f,    0x4fc3,    0x28fc,    0x1953,    0xe3ab,    0x3648,    0x2755,
/* srom[384]:*/    0x187f,    0xdbc9,    0x1c23,    0x22d3,    0x1912,    0xdd14,    0x2a4a,    0x265b,
/* srom[392]:*/    0x196e,    0xdd80,    0x244e,    0x24c0,    0x18c4,    0xe7ff,    0x3735,    0x2728,
/* srom[400]:*/    0x5e54,    0x1b61,    0xea82,    0x5989,    0x2ac4,    0xabab,    0x5e5e,    0x5e5e,
/* srom[408]:*/    0x19a0,    0xca3c,    0x0a7e,    0x203a,    0x17c1,    0xe636,    0x427c,    0x2934,
/* srom[416]:*/    0x1827,    0xe45d,    0x3973,    0x2809,    0x197a,    0xd8d1,    0x2bc6,    0x27d6,
/* srom[424]:*/    0x1911,    0xdcb7,    0x2799,    0x2625,    0x1e6a,    0xc31d,    0x0ec6,    0x21c0,
/* srom[432]:*/    0x1905,    0xdd90,    0x29a7,    0x2690,    0x17e2,    0xe4da,    0x2c96,    0x2542,
/* srom[440]:*/    0x17ea,    0xef5a,    0x4034,    0x2705,    0x19c6,    0xd798,    0x1b5c,    0x239c,
/* srom[448]:*/    0x19b4,    0xdb06,    0x1ee3,    0x23cf,    0x198c,    0xddda,    0x1776,    0x219c,
/* srom[456]:*/    0x18d1,    0xe1cb,    0x2e4f,    0x2682,    0x1aea,    0xc99c,    0x0a71,    0x209e,
/* srom[464]:*/    0x1aeb,    0xd5ae,    0x1d4d,    0x2440,    0x1b55,    0xce42,    0x0ee6,    0x2176,
/* srom[472]:*/    0xffff,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,
/* srom[480]:*/    0x0000,    0x0000,    0x0000,    0xabab,    0xabab,    0xabab,    0x000e,    0x0000,
/* srom[488]:*/    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0001,    0x4444,    0x3333,
/* srom[496]:*/    0x2222,    0x0000,    0x4444,    0x3333,    0x2222,    0x0000,    0x0000,    0x0000,
/* srom[504]:*/    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,    0x0000,
/* srom[512]:*/    0x5e54,    0x1b54,    0xf01c,    0x5b00,    0x2ad4,    0xabab,    0x5e5e,    0x5e5e,
/* srom[520]:*/    0x18a7,    0xe0dc,    0x3997,    0x2991,    0x17d0,    0xe3e9,    0x447d,    0x2bf1,
/* srom[528]:*/    0x182a,    0xe71f,    0x4ae6,    0x2c09,    0x191f,    0xdddd,    0x33b7,    0x29a9,
/* srom[536]:*/    0x1911,    0xe31c,    0x3ea3,    0x2ad0,    0x1b6a,    0xeb16,    0x3c8c,    0x2772,
/* srom[544]:*/    0x1849,    0xea37,    0x3b0a,    0x27f3,    0x17c8,    0xe125,    0x1bb2,    0x2304,
/* srom[552]:*/    0x18b2,    0xe134,    0x2672,    0x2589,    0x1942,    0xe27e,    0x3903,    0x2984,
/* srom[560]:*/    0x1964,    0xdf80,    0x2819,    0x265c,    0x19fc,    0xe519,    0x3495,    0x26dd,
/* srom[568]:*/    0x1916,    0xe31d,    0x35d4,    0x27ea,    0x19ea,    0xd899,    0x1b23,    0x2392,
/* srom[576]:*/    0x1977,    0xee57,    0x50c1,    0x2a8d,    0x1a2a,    0xdfc6,    0x23bf,    0x24b0,
/* srom[584]:*/    0xffff,    0xffff,    0x0800,    0x0048,    0xffff,    0x720d        

};

