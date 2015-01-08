/*
History
=======
2006/09/27 Shinigami: GCC 3.4.x fix - added "#include "charactr.h" because of ForEachMobileInVisualRange
2009/10/17 Turley:    added ForEachItemInRange & ForEachItemInVisualRange

Notes
=======

*/


#ifndef UWORLD_H
#define UWORLD_H

#ifndef __UOBJECT_H
#include "uobject.h"
#endif
#ifndef ITEM_H
#include "item/item.h"
#endif
#ifndef MULTI_H
#include "multi/multi.h"
#endif
#ifndef __CHARACTR_H
#include "mobile/charactr.h"
#endif
#include "zone.h"

#include "../clib/passert.h"
#include "../plib/realm.h"

#include <vector>

namespace Pol {
  namespace Mobile {
	class Character;
  }
  namespace Multi {
	class UMulti;
  }
  namespace Items {
	class Item;
  }
  namespace Core {

	void add_item_to_world( Items::Item* item );
	void remove_item_from_world( Items::Item* item );

	void add_multi_to_world( Multi::UMulti* multi );
	void remove_multi_from_world( Multi::UMulti* multi );
	void move_multi_in_world( unsigned short oldx, unsigned short oldy,
							  unsigned short newx, unsigned short newy,
							  Multi::UMulti* multi, Plib::Realm* oldrealm );

	void SetCharacterWorldPosition( Mobile::Character* chr, Plib::WorldChangeReason reason);
	void ClrCharacterWorldPosition( Mobile::Character* chr, Plib::WorldChangeReason reason);
	void MoveCharacterWorldPosition( unsigned short oldx, unsigned short oldy,
									 unsigned short newx, unsigned short newy,
									 Mobile::Character* chr, Plib::Realm* oldrealm );

	void SetItemWorldPosition( Items::Item* item );
	void ClrItemWorldPosition( Items::Item* item );
	void MoveItemWorldPosition( unsigned short oldx, unsigned short oldy,
								Items::Item* item, Plib::Realm* oldrealm );

	int get_toplevel_item_count();
	int get_mobile_count();

    void optimize_zones();

	typedef std::vector<Mobile::Character*> ZoneCharacters;
	typedef std::vector<Multi::UMulti*> ZoneMultis;
	typedef std::vector<Items::Item*> ZoneItems;

	struct Zone
	{
	  ZoneCharacters characters;
      ZoneCharacters npcs;
	  ZoneItems items;
	  ZoneMultis multis;
	};

	const unsigned WGRID_SIZE = 64;
	const unsigned WGRID_SHIFT = 6;

    inline void zone_convert( unsigned short x, unsigned short y, unsigned short* wx, unsigned short* wy, const Plib::Realm* realm )
	{
	  passert( x < realm->width() );
	  passert( y < realm->height() );

	  (*wx) = x >> WGRID_SHIFT;
	  (*wy) = y >> WGRID_SHIFT;
	}

	inline void zone_convert_clip( int x, int y, const Plib::Realm* realm, unsigned short* wx, unsigned short* wy )
	{
	  if ( x < 0 )
		x = 0;
	  if ( y < 0 )
		y = 0;
	  if ( (unsigned)x >= realm->width() )
		x = realm->width() - 1;
	  if ( (unsigned)y >= realm->height() )
		y = realm->height() - 1;

	  (*wx) = static_cast<unsigned short>( x >> WGRID_SHIFT );
	  (*wy) = static_cast<unsigned short>( y >> WGRID_SHIFT );
	}

	inline Zone& getzone( unsigned short x, unsigned short y, Plib::Realm* realm )
	{
	  passert( x < realm->width() );
	  passert( y < realm->height() );

	  return realm->zone[x >> WGRID_SHIFT][y >> WGRID_SHIFT];
	}

	namespace {
	  struct CoordsArea;
	}
    // Helper functions to iterator over world items in given realm an range
    // takes any function with only one open parameter
    // its recommended to use lambdas even if std::bind can also be used, but a small benchmark showed
    // that std::bind is a bit slower (compiler can optimize better lambdas)
    // Usage:
    // WorldIterator<PlayerFilter>::InRange(...)

    // main struct, define as template param which filter to use
    template <class Filter>
    struct WorldIterator
    {
      template <typename F>
	  static void InRange( u16 x, u16 y, const Plib::Realm* realm, unsigned range, F &&f );
      template <typename F>
	  static void InVisualRange( const UObject* obj, F &&f );
      template <typename F>
	  static void InBox( u16 x1, u16 y1, u16 x2, u16 y2, const Plib::Realm* realm, F &&f );
    protected:
      template <typename F>
	  static void _forEach( const CoordsArea &coords, const Plib::Realm* realm, F &&f );
    };

	enum class FilterType
	{
	  Mobile,		// iterator over npcs and players
	  Player,		// iterator over player
	  OnlinePlayer, // iterator over online player
	  NPC,			// iterator over npcs 
	  Item,			// iterator over items
	  Multi			// iterator over multis
	};

	// Filter implementation struct,
	// specializations for the enum values are given
	template <FilterType T>
	struct FilterImp
	{
	  friend struct WorldIterator<FilterImp<T>>; 
	protected:
      template <typename F>
	  static void call( Core::Zone &zone, const CoordsArea &coords, F &&f );
	};

	// shortcuts for filtering
	typedef FilterImp < FilterType::Mobile >		MobileFilter;
	typedef FilterImp < FilterType::Player >		PlayerFilter;
	typedef FilterImp < FilterType::OnlinePlayer >	OnlinePlayerFilter;
	typedef FilterImp < FilterType::NPC >			NPCFilter;
	typedef FilterImp < FilterType::Item >			ItemFilter;
	typedef FilterImp < FilterType::Multi >			MultiFilter;

	namespace {
      // template independent code
	  struct CoordsArea
      {
		// structure to hold the world and shifted coords
		CoordsArea( u16 x, u16 y, const Plib::Realm* realm, unsigned range); // create from range
		CoordsArea( u16 x1, u16 y1, u16 x2, u16 y2, const Plib::Realm* realm ); // create from box
		bool inRange( const UObject *obj ) const;

		// shifted coords
        u16 wxL;
        u16 wyL;
        u16 wxH;
        u16 wyH;

	  private:
		void convert( int xL, int yL, int xH, int yH, const Plib::Realm* realm );

		// plain coords
        int xL;
        int yL;
        int xH;
        int yH;
      };
    }
	///////////////
	// imp
	namespace {
	  CoordsArea::CoordsArea( u16 x, u16 y, const Plib::Realm* realm, unsigned range )
	  {
		convert( x - range, y - range, x + range, y + range, realm );
		xL = x - range;
		if ( xL < 0 )
		  xL = 0;
		yL = y - range;
		if ( yL < 0 )
		  yL = 0;
		xH = x + range;
		yH = y + range;
	  }

	  CoordsArea::CoordsArea( u16 x1, u16 y1, u16 x2, u16 y2, const Plib::Realm* realm )
	  {
		convert( x1, y1, x2, y2, realm );
		xL = x1;
		yL = y1;
		xH = x2;
		yH = y2;
	  }

	  bool CoordsArea::inRange( const UObject *obj) const
	  {
		return ( obj->x >= xL && obj->x <= xH &&
				 obj->y >= yL && obj->y <= yH );
	  }

	  void CoordsArea::convert( int xL, int yL, int xH, int yH, const Plib::Realm* realm )
	  {
		zone_convert_clip( xL, yL, realm, &wxL, &wyL );
		zone_convert_clip( xH, yH, realm, &wxH, &wyH );
		passert( wxL <= wxH );
		passert( wyL <= wyH );
	  }
	} // namespace

	template <class Filter>
	template <typename F>
    void WorldIterator<Filter>::InRange( u16 x, u16 y, const Plib::Realm* realm, unsigned range, F &&f )
    {
	  if ( realm == nullptr )
		return;
      CoordsArea coords(x, y, realm, range);
      _forEach( coords, realm, std::forward<F>( f ) );
    }
    template <class Filter>
	template <typename F>
    void WorldIterator<Filter>::InVisualRange( const UObject* obj, F &&f )
    {
      InRange( obj->toplevel_owner()->x, obj->toplevel_owner()->y, obj->toplevel_owner()->realm, RANGE_VISUAL, std::forward<F>( f ) );
    }
    template <class Filter>
	template <typename F>
    void WorldIterator<Filter>::InBox( u16 x1, u16 y1, u16 x2, u16 y2, const Plib::Realm* realm, F &&f )
    {
	  if ( realm == nullptr )
		return;
	  CoordsArea coords( x1, y1, x2, y2, realm );
      _forEach( coords, realm, std::forward<F>( f ) );
    }

	template <class Filter>
	template <typename F>
    void WorldIterator<Filter>::_forEach( const CoordsArea &coords,
                          const Plib::Realm* realm, F &&f )
    {
      for ( u16 wx = coords.wxL; wx <= coords.wxH; ++wx )
      {
        for ( u16 wy = coords.wyL; wy <= coords.wyH; ++wy )
        {
          Filter::call( realm->zone[wx][wy], coords, f );
        }
      }
    }

	// specializations of FilterImp

	template<>
	template <typename F>
    void FilterImp<FilterType::Mobile>::call( Core::Zone &zone, const CoordsArea &coords, F &&f )
    {
      for ( auto &chr : zone.characters )
      {
        if ( coords.inRange( chr ) )
          f( chr );
      }
      for ( auto &npc : zone.npcs )
      {
        if ( coords.inRange( npc ) )
          f( npc );
      }
    }

	template<>
	template <typename F>
	void FilterImp<FilterType::Player>::call( Core::Zone &zone, const CoordsArea &coords, F &&f )
    {
      for ( auto &chr : zone.characters )
      {
        if ( coords.inRange( chr ) )
          f( chr );
      }
    }

	template<>
	template <typename F>
    void FilterImp<FilterType::OnlinePlayer>::call( Core::Zone &zone, const CoordsArea &coords, F &&f )
    {
      for ( auto &chr : zone.characters )
      {
		if ( chr->has_active_client() )
		{
		  if ( coords.inRange( chr ) )
			f( chr );
		}
      }
    }

	template<>
	template <typename F>
    void FilterImp<FilterType::NPC>::call( Core::Zone &zone, const CoordsArea &coords, F &&f )
    {
      for ( auto &npc : zone.npcs )
      {
        if ( coords.inRange( npc ) )
          f( npc );
      }
    }

	template<>
	template <typename F>
    void FilterImp<FilterType::Item>::call( Core::Zone &zone, const CoordsArea &coords, F &&f )
    {
      for ( auto &item : zone.items )
      {
        if ( coords.inRange( item ) )
          f( item );
      }
    }

	template<>
	template <typename F>
    void FilterImp<FilterType::Multi>::call( Core::Zone &zone, const CoordsArea &coords, F &&f )
    {
      for ( auto &multi : zone.multis )
      {
        if ( coords.inRange( multi ) )
          f( multi );
      }
    }
  }
}
#endif
