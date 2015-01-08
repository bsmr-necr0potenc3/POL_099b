/*
History
=======


Notes
=======

*/

#ifndef SYSHOOKSCRIPT_H
#define SYSHOOKSCRIPT_H

#include "scrdef.h"
#include "uoexec.h"

#include <string>
#include <vector>

namespace Pol {
  namespace Plib {
	class Package;
  }
  namespace Core {
	
	class ExportScript
	{
	public:
	  ExportScript( const Plib::Package* pkg, std::string scriptname );
	  ExportScript( const ScriptDef& isd );
	  bool Initialize();
	  const std::string& scriptname() const;
	  bool FindExportedFunction( const std::string& name, unsigned args, unsigned& PC ) const;
	  bool FindExportedFunction( const char* name, unsigned args, unsigned& PC ) const;

	  bool call( unsigned PC,
				 Bscript::BObjectImp* p0 ); // throw()
	  bool call( unsigned PC,
				 Bscript::BObjectImp* p0, Bscript::BObjectImp* p1 ); // throw()
	  bool call( unsigned PC,
				 Bscript::BObjectImp* p0, Bscript::BObjectImp* p1, Bscript::BObjectImp* p2 ); // throw()
	  bool call( unsigned PC,
				 Bscript::BObjectImp* p0, Bscript::BObjectImp* p1, Bscript::BObjectImp* p2, Bscript::BObjectImp* p3 ); // throw()

	  std::string call_string( unsigned PC,
							   Bscript::BObjectImp* p0, Bscript::BObjectImp* p1 ); // throw()
	  std::string call_string( unsigned PC,
							   Bscript::BObjectImp* p0, Bscript::BObjectImp* p1, Bscript::BObjectImp* p2 ); // throw()

	  int call_long( unsigned PC, Bscript::BObjectImp* p0 ); // throw()
	  int call_long( unsigned PC, Bscript::BObjectImp* p0, Bscript::BObjectImp* p1 ); // throw()

	  Bscript::BObjectImp* call( unsigned PC, Bscript::BObjectImp* p0, std::vector<Bscript::BObjectRef>& pmore );
	  Bscript::BObject call( unsigned PC, Bscript::BObjectImp* p0, Bscript::BObjectImpRefVec& pmore );

	  Bscript::BObject call_object( unsigned PC, Bscript::BObjectImp* p0, Bscript::BObjectImp* p1 );
	  Bscript::BObject call_object( unsigned PC, Bscript::BObjectImp* p0, Bscript::BObjectImp* p1, Bscript::BObjectImp* p2 );

	  void SaveStack( Bscript::BackupStruct& backup );
	  void LoadStack( Bscript::BackupStruct& backup );

	  friend class SystemHook;

	private:
	  ScriptDef sd;
	  UOExecutor uoexec;
	};
  }
}

#endif