/*
History
=======


Notes
=======

*/

#include "guardrgn.h"

#include "../bscript/impstr.h"

#include "mobile/charactr.h"
#include "polcfg.h"
#include "scrdef.h"
#include "scrstore.h"
#include "scrsched.h"
#include "uoscrobj.h"
#include "globals/uvars.h"

#include "network/client.h"
#include "network/cgdata.h"

#include "../clib/cfgelem.h"
#include "../clib/fileutil.h"

namespace Pol {
  namespace Core {
	JusticeRegion::JusticeRegion( Clib::ConfigElem& elem, RegionId id ) :
	  Region( elem, id ),
	  guarded_( elem.remove_bool( "Guarded", false ) ),
	  nocombat_( elem.remove_bool( "NoCombat", false ) ),
	  region_name_( elem.rest() ),
	  entertext_( elem.remove_string( "EnterText", "" ) ),
	  leavetext_( elem.remove_string( "LeaveText", "" ) ),
	  enter_script_( elem.remove_string( "EnterScript", "" ) ),
	  leave_script_( elem.remove_string( "LeaveScript", "" ) )
	{}

	void read_justice_zones()
	{
	  gamestate.justicedef = new JusticeDef( "Justice" );
	  read_region_data( *gamestate.justicedef,
						"regions/justice.cfg", // preferred
						"regions/regions.cfg", // other
						"JusticeRegion Region" );


	}

	bool JusticeRegion::RunEnterScript( Mobile::Character* chr )
	{
	  if ( enter_script_.empty() )
		return false;

	  ScriptDef sd;
	  if ( !sd.config_nodie( enter_script_, 0, 0 ) )
		return false;
	  if ( !sd.exists() )
		return false;

      Bscript::BObjectImp* res = run_script_to_completion( sd, new Module::ECharacterRefObjImp( chr ), new Bscript::String( region_name_ ) );
	  return res->isTrue();
	}

	bool JusticeRegion::RunLeaveScript( Mobile::Character* chr )
	{
	  if ( leave_script_.empty() )
		return false;

	  ScriptDef sd;
	  if ( !sd.config_nodie( leave_script_, 0, 0 ) )
		return false;
	  if ( !sd.exists() )
		return false;

      Bscript::BObjectImp* res = run_script_to_completion( sd, new Module::ECharacterRefObjImp( chr ), new Bscript::String( region_name_ ) );
	  return res->isTrue();
	}

	// This function is static!
	bool JusticeRegion::RunNoCombatCheck( Network::Client* client )
	{
		JusticeRegion* cur_justice_region = NULL;
	  
		if (client != NULL && client->gd != NULL)
			cur_justice_region = client->gd->justice_region;

		bool no_combat = false;
		if (cur_justice_region != NULL)
			no_combat = cur_justice_region->nocombat_;

		return no_combat;
	}
  }
}