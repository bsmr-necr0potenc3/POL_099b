/*
History
=======
2009/09/01 Turley:    VS2005/2008 Support moved inline MultiDef::getkey to .h
2009/09/03 MuadDib:   Relocation of multi related cpp/h

Notes
=======

*/

#include "multidefs.h"

#include "../multi/multidef.h"


namespace Pol {
  namespace Multi {
	MultiDefBuffer multidef_buffer;

	MultiDefBuffer::MultiDefBuffer() :
	  multidefs_by_multiid()
	{
	}
	MultiDefBuffer::~MultiDefBuffer()
	{}

	void MultiDefBuffer::deinitialize()
	{
	  Multi::MultiDefs::iterator iter = multidefs_by_multiid.begin();
	  for ( ; iter != multidefs_by_multiid.end(); ++iter )
	  {
		if ( iter->second != NULL )
		  delete iter->second;
		iter->second = NULL;
	  }
	  multidefs_by_multiid.clear();
	}
  }
}