/*
History
=======
2006/05/24 Shinigami: extended progverstr and buildtagstr upto 64 chars to hold Code names

Notes
=======

*/

#ifndef CLIB_PROGVER_H
#define CLIB_PROGVER_H
namespace Pol {
	extern char progverstr[64]; // must be assigned
	extern char buildtagstr[64]; // must be assigned
	extern unsigned int progver; // must be assigned
	extern const char compiledate[];
	extern const char compiletime[];
}
#endif
