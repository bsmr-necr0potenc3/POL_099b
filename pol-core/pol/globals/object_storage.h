#ifndef GLOBALS_OBJ_STORAGE_H
#define GLOBALS_OBJ_STORAGE_H


#include "../../clib/rawtypes.h"
#include "../objecthash.h"
#include "../poltype.h"

#include <boost/noncopyable.hpp>

#include <map>
#include <unordered_map>
#include <vector>

namespace Pol {
namespace Core {

  // if index is UINT_MAX, has been deleted
  typedef std::unordered_map<pol_serial_t, unsigned> SerialIndexMap;
  typedef std::multimap<pol_serial_t, UObject*> DeferList;

  class ObjectStorageManager : boost::noncopyable
  {
  public:
	  ObjectStorageManager();
	  ~ObjectStorageManager();

	  void deinitialize();

	  unsigned incremental_save_count;
	  unsigned current_incremental_save;
	  SerialIndexMap incremental_serial_index;
	  DeferList deferred_insertions;
	  std::vector< u32 > modified_serials;
	  std::vector< u32 > deleted_serials;
	  unsigned int clean_objects;
	  unsigned int dirty_objects;
	  bool incremental_saves_disabled;
	  
	  ObjectHash objecthash;

  };

  extern ObjectStorageManager objStorageManager;
}
}
#endif
