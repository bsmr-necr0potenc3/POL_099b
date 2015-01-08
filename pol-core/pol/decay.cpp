/*
History
=======
2005/01/23 Shinigami: decay_items & decay_thread - Tokuno MapDimension doesn't fit blocks of 64x64 (WGRID_SIZE)
2010/03/28 Shinigami: Transmit Pointer as Pointer and not Int as Pointer within decay_thread_shadow

Notes
=======

*/

#include "decay.h"

#include "../clib/esignal.h"
#include "../plib/realm.h"

#include "core.h"
#include "item/item.h"
#include "item/itemdesc.h"
#include "gameclck.h"
#include "polclock.h"
#include "polsem.h"
#include "realms.h"
#include "scrsched.h"
#include "syshook.h"
#include "ufunc.h"
#include "uofile.h"
#include "uoscrobj.h"
#include "globals/uvars.h"
#include "globals/state.h"
#include "uworld.h"

namespace Pol {
  namespace Core {
	///
	/// [1] Item Decay Criteria
	///     An Item is allowed to decay if ALL of the following are true:
	///        - it is not In Use
	///        - it is Movable, OR it is a Corpse
	///        - its 'decayat' member is nonzero 
	///            AND the Game Clock has passed this 'decayat' time
	///        - it is not supported by a multi,
	///            OR its itemdesc.cfg entry specifies 'DecaysOnMultis 1'
	///        - it itemdesc.cfg entry specifies no 'DestroyScript', 
	///            OR its 'DestroyScript' returns nonzero.
	///
	/// [2] Decay Action
	///     Container contents are moved to the ground at the Container's location
	///     before destroying the container.
	///

	void decay_worldzone( unsigned wx, unsigned wy, Plib::Realm* realm )
	{
	  Zone& zone = realm->zone[wx][wy];
	  gameclock_t now = read_gameclock();

	  for ( ZoneItems::size_type idx = 0; idx < zone.items.size(); ++idx )
	  {
		Items::Item* item = zone.items[idx];
		if ( item->should_decay( now ) )
		{
		  // check the CanDecay syshook first if it returns 1 go over to other checks
		  if ( gamestate.system_hooks.can_decay )
		  {
			if ( !gamestate.system_hooks.can_decay->call( new Module::EItemRefObjImp( item ) ) )
			  continue;
		  }

		  const Items::ItemDesc& descriptor = item->itemdesc();
		  Multi::UMulti* multi = realm->find_supporting_multi( item->x, item->y, item->z );

		  // some things don't decay on multis:
		  if ( multi != NULL && !descriptor.decays_on_multis )
			continue;

		  if ( !descriptor.destroy_script.empty() && !item->inuse() )
		  {
			bool decayok = call_script( descriptor.destroy_script, item->make_ref() );
			if ( !decayok )
			  continue;
		  }

		  item->spill_contents( multi );
		  destroy_item( item );
		  --idx;
		}
	  }
	}


	// this is used in single-thread mode only
	void decay_items()
	{
	  static unsigned wx = ~0u;
	  static unsigned wy = 0;

	  Plib::Realm* realm;
	  for (auto itr = gamestate.Realms.begin(); itr != gamestate.Realms.end(); ++itr )
	  {
		realm = *itr;
		if ( !--stateManager.cycles_until_decay_worldzone )
		{
		  stateManager.cycles_until_decay_worldzone = stateManager.cycles_per_decay_worldzone;

		  unsigned int gridwidth = realm->width() / WGRID_SIZE;
		  unsigned int gridheight = realm->height() / WGRID_SIZE;

		  // Tokuno-Fix
		  if ( gridwidth * WGRID_SIZE < realm->width() )
			gridwidth++;
		  if ( gridheight * WGRID_SIZE < realm->height() )
			gridheight++;

		  if ( ++wx >= gridwidth )
		  {
			wx = 0;
			if ( ++wy >= gridheight )
			{
			  wy = 0;
			}
		  }
		  decay_worldzone( wx, wy, realm );
		}
	  }
	}

	///
	/// [3] Decay Sweep
	///     Each 64x64 tile World Zone is checked for decay approximately
	///     once every 10 minutes
	///

	void decay_single_zone( Plib::Realm* realm, unsigned gridx, unsigned gridy, unsigned& wx, unsigned& wy )
	{
	  if ( ++wx >= gridx )
	  {
		wx = 0;
		if ( ++wy >= gridy )
		{
		  wy = 0;
		}
	  }
	  decay_worldzone( wx, wy, realm );
	}

	void decay_thread( void* arg ) //Realm*
	{
	  unsigned wx = ~0u;
	  unsigned wy = 0;
      Plib::Realm* realm = static_cast<Plib::Realm*>( arg );

	  unsigned gridx = ( realm->width() / WGRID_SIZE );
	  unsigned gridy = ( realm->height() / WGRID_SIZE );

	  // Tokuno-Fix
	  if ( gridx * WGRID_SIZE < realm->width() )
		gridx++;
	  if ( gridy * WGRID_SIZE < realm->height() )
		gridy++;

	  unsigned sleeptime = ( 60 * 10L * 1000 ) / ( gridx * gridy );
	  while ( !Clib::exit_signalled )
	  {
		{
		  PolLock lck;
		  polclock_checkin();
		  decay_single_zone( realm, gridx, gridy, wx, wy );
		  restart_all_clients();
		}
		// sweep entire world every 10 minutes
		// (60 * 10 * 1000) / (96 * 64) -> (600000 / 6144) -> 97 ms

		pol_sleep_ms( sleeptime );
	  }
	}

	void decay_thread_shadow( void* arg ) //Realm*
	{
	  unsigned wx = ~0u;
	  unsigned wy = 0;
      unsigned id = static_cast<Plib::Realm*>( arg )->shadowid;

	  if ( gamestate.shadowrealms_by_id[id] == NULL )
		return;
	  unsigned width = gamestate.shadowrealms_by_id[id]->width();
	  unsigned height = gamestate.shadowrealms_by_id[id]->height();

	  unsigned gridx = ( width / WGRID_SIZE );
	  unsigned gridy = ( height / WGRID_SIZE );

	  // Tokuno-Fix
	  if ( gridx * WGRID_SIZE < width )
		gridx++;
	  if ( gridy * WGRID_SIZE < height )
		gridy++;

	  unsigned sleeptime = ( 60 * 10L * 1000 ) / ( gridx * gridy );
	  while ( !Clib::exit_signalled )
	  {
		{
		  PolLock lck;
		  polclock_checkin();
		  if ( gamestate.shadowrealms_by_id[id] == NULL ) // is realm still there?
			break;
		  decay_single_zone( gamestate.shadowrealms_by_id[id], gridx, gridy, wx, wy );
		  restart_all_clients();
		}
		// sweep entire world every 10 minutes
		// (60 * 10 * 1000) / (96 * 64) -> (600000 / 6144) -> 97 ms

		pol_sleep_ms( sleeptime );
	  }
	}

	void calc_grid_count(const Plib::Realm* realm, unsigned *gridx, unsigned *gridy)
	{
	  (*gridx) = ( realm->width() / WGRID_SIZE );
	  (*gridy) = ( realm->height() / WGRID_SIZE );
	  // Tokuno-Fix
	  if ( (*gridx) * WGRID_SIZE < realm->width() )
		(*gridx)++;
	  if ( (*gridy) * WGRID_SIZE < realm->height() )
		(*gridy)++;
	}

	bool should_switch_realm(size_t index, unsigned x, unsigned y, unsigned *gridx, unsigned *gridy)
	{
	  (void)x;
	  if (index >= gamestate.Realms.size())
		return true;
	  Plib::Realm* realm = gamestate.Realms[index];
	  if (realm == nullptr)
		return true;

	  calc_grid_count( realm, gridx, gridy );

	  // check if ++y would result in reset
	  if (y + 1 >= (*gridy))
		return true;
	  return false;
	}

	void decay_single_thread( void* arg ) 
	{
	  (void)arg;
	  // calculate total grid count, based on current realms
	  unsigned total_grid_count = 0;
	  for (const auto& realm : gamestate.Realms)
	  {
		unsigned _gridx, _gridy;
		calc_grid_count( realm, &_gridx, &_gridy );
		total_grid_count += (_gridx*_gridy);
	  }
	  // sweep every realm ~10minutes -> 36ms for 6 realms
	  unsigned sleeptime = ( 60 * 10L * 1000 ) / total_grid_count;
	  sleeptime = std::max( sleeptime, 30u ); // limit to 30ms
	  size_t realm_index=~0u;
	  unsigned wx = 0;
	  unsigned wy = 0;
	  unsigned gridx = 0;
	  unsigned gridy = 0;
	  while ( !Clib::exit_signalled )
	  {
		{
		  PolLock lck;
		  polclock_checkin();
		  // check if realm_index is still valid and if y is still in valid range
		  if (should_switch_realm(realm_index, wx, wy, &gridx, &gridy))
		  {
			++realm_index;
			if (realm_index >= gamestate.Realms.size())
			  realm_index = 0;
			wx = 0;
			wy = 0;
		  }
		  else
		  {
			if ( ++wx >= gridx )
			{
			  wx = 0;
			  if ( ++wy >= gridy )
			  {
				POLLOG_ERROR << "SHOULD NEVER HAPPEN\n";
				wy = 0;
			  }
			}
		  }
		  decay_worldzone( wx, wy, gamestate.Realms[realm_index] );
		  restart_all_clients();
		}
		pol_sleep_ms( sleeptime );
	  }
	}

  }
}