/*
History
=======
2005/07/05 Shinigami: added uoconvert.cfg option *StaticsPerBlock (hard limit is set to 10000)
2005/07/16 Shinigami: added uoconvert.cfg flag ShowIllegalGraphicWarning
2006/04/09 Shinigami: added uoconvert.cfg flag ShowRoofAndPlatformWarning

Notes
=======

*/

#ifndef UOFILE_H
#define UOFILE_H

#include "ustruct.h"

#include "clidata.h"
#include "uconst.h"

#include <vector>

namespace Pol {
#define MAX_STATICS_PER_BLOCK 10000
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

	extern signed char rawmapinfo( unsigned short x, unsigned short y, struct USTRUCT_MAPINFO* gi );
	void rawmapfullread();
	void getmapinfo( unsigned short x, unsigned short y, short* z, USTRUCT_MAPINFO* mi );
	void readtile( unsigned short tilenum, USTRUCT_TILE *tile );
	void readtile( unsigned short tilenum, USTRUCT_TILE_HSA *tile );
	void clear_tiledata();
	void readstaticblock( std::vector<USTRUCT_STATIC>* ppst, int* pnum, unsigned short x, unsigned short y );
	void rawstaticfullread();


	void read_objinfo( u16 graphic, struct USTRUCT_TILE& objinfo );
	void read_objinfo( u16 graphic, struct USTRUCT_TILE_HSA& objinfo );
	void readlandtile( unsigned short tilenum, USTRUCT_LAND_TILE* landtile );
	void readlandtile( unsigned short tilenum, USTRUCT_LAND_TILE_HSA* landtile );

	void open_uo_data_files( void );
	void read_uo_data( void );
	void readwater();

	extern int uo_mapid;
	extern int uo_usedif;
	extern unsigned short uo_map_width;
	extern unsigned short uo_map_height;

	extern int cfg_max_statics_per_block;
	extern int cfg_warning_statics_per_block;
	extern bool cfg_show_illegal_graphic_warning;
	extern bool cfg_show_roof_and_platform_warning;
	extern bool cfg_use_new_hsa_format;
  }
}
#endif
