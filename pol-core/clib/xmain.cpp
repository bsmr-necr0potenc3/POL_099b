/*
History
=======
2009/07/20 MuadDib: Removed calls and use of StackWalker for leak detection. vld is now used, much better.
                    Removed StackWalker.cpp/.h from the vcproj files also.

Notes
=======

*/

#include "xmain.h"

#include "Debugging/ExceptionParser.h"

#include "clib.h"
#include "logfacility.h"
#include "strexcpt.h"

#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN
#	include <windows.h> // for GetModuleFileName
#	include <crtdbg.h>
#   include <psapi.h>
#   pragma comment(lib, "psapi.lib") // 32bit is a bit dumb..
#else
#   include <unistd.h>
#   include <sys/resource.h>
#endif
#include <stdexcept>
#include <string>

namespace Pol {
  unsigned int refptr_count;
  static void parse_args( int argc, char *argv[] );
}

int main( int argc, char *argv[] )
{
	using namespace Pol;
	Clib::Logging::LogFacility logger;
	Clib::Logging::initLogging( &logger );
	Clib::ExceptionParser::InitGlobalExceptionCatching();

	setlocale(LC_TIME,"");
    int exitcode = 0;

    try 
    {
        Clib::InstallStructuredExceptionHandler();
	// FIXME: 2008 Upgrades needed here? Make sure this still valid on 2008
#if defined(_WIN32) && defined(_DEBUG) && _MSC_VER >= 1300
        // on VS.NET, disable obnoxious heap checking
        int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        flags &= 0x0000FFFF; // set heap check frequency to 0
        _CrtSetDbgFlag( flags );
#endif
        parse_args( argc, argv );
        exitcode = xmain( argc, argv );
    }
    catch( const char *msg )
    {
        ERROR_PRINT << "Execution aborted due to: " << msg << "\n";
        exitcode = 1;
    }
    catch( std::string& str )
    {
      ERROR_PRINT << "Execution aborted due to: " << str << "\n";
        exitcode = 1;
    }       // egcs has some trouble realizing 'exception' should catch
    catch (std::runtime_error& re)   // runtime_errors, so...
    {
      ERROR_PRINT << "Execution aborted due to: " << re.what( ) << "\n";
        exitcode = 1;
    }
    catch (std::exception& ex)
    {
      ERROR_PRINT << "Execution aborted due to: " << ex.what( ) << "\n";
        exitcode = 1;
    }
    catch( int xn )
    {
        // Something that throws an integer is responsible for printing
        // its own error message.  
        // "throw 3" is meant as an alternative to exit(3).
        exitcode = xn;
    }
#ifndef _WIN32
    catch( ... )
    {
      ERROR_PRINT << "Execution aborted due to generic exception." << "\n";
        exitcode = 2;
    }
#endif    
	Clib::Logging::global_logger->wait_for_empty_queue();

    return exitcode;
}

namespace Pol {
  std::string xmain_exepath;
  std::string xmain_exedir;

  std::string fix_slashes(std::string pathname)
  {
	std::string::size_type bslashpos;
    while (std::string::npos != (bslashpos = pathname.find('\\')))
	{
	  pathname.replace( bslashpos, 1, 1, '/' );
	}
	return pathname;
  }

  static void parse_args( int /*argc*/, char *argv[] )
  {
	std::string exe_path;

#ifdef _WIN32
	// useless windows shell (cmd) usually doesn't tell us the whole path.  
	char module_path[_MAX_PATH];
	if ( GetModuleFileName( NULL, module_path, sizeof module_path ) )
	  exe_path = module_path;
	else
	  exe_path = argv[0];
#else
	// linux shells are generally more informative.
	exe_path = argv[0];
#endif

	xmain_exepath = fix_slashes( exe_path );
	xmain_exedir = fix_slashes( exe_path );

    std::string::size_type pos = xmain_exedir.find_last_of("/");
    if (pos != std::string::npos)
	{
	  xmain_exedir.erase( pos );
	  xmain_exedir += "/";
	}
  }
  namespace Clib  {

    size_t getCurrentMemoryUsage( )
    {
#if defined(_WIN32)
      PROCESS_MEMORY_COUNTERS info;
      GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof( info ) );
      return (size_t)info.WorkingSetSize;

#else
      long rss = 0L;
      FILE* fp = NULL;
      if ( ( fp = fopen( "/proc/self/statm", "r" ) ) == NULL )
        return (size_t)0L;		/* Can't open? */
      if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
      {
        fclose( fp );
        return (size_t)0L;		/* Can't read? */
      }
      fclose( fp );
      return (size_t)rss * (size_t)sysconf( _SC_PAGESIZE );
#endif
    }
  }
}
