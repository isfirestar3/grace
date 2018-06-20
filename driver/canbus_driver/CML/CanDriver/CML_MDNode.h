#ifndef _MDNODE_
#define _MDNODE_

#include "CML_Node.h"
#include "CML_Threads.h"
#include "CML_Error.h"
#include "CML_ErrorCodes.h"
CML_NAMESPACE_START()
class Device;


//enum PDO_EVENT
//{
//	PDO0				= 0x00000001,
//	PDO1                = 0x00000002,
//	PDO2                = 0x00000004,
//	PDO3                = 0x00000008
//};


enum MDNODE_EVENT
{

	MDNODE_SYNC			= 0x00000002,
	/// A node guarding (or heartbeat) error has occurred.  This indicates that
	/// the amplifier failed to respond within the expected amount of time for
	/// either a heartbeat or node guarding message.  This could be caused by
	/// a network wiring problem, amplifier power down, amp reset, etc.
	/// This bit is set when the error occurs, and is cleared by a call to the
	/// function Amp::ClearNodeGuardEvent.
	MDNODE_GUARDERR     = 0x00000004,
};

class NodeSettings
{
public:
	NodeSettings();
	uint32 synchPeriod;
	uint32 synchID;
	bool synchUseFirstNode;
	bool synchProducer;
	uint32 timeStampID;
	uint16 heartbeatPeriod;
	uint8 heartbeatTimeout;
	uint16 guardTime;
	uint8 lifeFactor;
	uint8 pdoCnt;
	bool enableOnInit;
	bool resetOnInit;
};

class MDNode: public Node
{
	/// This array keeps track of all device objects associated
	/// with this CANopen node
	uint32 devices_[8];

	Mutex mtx_;

	/// Count of the devices on this node.
	uint8 devCnt_;

	/// Profile number of the device.
	uint16 profileNum_[8];

	EventMap eventMap_;

public:
	MDNode( void )
	{
		SetRefName( "MDNode" );
		devCnt_ = 0;
		for( int i=0; i<8; i++ )
			devices_[i] = 0;
	}
	MDNode( Network &net, int16 nodeID ): Node(net,nodeID)
	{
		SetRefName( "MDNode" );
		devCnt_ = 0;
		for( int i=0; i<8; i++ )
			devices_[i] = 0;
		Init( net, nodeID );
	}
	MDNode( Network &net, int16 nodeID, NodeSettings &settings ): Node(net,nodeID)
	{
		SetRefName( "MDNode" );
		devCnt_ = 0;
		for( int i=0; i<8; i++ )
			devices_[i] = 0;
		Init( net, nodeID,  settings);
	}
	virtual ~MDNode()
	{
//		StopNode();
//		if(err) printf("Cannot stop node!\n");
	};

	virtual const Error *Init( Network &net, int16 nodeID );
	virtual const Error *Init( Network &net, int16 nodeID, NodeSettings &settings );

	virtual const Error *AttachDev( Device *dev );
	virtual const Error *DetachDev( Device *dev );

	virtual const uint8 GetDevCnt(){ return devCnt_; }
	virtual const uint16 GetDevProfile(uint8 devID){ return profileNum_[devID-1]; }

	virtual const Error *GetHardwareVersion(char *data);
	virtual const Error *GetSoftwareVersion(char *data);

	const Error *SaveConfig( void );

	virtual void HandleEmergency( CanFrame &frame );
	virtual void HandleStateChange( NodeState from, NodeState to );
	const Error *WaitNodeEvent( MDNODE_EVENT event, Timeout timeout=-1 );

private:
	/// Private copy constructor (not supported)
	MDNode( const MDNode& );

	/// Private assignment operator (not supported)
	MDNode& operator=( const MDNode& );
};

CML_NAMESPACE_END()
#endif //_MDNODE_
