/*
History
=======


Notes
=======

*/

#ifndef POLSTATS_H
#define POLSTATS_H

#include "../clib/rawtypes.h"
namespace Pol {
  namespace Core {
	class PolStats
	{
	public:
	  u64 bytes_received;
	  u64 bytes_sent;
	};
	//extern PolStats auxstats; (Not yet... -- Nando)
	//extern PolStats webstats;
  }
}
#endif
