/*
History
=======

Notes
=======

*/

#ifndef BSCRIPT_BERROR_H
#define BSCRIPT_BERROR_H

#include "bstruct.h"

#include <istream>
#include <string>

namespace Pol {
  namespace Bscript {

	class BError : public BStruct
	{
	public:
	  BError();
	  explicit BError( const char *errortext );
	  explicit BError( const std::string& errortext );

	  static BObjectImp* unpack( std::istream& is );

	  static unsigned int creations();

	protected:
	  BError( const BError& i );
	  BError( std::istream& is, unsigned size );

	  virtual BObjectImp* copy() const POL_OVERRIDE;
	  virtual BObjectRef OperSubscript( const BObject& obj ) POL_OVERRIDE;
	  virtual BObjectImp* array_assign( BObjectImp* idx, BObjectImp* target, bool copy ) POL_OVERRIDE;

	  virtual char packtype() const POL_OVERRIDE;
	  virtual const char* typetag() const POL_OVERRIDE;
	  virtual const char* typeOf() const POL_OVERRIDE;
	  virtual int typeOfInt() const POL_OVERRIDE;

	  virtual bool isEqual( const BObjectImp& objimp ) const POL_OVERRIDE;
	  virtual bool isTrue() const POL_OVERRIDE;

	  ContIterator* createIterator( BObject* pIterVal );

	private:
	  static unsigned int creations_;
	};
  }
}
#endif
