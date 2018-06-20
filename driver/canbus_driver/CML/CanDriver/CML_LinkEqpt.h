#ifndef _LINKEQUIPMENT_
#define _LINKEQUIPMENT_

#include "CML_Equipment.h"

CML_NAMESPACE_START()

enum LINKEQPT_EVENT
{
	/// Set when all amplifiers attached to this linkage have finished their moves and
	/// have settled in to position at the end of the move.  Cleared when a new move is
	/// started on any amplifier.
	LINKEQPT_MOVEDONE      = 0x00000001,

	/// Set when all amplifiers attached to the linkage have finished their moves, but
	/// have not yet settled into position at the end of the move.  Cleared when a new
	/// move is on any amplifier started.
	LINKEQPT_TRJDONE       = 0x00000002,

	/// A node guarding (or heartbeat) error has occurred.  This indicates that
	/// one of the amplifiers failed to respond within the expected amount of
	/// time for either a heartbeat or node guarding message.
	LINKEQPT_NODEGUARD     = 0x00000004,

	/// A latching fault has occurred on one of the amplifiers attached to this linkage.
	LINKEQPT_FAULT         = 0x00000010,

	/// A non-latching error has occurred on one of the amplifiers.
	LINKEQPT_ERROR         = 0x00000020,

	/// One of the the amplifiers is reporting a position warning event.
	LINKEQPT_POSWARN       = 0x00000040,

	/// One of the amplifiers is reporting a position window event.
	LINKEQPT_POSWIN        = 0x00000080,

	/// One of the amplifiers is reporting a velocity window event.
	LINKEQPT_VELWIN        = 0x00000100,

	/// One of the amplifiers is currently disabled.
	LINKEQPT_DISABLED      = 0x00000200,

	/// The positive limit switch of one or more amplifier is currently active
	LINKEQPT_POSLIM        = 0x00000400,

	/// The negative limit switch of one or more amplifier is currently active
	LINKEQPT_NEGLIM        = 0x00000800,

	/// The positive software limit of one or more amplifier is currently active
	LINKEQPT_SOFTLIM_POS   = 0x00001000,

	/// The negative software limit of one or more amplifier is currently active
	LINKEQPT_SOFTLIM_NEG   = 0x00002000,

	/// One of the linkage amplifiers is presently performing a quick stop sequence
	/// or is holding in quick stop mode.  The amplifier must be disabled to clear this.
	LINKEQPT_QUICKSTOP     = 0x00004000,

	/// One or more amplifier aborted the last profile without finishing.
	LINKEQPT_ABORT         = 0x00008000
};




class LinkEqpt: public Equipment
{
public:
	LinkEqpt(Equipment *parent = 0);
	virtual ~LinkEqpt();

	const Error *Init( uint16 ct, CiA402 a[] );
	const Error *Init( uint16 ct, CiA402 *a[] );

	CiA402 &GetCiA402( uint16 i );

private:
	LinkEqpt( const LinkEqpt & );
	LinkEqpt &operator=( const LinkEqpt & );

public:
	EventMap linkEventMap_;

private:
	uint16 dev402Cnt_;
	CiA402 *dev402_[ CML_MAX_402_PER_LINK ];
	friend class CiA402;

	class StateEvent: public Event
	{
	public:
		LinkEqpt *link;
		StateEvent( void ): Event(0){}
		bool isTrue( uint32 mask );
	}stateEvent_[ CML_MAX_402_PER_LINK ];
	friend class StateEvent;


};

CML_NAMESPACE_END()
#endif //_LINKEQUIPMENT_

