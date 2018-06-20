#ifndef _EQUIPMENT_
#define _EQUIPMENT_

#include "CML_Reference.h"
#include "CML_EventMap.h"

CML_NAMESPACE_START()

class EquipmentError: public Error
{
public:
	static const EquipmentError NoParent;

protected:
	EquipmentError( uint16 id, const char *desc ): Error( id, desc ){}
};

class Equipment: public RefObj
{
	uint32 childRef_[CML_MAX_CHILD_PER_EQPT];
	uint8 childIdx_;
	uint32 parentRef_;
	uint8 eqptID_;
	//	Semaphore startSema_;
	Mutex mtx_;

public:
	Equipment(Equipment *parent = 0);
	virtual ~Equipment();
	//	virtual void run( void ) = 0;

	void attachEqpt(Equipment *eqpt, uint8 &id);
	void detachEqpt(Equipment *eqpt, uint8 id);

private:
	Equipment( const Equipment & );
	Equipment &operator=( const Equipment & );

};

CML_NAMESPACE_END()
#endif //_EQUIPMENT_

