/*
History
=======

Notes
=======

*/

#include "filecont.h"

#include "logfacility.h"
#include <cstdio>
#include <stdexcept>

// disables unsafe warning for fopen
#ifdef _MSC_VER
#pragma warning(disable:4996) 
#define _CRT_SECURE_NO_WARNINGS 
#endif

namespace Pol {
  namespace Clib {
	FileContents::FileContents( const char* filename )
	{
	  FILE *fp = fopen( filename, "rb" );
	  if( fp == NULL )
	  {
        ERROR_PRINT << "Unable to open '" << filename << "' for reading.\n";
		throw std::runtime_error( "Error opening file" );
	  }

	  char buf[1024];
	  while( !ferror( fp ) && !feof( fp ) )
	  {
		size_t nread = fread( buf, 1, sizeof buf, fp );
		if( nread )
		  contents_.append( buf, nread );
	  }

	  fclose( fp );
	}

	const char* FileContents::contents() const
	{
	  return contents_.c_str();
	}

	void FileContents::set_contents( const std::string& str )
	{
	  contents_ = str;
	}

  }
}