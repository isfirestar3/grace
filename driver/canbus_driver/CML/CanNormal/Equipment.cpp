#include "CML.h"

CML_NAMESPACE_USE();

//CML_NEW_ERROR( EquipmentError, NoParent, "Does not hav a parent." );

Equipment::Equipment(Equipment *parent)
{
	parentRef_ = 0;
	childIdx_ = 0;
	eqptID_ = 0;

	cml.Debug( "Equipment %d init.\n", RefID() );

	for(int i=0; i<CML_MAX_CHILD_PER_EQPT; i++)
		childRef_[i] = 0;

	if(parent)
	{
		parent->attachEqpt(this, eqptID_);
		parentRef_ = parent->GrabRef();
	}

}

Equipment::~Equipment()
{
	cml.Debug( "Equipment %d uninit.\n", RefID() );
	KillRef();
	if( parentRef_ )
	{
		Equipment *eqpt = (Equipment*)RefObj::LockRef( parentRef_ );
		if( eqpt )
		{
			eqpt->detachEqpt( this, eqptID_ );
			eqpt->UnlockRef();
		}
		RefObj::ReleaseRef( parentRef_ );
		parentRef_ = 0;
	}
}

void Equipment::attachEqpt(Equipment *eqpt, uint8 &id)
{
	MutexLocker ml( mtx_ );
	for(int i=1; i<=CML_MAX_CHILD_PER_EQPT; i++)
		if(!childRef_[i-1])
		{
			cml.Debug( "Attach %d init.\n", eqpt->RefID() );
			id = i;
			childRef_[i-1] = eqpt->GrabRef();
			break;
		}
	return;
}

void Equipment::detachEqpt(Equipment *eqpt, uint8 id)
{
	MutexLocker ml( mtx_ );
	cml.Debug( "Detach %d init.\n", childRef_[id-1] );
	RefObj::ReleaseRef( childRef_[id-1] );
	childRef_[id-1] = 0;
	return;
}
