/*
History
=======
2009/09/03 MuadDib:	  Changes for account related source file relocation
                      Changes for multi related source file relocation


Notes
=======

*/

#ifndef BOATEMOD_H
#define BOATEMOD_H

#include "../../bscript/execmodl.h"
namespace Pol {
  namespace Module {

	class UBoatExecutorModule : public Bscript::TmplExecutorModule<UBoatExecutorModule>
	{
	public:
	  UBoatExecutorModule( Bscript::Executor& exec ) :
		Bscript::TmplExecutorModule<UBoatExecutorModule>( "boat", exec ) {};

	  Bscript::BObjectImp* mf_MoveBoat( );
	  Bscript::BObjectImp* mf_MoveBoatRelative( );

	  Bscript::BObjectImp* mf_TurnBoat( );

	  Bscript::BObjectImp* mf_RegisterItemWithBoat( );
	  Bscript::BObjectImp* mf_BoatFromItem( );

	  Bscript::BObjectImp* mf_SystemFindBoatBySerial( );

	  Bscript::BObjectImp* mf_MoveBoatXY( );
	};
  }
}
#endif
