#include "CML.h"
#include "stdio.h"
#include <iostream>
#include "log.h"
CML_NAMESPACE_USE();

// macro used for rounding floats
#define Round(x)  ((x>=0) ? (x+0.5) : (x-0.5))


//TPDO1
const Error *TPDO_PDO1::Init( Node *node, int slot, int canID )
{
   // Initialize the transmit PDO base class.
   // This needs to know the CANopen network and CAN message ID 
   // associated with the PDO.
   const Error *err = TPDO::Init( canID );

   // Set transmit type to 1.  This causes the PDO to be sent every 
   // sync period.  Setting the type to 2 would cause it to be sent every
   // second period, etc.
   if( !err ) err = SetType( 1 );

   // Init the two mapping objects to correspond to the 
   // data that will be mapped to this PDO
   if( !err ) err = actual_vel.Init( 0x606c, 0 );  // motor position
   if( !err ) err = target_vel.Init( 0x60FF, 0 );  // motor position

   // Add the mapped variables to the PDO
   if( !err ) err = AddVar( actual_vel );
   if( !err ) err = AddVar( target_vel );
  
   // Program this PDO in the node and enable it
   if( !err ) err = node->PdoSet( slot, *this );
   if (!err)
   {
	   printf("init pdo1 ok\n");
   }
   else  printf("init pdo1 fail!\n");

   return err;
}

/**
 * This function will be called by the high priority CANopen receive
 * thread when the PDO is received.  
 *
 * By the time this function is called, the data from the two mapped objects will
 * have been parsed from the input message.  It can be accessed by the Pmap objects
 * that we created.
 *
 * Keep in mind that this function is called from the same thread that receives all
 * CANopen messages.  Keep any processing here short and don't try to do any SDO access.
 * Often it's best to simply post a semaphore here and have another thread handle the data.
 *
 */
void TPDO_PDO1::Received( void )
{
	//printf( "-->TPDO_PDO1 received,XGyro:%4x,YGyro:%4x,ZGyro:%4x,XAcc:%4x\n",XGyro.Read(),
	//	YGyro.Read(),ZGyro.Read(),XAcc.Read());
}

CiA402::CiA402( void )
{
	SetRefName( "CiA402" );
}

CiA402::CiA402( MDNode &node, uint8 devID )
{
	SetRefName( "CiA402" );
	Init( node, devID );
}

CiA402::CiA402( MDNode &node, uint8 devID, CiA402Settings &settings )
{
	SetRefName( "CiA402" );
	Init( node, devID, settings );
}

CiA402::~CiA402()
{
	UnInit();
//	Disable( false );
}

const Error *CiA402::Init( MDNode &node, uint8 devID )
{
	CiA402Settings settings;
	return Init( node, devID, settings );
}

const Error *CiA402::Init( MDNode &node, uint8 devID, CiA402Settings &settings )
{
	const Error *err;
	err = Device::Init( node, devID );
	if( err ) return err;

	cml.Debug( "Device %d, GetDevProfile\n", devID );
	if(node.GetDevProfile(devID) != 402)
		return &DeviceError::WrongDevice;

	// If using programmable units, default to units
	// of encoder counts.
#ifdef CML_ENABLE_USER_UNITS
	SetFactorConfig(settings.factor);
#endif

	// I always start out with the following stopping modes:
	// quick stop - use the quick stop ramp
	// halt - use the profile deceleration
	cml.Debug( "Device %d, SetQuickStop\n", devID );
	err = SetQuickStop( CIA402QSTOP_QUICKSTOP );
	if( err ) return err;
	cml.Debug( "Device %d, SetHaltMode\n", devID );
	err = SetHaltMode( CIA402HALT_DECEL );
	if( err ) return err;

	statusPDO_.Init(*this);
	for(int i=0; i<3; i++)
	{
		dataTPDO_[i].Init(*this);
		dataRPDO_[i].Init(*this, i+1);
	}

	// If the amplifier should be disabled on startup,
	// clear the control word.  This ensures that the
	// amp won't be enabled when I start it.
	//Add @Seven  Elmo control should set power on first
	err = SetControlWord(0xFF);
	err = SetControlWord( 0x000E );
	if( err ) return err;
	cml.Debug( "Device %d, Disable\n", devID );
	err = Disable( false );
	if( err ) return err;

	cml.Debug( "Device %d, SetAmpMode\n", devID );
	err = SetOperationMode( settings.initialMode );
	if( err ) return err;

	// Clear the latched version of the amplifier's
	// event status register.  We use this register
	// to check for unexpected amplifier resets.
//	err = ClearEventLatch( (CIA402_EVENT_STATUS)0xFFFFFFFF );
//	if( err ) return err;

	// Clear any latched fault conditions
	//@Seven Elmo clear falut fail
	/*cml.Debug( "Device %d, ClearFaults\n", devID );
	err = ClearFaults();
	if( err ) return err;*/

	// Start the node at the end of the last dev's initialization.
	if (devID == node.GetDevCnt())
		err = node.StartNode();
	tpdo1.Init(&node,1,0x0000480+node.GetNodeID());
	return err;
}

const Error *CiA402::UnInit( void )
{
	const Error *err = 0;
	//if(!err) err = 
	Disable( false );
	//if(!err) err = Device::UnInit();
	if(!err) err = Device::UnInit();
	return err;
}

//const Error *CiA402::StartStatusPDO(uint16 slot)
//{
//	return statusPDO_.Init(*this, slot);
//}
//
//const Error *CiA402::StopStatusPDO()
//{
//	return statusPDO_.UnInit();
//}

const Error *CiA402::WaitPDOEvent( PDO_EVENT event, Timeout timeout )
{
	EventAny any( (uint32)event );
	return any.Wait( pdoMap_, timeout );
}

const Error *CiA402::WaitEvent( Event &e, Timeout timeout )
{
	CIA402_EVENT match;
	return WaitEvent( e, timeout, match );
}

const Error *CiA402::WaitEvent( Event &e, Timeout timeout, CIA402_EVENT &match )
{
	const Error *err = e.Wait( cia402Event_, timeout );
	match = (CIA402_EVENT)e.getMask();
	return err;
}

//const Error *CiA402::SetCountsPerUnit( uunit cts )
//{
//#ifdef CML_ENABLE_USER_UNITS
////	u2lPos = cts;         l2uPos = 1.0/u2lPos;
////	u2lVel = cts*10.0;    l2uVel = 1.0/u2lVel;
////	u2lAcc = cts*0.1;     l2uAcc = 1.0/u2lAcc;
////	u2lJrk = cts*0.01;    l2uJrk = 1.0/u2lJrk;
////
////	u2mPos = u2lPos;  m2uPos = l2uPos;
////	u2mVel = u2lVel;  m2uVel = l2uVel;
////	u2mAcc = u2lAcc;  m2uAcc = l2uAcc;
//	return 0;
//#else
//	return &CiA402Error::NoUserUnits;
//#endif
//}

void CiA402::GetFactorConfig( FactorCfg &cfg )
{
	GetPosFctNum(cfg.posFctNum);
	GetPosFctDiv(cfg.posFctDiv);
	GetVelFctNum(cfg.velFctNum);
	GetVelFctDiv(cfg.velFctDiv);
	GetAccFctNum(cfg.accFctNum);
	GetAccFctDiv(cfg.accFctDiv);
	GetPolarity(cfg.polarity);
}

void CiA402::SetFactorConfig( FactorCfg &cfg )
{
	SetPosFctNum(cfg.posFctNum);
	SetPosFctDiv(cfg.posFctDiv);
	SetVelFctNum(cfg.velFctNum);
	SetVelFctDiv(cfg.velFctDiv);
	SetAccFctNum(cfg.accFctNum);
	SetAccFctDiv(cfg.accFctDiv);
	SetPolarity(cfg.polarity);
}

void CiA402::SetPosFctNum( uint32 num )
{
	factor_.posFctNum = num;
}

void CiA402::GetPosFctNum( uint32 &num )
{
	num =  factor_.posFctNum;
}

void CiA402::SetPosFctDiv( uint32 div )
{
	factor_.posFctDiv = div;
}

void CiA402::GetPosFctDiv( uint32 &div )
{
	div = factor_.posFctDiv;
}

void CiA402::SetVelFctNum( uint32 num )
{
	factor_.velFctNum = num;
}

void CiA402::GetVelFctNum( uint32 &num )
{
	num = factor_.velFctNum;
}

void CiA402::SetVelFctDiv( uint32 div )
{
	factor_.velFctDiv = div;
}

void CiA402::GetVelFctDiv( uint32 &div )
{
	div = factor_.velFctDiv;
}

void CiA402::SetAccFctNum( uint32 num )
{
	factor_.accFctNum = num;
}

void CiA402::GetAccFctNum( uint32 &num )
{
	num = factor_.accFctNum;
}

void CiA402::SetAccFctDiv( uint32 div )
{
	factor_.accFctDiv = div;
}

void CiA402::GetAccFctDiv( uint32 &div )
{
	div = factor_.accFctDiv;
}

void CiA402::SetPosPolarity( bool val )
{
	if(val)
		factor_.polarity &= 0x7f;
	else
		factor_.polarity |=0x80;
}

bool CiA402::GetPosPolarity( void )
{
	if(factor_.polarity & 0x80)
		return false;
	else
		return true;
}

void CiA402::SetVelPolarity( bool val )
{
	if(val)
		factor_.polarity &= 0xBf;
	else
		factor_.polarity |=0x40;
}

bool CiA402::GetVelPolarity( void )
{
	if(factor_.polarity & 0x40)
		return false;
	else
		return true;
}

void CiA402::SetPolarity( uint8 val )
{
	factor_.polarity = val;
}

void CiA402::GetPolarity( uint8 &val )
{
	val = factor_.polarity;
}

int32 CiA402::RevPosition( int32 pos )
{
	return (factor_.polarity & 0x80) ? (-pos) : (pos);
}
int32 CiA402::RevVelocity( int32 vel )
{
	return (factor_.polarity & 0x40) ? (-vel) : (vel);
}

int32 CiA402::PosUser2Intl( int32 pos )
{
#ifdef CML_ENABLE_USER_UNITS
	uunit p = (uunit)pos * factor_.posFctNum / factor_.posFctDiv;
	return (int32)Round(p);
#else
	return pos;
#endif
}

int32 CiA402::VelUser2Intl( int32 vel )
{
#ifdef CML_ENABLE_USER_UNITS
	uunit v = (uunit)vel * factor_.velFctNum / factor_.velFctDiv;
	return (int32)Round(v);
#else
	return vel;
#endif
}

int32 CiA402::AccUser2Intl( int32 acc )
{
#ifdef CML_ENABLE_USER_UNITS
	uunit a = (uunit)acc * factor_.accFctNum / factor_.accFctDiv;
	return (int32)Round(a);
#else
	return acc;
#endif
}

int32 CiA402::PosIntl2User( int32 pos )
{
#ifdef CML_ENABLE_USER_UNITS
	uunit p = (uunit)pos * factor_.posFctDiv / factor_.posFctNum;
	return (int32)Round(p);
#else
	return pos;
#endif
}

int32 CiA402::VelIntl2User( int32 vel )
{
#ifdef CML_ENABLE_USER_UNITS
	uunit v = (uunit)vel * factor_.velFctDiv / factor_.velFctNum;
	return (int32)Round(v);
#else
	return vel;
#endif
}

int32 CiA402::AccIntl2User( int32 acc )
{
#ifdef CML_ENABLE_USER_UNITS
	uunit a = (uunit)acc * factor_.accFctDiv / factor_.accFctNum;
	return (int32)Round(a);
#else
	return acc;
#endif
}

const Error *CiA402::SetQuickStop( CIA402_QUICK_STOP_MODE mode )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	return node->sdo.Dnld16( CIA402_QSTOP_MODE + offset, 0, (int16)mode );
}

const Error *CiA402::SetHaltMode( CIA402_H_MODE mode )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	return node->sdo.Dnld16( CIA402_HALT_MODE + offset, 0, (int16)mode );
}

const Error *CiA402::Disable( bool wait )
{
	const Error *err = SetControlWord( 0x0005 );
	return err;

	if( !wait ) return err;

	// Now, wait for the amp to actually disable before returning
	// Note that when this returns the amplifier is trying to
	// disable, but if the motor's brake times are set, then this
	// could take a long time.
	EventAny e( AMPEVENT_DISABLED  );
	return WaitEvent( e, 500 );
}

const Error *CiA402::Enable( bool wait )
{
	const Error *err = SetControlWord( 0x0006 );
	if (err)
	{
		return err;
	}
	err = SetControlWord( 0x0007 );
	if (err)
	{
		return err;
	}
	err = SetControlWord( 0x000F );
	if (err)
	{
		return err;
	}
	//if(!wait)
	//	return err;

	// Now, wait for the amp to actually enable before returning
	//EventNone e( CIA402EVENT_DISABLED );
	//if(!err) err = WaitEvent( e, 500 );

	//if( err == &ThreadError::Timeout )
	//{
	//	if( cia402Event_.getMask() & CIA402EVENT_FAULT )
	//	{
	//		return &CiA402Error::Fault;
	//	}
	//}
	return err;
}

const Error *CiA402::SetControlWord( uint16 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	return node->sdo.Dnld16( CIA402_CONTROL + offset, 0, value );
}

const Error *CiA402::GetPdoEventMask( PDO_EVENT &e )
{
	e = (PDO_EVENT)pdoMap_.getMask();
	return 0;
}

const Error *CiA402::GetEventMask( CIA402_EVENT &e )
{
	e = (CIA402_EVENT)cia402Event_.getMask();
	return 0;
}

const Error *CiA402::GetErrorStatus( bool noComm )
{
	uint32 events = cia402Event_.getMask();
//	EVENT_STATUS stat = (EVENT_STATUS)statPdo.estat.Read();
	const Error *err = 0;

	if( events & CIA402EVENT_NODEGUARD )
		err = &NodeError::GuardTimeout;

	else if( events & CIA402EVENT_FAULT )
	{
		uint32 fault = 0;
		for(int i=0; i<8; i++)
		{
			GetFault( fault, i );
			if(fault)
				break;
		}
		err = CiA402Fault::DecodeFault( fault );
	}

	else if( events & CIA402EVENT_QUICKSTOP )
		err = &CiA402Error::QuickStopMode;

	else if( events & CIA402EVENT_ABORT )
		err = &CiA402Error::Abort;

	else if( events & CIA402EVENT_DISABLED )
		err = &CiA402Error::Disabled;

	return err;
}

const Error *CiA402::GetErrorCode(int32 &error)
{
	int16 offset = (int16)(0x800 * (GetDeviceID() - 1));
	RefObjLocker<MDNode> node(GetNodeRef());
	if (!node) return &DeviceError::NodeUnavailable;

	int16 value;
	const Error *err = node->sdo.Upld16(0x603f + offset, 0, value);
	error = value;
	return err;
}
const Error *CiA402::SetOperationMode( int8 mode )
{

	const Error *err = 0;
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
#if 0
	//This step makes sure the driver is working in CANopen mode.
	if(mode == CIA402MODE_PROG_VEL)
	{
		err = node->sdo.Dnld16(  0x2300 + offset, 0, (int16)11 );
	}
	else
	{
		err = node->sdo.Dnld16(  0x2300 + offset, 0, (int16)30 );
		if(!err) err = node->sdo.Dnld8( CIA402_OP_MODE + offset, 0, (int8)mode );
//		if(!err) err = node->sdo.Dnld16( 0x6086 + offset, 0, (int16)0 );
//		if(!err) err = node->sdo.Dnld32( 0x6083 + offset, 0, (int32)100000 );
//		if(!err) err = node->sdo.Dnld32( 0x6084 + offset, 0, (int32)100000 );
//		if(!err) err = node->sdo.Dnld32( 0x2121 + offset, 0, (int32)1000 );
	}
#endif
	//This step makes sure the driver is working in CANopen mode.
	//Add @Seven Elmo should not set operation mode
	NodeIdentity id;
//	cml.Debug( "Amp %d, checking ID\n", nodeID );
	err = node->GetIdentity( id );
	if( err ) return err;

// 	if( id.vendorID==0x9A)
// 	{
// 		//设置为PV mode
// 		err = node->sdo.Dnld8(  0x6060, 0, (int8)3 );
// 	}
// 	else{
// 		if(mode == CIA402MODE_PROG_VEL)
// 		{
// 			err = node->sdo.Dnld16(  0x2300 + offset, 0, (int16)11 );
// 		}
// 		else
// 		{
// 			err = node->sdo.Dnld16(  0x2300 + offset, 0, (int16)30 );
// 			if(!err) err = node->sdo.Dnld8( CIA402_OP_MODE + offset, 0, (int8)mode );
// 	//		if(!err) err = node->sdo.Dnld16( 0x6086 + offset, 0, (int16)0 );
// 	//		if(!err) err = node->sdo.Dnld32( 0x6083 + offset, 0, (int32)100000 );
// 	//		if(!err) err = node->sdo.Dnld32( 0x6084 + offset, 0, (int32)100000 );
// 	//		if(!err) err = node->sdo.Dnld32( 0x2121 + offset, 0, (int32)1000 );
// 		}
// 	}

    //err = node->sdo.Dnld16(  0x2300 + offset, 0, (int16)30 );
    //if(!err) 
        err = node->sdo.Dnld8( CIA402_OP_MODE + offset, 0, (int8)mode );
	return err;
}

const Error *CiA402::ClearEventLatch( CIA402_EVENT_STATUS stat )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	return node->sdo.Dnld32(  CIA402_EVENT_STAT_LATCH + offset, 0, (uint32)stat );
}

const Error *CiA402::ClearFaults( void )
{
	return SetControlWord( 0x0080 );
}

const Error *CiA402::ClearErrHistory()
{
//	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
//	if(offset == 0)
	return node->sdo.Dnld8( CIA402_CAN_ERR_HISTORY, 0, (CML::uint8)0 );
//	else
//		return node->sdo.Dnld8( 0x2003 + offset, 0, (CML::uint8)0 );
}

const Error *CiA402::GetFaultsCnt( uint8 &value )
{
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	return node->sdo.Upld8( CIA402_CAN_ERR_HISTORY, 0, value );
}

const Error *CiA402::GetFault( uint32 &value, uint8 idx )
{
	const Error *err = 0;
	uint32 v;
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	err = node->sdo.Upld32( CIA402_CAN_ERR_HISTORY, idx+1, v );
	if(!v)
		value = 0;
	else
		if(((v>>16)&0x000f) == (GetDeviceID() - 1))
			value = v & 0x0000ffff;
		else
			value = 0;
	return err;
}

const Error *CiA402::GetVelLoopConfig( VelLoopCfg &cfg )
{
	const Error *err = GetVelLoopP(cfg.kp );
	if( !err ) err = GetVelLoopI( cfg.ki );
	if( !err ) err = GetProfileAcc( cfg.maxAcc );
	if( !err ) err = GetProfileDec( cfg.maxDec );
	if( !err ) err = GetVelLoopMax( cfg.maxVel );
	return err;
}

const Error *CiA402::SetVelLoopConfig( VelLoopCfg &cfg )
{
	const Error *err = SetVelLoopP( cfg.kp );
	if( !err ) err = SetVelLoopI( cfg.ki );
	if( !err ) err = SetProfileAcc( cfg.maxAcc );
	if( !err ) err = SetProfileDec( cfg.maxDec );
	if( !err ) err = SetVelLoopMax( cfg.maxVel );
	return err;
}


const Error *CiA402::GetCrntLoopConfig( CrntLoopCfg &cfg )
{
	const Error *err =GetCrntLoopP( cfg.kp );
	if( !err ) err = GetCrntLoopI( cfg.ki );
	if( !err ) err = GetIitPeak( cfg.peakLim );
	if( !err ) err = GetIitCont( cfg.contLim );
	if( !err ) err = GetIitTime( cfg.peakTime );
	return err;
}

const Error *CiA402::SetCrntLoopConfig( CrntLoopCfg &cfg )
{
	const Error *err = SetCrntLoopP( cfg.kp );
	if( !err ) err = SetCrntLoopI( cfg.ki );
	if( !err ) err = SetIitPeak( cfg.peakLim );
	if( !err ) err = SetIitCont( cfg.contLim );
	if( !err ) err = SetIitTime( cfg.peakTime );
	return err;
}

const Error *CiA402::GetVelLoopP( uint16 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Upld16( CIA402_VELLOOP+offset, 1, value );
}

const Error *CiA402::SetVelLoopP( uint16 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Dnld16( CIA402_VELLOOP+offset, 1, value );
}

const Error *CiA402::GetVelLoopI( uint16 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Upld16( CIA402_VELLOOP+offset, 2, value );
}

const Error *CiA402::SetVelLoopI( uint16 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Dnld16( CIA402_VELLOOP+offset, 2, value );
}

const Error *CiA402::GetVelLoopMax( int32 &value )
{
	int32 v;
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	const Error *err = node->sdo.Upld32( CIA402_VELLIM_MAXVEL+offset, 0, v );
	value = VelIntl2User( v );
	return err;
}

const Error *CiA402::SetVelLoopMax( int32 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Dnld32( CIA402_VELLIM_MAXVEL+offset, 0, VelUser2Intl(value) );
}

const Error *CiA402::SetProfileAcc( int32 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Dnld32( CIA402_PROFILE_ACC+offset, 0, AccUser2Intl(value) );
}

const Error *CiA402::GetProfileAcc( int32 &value )
{
	int32 v;
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	const Error *err = node->sdo.Upld32(CIA402_PROFILE_ACC+ offset, 0, v);
	value = AccIntl2User( v );
	return err;
}

const Error *CiA402::SetProfileDec( int32 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Dnld32(CIA402_PROFILE_DCC + offset, 0, AccUser2Intl(value));
}

const Error *CiA402::GetProfileDec( int32 &value )
{
	int32 v;
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	const Error *err = node->sdo.Upld32(CIA402_PROFILE_DCC + offset, 0, v);
	value = AccIntl2User( v );
	return err;
}

const Error *CiA402::GetCrntLoopP( uint16 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Upld16( CIA402_CRNTLOOP+offset, 1, value );
}

const Error *CiA402::SetCrntLoopP( uint16 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Dnld16( CIA402_CRNTLOOP+offset, 1, value );
}

const Error *CiA402::GetCrntLoopI( uint16 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Upld16( CIA402_CRNTLOOP+offset, 2, value );
}

const Error *CiA402::SetCrntLoopI( uint16 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Dnld16( CIA402_CRNTLOOP+offset, 2, value );
}

const Error *CiA402::SetIitPeak( int16 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Dnld16( CIA402_CRNTLIM_PEAK+offset, 0, value );
}

const Error *CiA402::GetIitPeak( int16 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Upld16( CIA402_CRNTLIM_PEAK+offset, 0, value );
}

const Error *CiA402::SetIitCont( int16 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Dnld16( CIA402_CRNTLIM_CONT+offset, 0, value );
}

const Error *CiA402::GetIitCont( int16 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Upld16( CIA402_CRNTLIM_CONT+offset, 0, value );
}

const Error *CiA402::SetIitTime( uint16 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Dnld16( CIA402_CRNTLIM_TIME+offset, 0, value );
}

const Error *CiA402::GetIitTime( uint16 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Upld16( CIA402_CRNTLIM_TIME+offset, 0, value );
}

const Error *CiA402::SetProfileType( CIA402_6086_CODE type )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Dnld16( CIA402_PROFILE_TYPE+offset, 0, (int16)type );
}

const Error *CiA402::GetProfileType( CIA402_6086_CODE &type )
{
	int16 value;
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	const Error *err = node->sdo.Upld16( CIA402_PROFILE_TYPE+offset, 0, value );
	type = (CIA402_6086_CODE)value;
	return err;
}

const Error *CiA402::GoHome( void )
{
	cml.Debug( "Driver %d Home\n", GetDeviceID() );
	const Error *err = 0;

	// Make sure we are in homing mode
	if(!err) err = SetOperationMode( CIA402MODE_CAN_HOMING );
	if(!err) err = Enable();

	// Start homing
	if(!err) err = SetControlWord( 0x001F );

	// Wait for the move to start if the home method isn't NONE.
	// We wait with a short timeout and don't return an error if this
	// does time out.  The reason is that some homing types can complete
	// immediately without ever starting a move, or occasionally the move
	// can be so quick that we never see it.  In those cases this is more
	// of a short delay to ensure the move gets a chance to start before
	// we return.
	CIA402_6098_CODE method;
	if(!err) err = GetHomeMethod( method );
	if( method != HM_NONE )
	{
		EventNone e( CIA402EVENT_MOVEDONE );
		err = WaitEvent( e, 10 );
	}

	// Otherwise, wait for the 'trajectory aborted' bit to be
	// clear if it was set.
	else
	{
		EventNone e( CIA402EVENT_ABORT );
		err = WaitEvent( e, 200 );
	}

	if( err )
		cml.Warn( "Driver %d Home failed: %s\n", GetDeviceID(), err->toString() );

	return err;
}

const Error *CiA402::GoHome( HomeCfg &cfg )
{
	/**************************************************
	 * If a move is necessary, make sure that the accel
	 * and velocity parameters are non-zero.
	 **************************************************/
	const Error *err = 0;

	if( (cfg.method == HM_NONE) && (cfg.offset == 0) )

	// Set the home parameters
//	if( !err ) err = SetHomeVelFast( cfg.velFast );
//	if( !err ) err = SetHomeVelSlow( cfg.velSlow );
//	if( !err ) err = SetHomeAccel( cfg.accel );

	if( !err ) err = SetHomeOffset( cfg.offset );
	if( !err ) err = SetHomeMethod( cfg.method );

	// Start the home.
	if( !err ) err = GoHome();
	return err;
}

const Error *CiA402::SetHomeConfig( HomeCfg &cfg )
{
	const Error *err = SetHomeMethod( cfg.method );
	if( !err ) err = SetHomeOffset( cfg.offset );
	return err;
}

const Error *CiA402::GetHomeConfig( HomeCfg &cfg )
{
	const Error *err = GetHomeMethod( cfg.method );
	if( !err ) err = GetHomeOffset( cfg.offset );
	return err;
}

const Error *CiA402::SetHomeMethod( CIA402_6098_CODE method )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	return node->sdo.Dnld8( CIA402_HOME_METHOD+offset, 0, (int8)method );
}

const Error *CiA402::GetHomeMethod( CIA402_6098_CODE &method )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	int8 m;
	const Error *err =  node->sdo.Upld8( CIA402_HOME_METHOD+offset, 0, m );
	method = (CIA402_6098_CODE)m;
	return err;
}

const Error *CiA402::SetHomeOffset( int32 value  )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	return node->sdo.Dnld32( CIA402_HOME_OFFSET+offset, 0, PosUser2Intl(value) );
}

const Error *CiA402::GetHomeOffset( int32 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	int32 p;
	const Error *err =  node->sdo.Upld32( CIA402_HOME_OFFSET+offset, 0, p );
	value = PosIntl2User(p);
	return err;
}

const Error *CiA402::GetCiA402Config( CiA402Cfg &cfg )
{
	const Error *err = GetIoConfig(cfg.io);
//	if( !err ) err = GetMtrInfo(cfg.motor);
	if( !err ) err = GetTrackingWindows(cfg.window);
	if( !err ) err = GetVelLoopConfig(cfg.vLoop);
	if( !err ) err = GetCrntLoopConfig(cfg.cLoop);
	return err;
}

const Error *CiA402::SetCiA402Config( CiA402Cfg &cfg )
{
	const Error *err = SetIoConfig(cfg.io);
//	if( !err ) err = SetMtrInfo(cfg.motor);
	if( !err ) err = SetTrackingWindows(cfg.window);
	if( !err ) err = SetVelLoopConfig(cfg.vLoop);
	if( !err ) err = SetCrntLoopConfig(cfg.cLoop);
	return err;
}

#if 0
const Error *CiA402::GetMtrInfo( MtrCfg &cfg )
{

	int32 l;
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	// Upload those parameters that can be mapped directly into the
	// MtrInfo structure with no translation.
	//	const Error *err = sdo.UpldString( OBJID_MOTOR_MODEL, 0, l=COPLEY_MAX_STRING, cfg.model );
	//	if( !err ) err = sdo.UpldString( OBJID_MOTOR_MFG,   0, l=COPLEY_MAX_STRING, cfg.mfgName );
	const Error *err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset,  1, cfg.type );
	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset,  2, cfg.poles );
	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset,  4, cfg.hallType );
	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset,  5, cfg.hallWiring );
	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset,  6, cfg.hallOffset );
	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset, 17, cfg.stopTime   );
	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset, 18, cfg.brakeDelay );
	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset, 20, cfg.encType );
	//	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset, 21, cfg.encUnits );
	if( !err ) err = node->sdo.Upld32( CIA402_MOTOR_INFO+offset, 23, cfg.ctsPerRev );
	//	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset, 24, cfg.encRes );
	//	if( !err ) err = node->sdo.Upld32( CIA402_MOTOR_INFO+offset, 25, cfg.eleDist );
	//	if( !err ) err = node->sdo.Upld32( CIA402_MOTOR_INFO+offset, 26, cfg.ndxDist );
	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset, 27, cfg.mtrUnits );
	//	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset, 28, cfg.encShift );
	//	if( !err ) err = node->sdo.Upld32( CIA402_MOTOR_INFO+offset, 29, cfg.stepsPerRev );

	// Upload the rest of the parameters to temporary locations
	// and convert them into the units used in the MtrCfg structure.
	int16 wiring, tSense, brake, encDir;
	int32 maxVel, bVel;

	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset,  3, wiring   );
	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset,  7, cfg.resistance );
	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset,  8, cfg.inductance );

	if( !err ) err = node->sdo.Upld32( CIA402_MOTOR_INFO+offset,  9, cfg.inertia );
	if( !err ) err = node->sdo.Upld32( CIA402_MOTOR_INFO+offset, 10, cfg.backEMF );
	if( !err ) err = node->sdo.Upld32( CIA402_MOTOR_INFO+offset, 11, cfg.velMax   );
	if( !err ) err = node->sdo.Upld32( CIA402_MOTOR_INFO+offset, 12, cfg.trqConst );
	if( !err ) err = node->sdo.Upld32( CIA402_MOTOR_INFO+offset, 13, cfg.trqPeak  );
	if( !err ) err = node->sdo.Upld32( CIA402_MOTOR_INFO+offset, 14, cfg.trqCont  );

	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset, 15, tSense   );
	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset, 16, brake    );

	if( !err ) err = node->sdo.Upld32( CIA402_MOTOR_INFO+offset, 19, bVel     );
	if( !err ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset, 22, encDir   );

	if( err ) return err;

	// Now, convert the uploaded values to more convenient units.
	cfg.mtrReverse = wiring ? true : false;
	cfg.encReverse = encDir ? true : false;
	cfg.hasBrake   = brake ? false : true;
	cfg.tempSensor = tSense ? true : false;

	//	cfg.velMax     = VelMtr2User( maxVel );
	cfg.brakeVel   = VelMtr2User( bVel );


	// Check for some parameters that we added in later versions of firmware.
	uint8 ct;
	err = node->sdo.Upld8( CIA402_MOTOR_INFO+offset,  0, ct );
	if( !err && (ct >=30) ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset, 30, cfg.loadEncType );
	if( !err && (ct >=31) ) err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset, 31, encDir );
	if( !err && (ct >=32) ) err = node->sdo.Upld32( CIA402_MOTOR_INFO+offset, 32, cfg.loadEncRes  );
	cfg.loadEncReverse = encDir ? true : false;

	if( !err && (ct >=33) && CheckFeature( FEATURE_GEAR_RATIO ) )
		err = node->sdo.Upld32( CIA402_MOTOR_INFO+offset, 33, cfg.gearRatio );

	if( !err && (ct >=34) && CheckFeature( FEATURE_RESOLVER_CYCLES ) )
		err = node->sdo.Upld16( CIA402_MOTOR_INFO+offset, 34, cfg.resolverCycles );

	if( (int16)cfg.resolverCycles <= 0 )
		cfg.resolverCycles =1;

	if( !err && CheckFeature( FEATURE_HALL_VEL_SHIFT ) )
		err = node->sdo.Upld16( OBJID_HALLVEL_SHIFT, 0, cfg.hallVelShift );

	if( !err && CheckFeature( FEATURE_ENC_OPTIONS ) )
		err = node->sdo.Upld32( OBJID_MTR_ENC_OPTIONS, 0, cfg.mtrEncOptions );

	if( !err && CheckFeature( FEATURE_ENC_OPTIONS ) )
		err = node->sdo.Upld32( OBJID_LOAD_ENC_OPTIONS, 0, cfg.loadEncOptions );


	return err;

}

const Error *CiA402::SetMtrInfo( MtrCfg &cfg )
{
	// Translate parameters into the proper units for upload.
	int16 wiring, tSense, brake, encDir;
	int32 maxVel, bVel;
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	//	maxVel  = VelUser2Mtr( cfg.velMax );
	bVel    = VelUser2Mtr( cfg.brakeVel );

	wiring = (cfg.mtrReverse == true);
	encDir = (cfg.encReverse == true);
	brake  = (cfg.hasBrake   == false);
	tSense = (cfg.tempSensor == true);

	// Upload these parameters to the motor
	//	const Error *err = node->sdo.DnldString( OBJID_MOTOR_MODEL, 0, cfg.model   );
	//	if( !err ) err = node->sdo.DnldString( OBJID_MOTOR_MFG,   0, cfg.mfgName );
	const Error *err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset,  1, cfg.type );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset,  2, cfg.poles );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset,  3, wiring   );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset,  4, cfg.hallType );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset,  5, cfg.hallWiring );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset,  6, cfg.hallOffset );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset,  7, cfg.resistance );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset,  8, cfg.inductance );
	if( !err ) err = node->sdo.Dnld32( CIA402_MOTOR_INFO+offset,  9, cfg.inertia  );
	if( !err ) err = node->sdo.Dnld32( CIA402_MOTOR_INFO+offset, 10, cfg.backEMF );
	if( !err ) err = node->sdo.Dnld32( CIA402_MOTOR_INFO+offset, 11, cfg.velMax   );
	if( !err ) err = node->sdo.Dnld32( CIA402_MOTOR_INFO+offset, 12, cfg.trqConst );
	if( !err ) err = node->sdo.Dnld32( CIA402_MOTOR_INFO+offset, 13, cfg.trqPeak  );
	if( !err ) err = node->sdo.Dnld32( CIA402_MOTOR_INFO+offset, 14, cfg.trqCont  );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset, 15, tSense   );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset, 16, brake    );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset, 17, cfg.stopTime   );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset, 18, cfg.brakeDelay );
	if( !err ) err = node->sdo.Dnld32( CIA402_MOTOR_INFO+offset, 19, bVel     );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset, 20, cfg.encType );
	//	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset, 21, cfg.encUnits );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset, 22, encDir   );
	if( !err ) err = node->sdo.Dnld32( CIA402_MOTOR_INFO+offset, 23, cfg.ctsPerRev );
	//	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset, 24, cfg.encRes );
	//	if( !err ) err = node->sdo.Dnld32( CIA402_MOTOR_INFO+offset, 25, cfg.eleDist );
	//	if( !err ) err = node->sdo.Dnld32( CIA402_MOTOR_INFO+offset, 26, cfg.ndxDist );
	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset, 27, cfg.mtrUnits );
	//	if( !err ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset, 28, cfg.encShift );
	//	if( !err ) err = node->sdo.Dnld32( CIA402_MOTOR_INFO+offset, 29, cfg.stepsPerRev );
	if( err ) return err;


	// Update the load encoder settings if available in the firmware
	uint8 ct;
	err = node->sdo.Upld8( CIA402_MOTOR_INFO+offset,  0, ct );

	encDir = (cfg.loadEncReverse == true);
	if( !err && (ct >= 30) ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset, 30, cfg.loadEncType );
	if( !err && (ct >= 31) ) err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset, 31, encDir );
	if( !err && (ct >= 32) ) err = node->sdo.Dnld32( CIA402_MOTOR_INFO+offset, 32, cfg.loadEncRes  );

	if( !err && (ct >=33) && CheckFeature( FEATURE_GEAR_RATIO ) )
		err = node->sdo.Dnld32( CIA402_MOTOR_INFO+offset, 33, cfg.gearRatio );

	if( !err && (ct >=34) && CheckFeature( FEATURE_RESOLVER_CYCLES ) )
		err = node->sdo.Dnld16( CIA402_MOTOR_INFO+offset, 34, cfg.resolverCycles );

	if( !err && CheckFeature( FEATURE_HALL_VEL_SHIFT ) )
		err = node->sdo.Dnld16( OBJID_HALLVEL_SHIFT, 0, cfg.hallVelShift );

	if( !err && CheckFeature( FEATURE_ENC_OPTIONS ) )
		err = node->sdo.Dnld32( OBJID_MTR_ENC_OPTIONS, 0, cfg.mtrEncOptions );

	if( !err && CheckFeature( FEATURE_ENC_OPTIONS ) )
		err = node->sdo.Dnld32( OBJID_LOAD_ENC_OPTIONS, 0, cfg.loadEncOptions );

	return err;
}
#endif


const Error *CiA402::SetTrackingWindows( TrackWinCfg &cfg )
{
//	const Error *err = SetPositionErrorWindow( cfg.trackErr );
//	if( !err ) err = SetPositionWarnWindow( cfg.trackWarn );
//	if( !err ) err = SetSettlingWindow( cfg.settlingWin );
//	if( !err ) err = SetSettlingTime( cfg.settlingTime );
	const Error *err = SetVelWindow( cfg.velWarnWin );
	if( !err ) err = SetVelWinTime( cfg.velWarnTime );
	return err;
}

const Error *CiA402::GetTrackingWindows( TrackWinCfg &cfg )
{
//	const Error *err = GetPositionErrorWindow( cfg.trackErr );
//	if( !err ) err = GetPositionWarnWindow( cfg.trackWarn );
//	if( !err ) err = GetSettlingWindow( cfg.settlingWin );
//	if( !err ) err = GetSettlingTime( cfg.settlingTime );
	const Error *err = GetVelWindow( cfg.velWarnWin );
	if( !err ) err = GetVelWinTime( cfg.velWarnTime );
	return err;
}

const Error *CiA402::SetVelWindow( uint16 value )
{
	int32 v = VelUser2Intl(value);
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

//	if(v>65535) v = 65535;
//	if(v<0) v = 0;
//	return node->sdo.Dnld32( CIA402_VELWARN_WIN+offset, 0, VelUser2Mtr(value) );
	return node->sdo.Dnld16( CIA402_VELWARN_WIN+offset, 0,(uint16)v );
}

const Error *CiA402::GetVelWindow( uint16 &value )
{
	uint16 v;
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	const Error *err = node->sdo.Upld16( CIA402_VELWARN_WIN+offset, 0, v );
	value = VelIntl2User( (int32)v );
	return err;
}

const Error *CiA402::SetVelWinTime( uint16 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Dnld16( CIA402_VELWARN_TIME+offset, 0, value );
}

const Error *CiA402::GetVelWinTime( uint16 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Upld16( CIA402_VELWARN_TIME+offset, 0, value );
}

const Error *CiA402::SetTargetVel( int32 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

//	return node->sdo.Dnld32( 0x2341 + offset, 0, RevVelocity(VelUser2Intl(value)) );
	return node->sdo.Dnld32( CIA402_TARGET_VEL + offset, 0, RevVelocity(VelUser2Intl(value)) );
}

const Error *CiA402::GetTargetVel( int32 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	int32 v;
	const Error *err = node->sdo.Upld32( CIA402_TARGET_VEL + offset, 0, v );
	value = VelIntl2User(RevVelocity(v));
	return err;
}

const Error *CiA402::GetPositionWindow( int32 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	int32 v;
	const Error *err = node->sdo.Upld32( CIA402_POS_WINDOW+offset, 0, v );
	value = v;
	return err;
}

const Error *CiA402::SetPositionWindow( int32 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	return node->sdo.Dnld32( CIA402_POS_WINDOW + offset, 0, value);
}

const Error *CiA402::GetPositionWindowTime( int32 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	int32 v;
	const Error *err = node->sdo.Upld32( CIA402_POS_WINDOW_TIME+offset, 0, v );
	value = v;
	return err;
}

const Error *CiA402::SetPositionWindowTime( int32 value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	return node->sdo.Dnld32( CIA402_POS_WINDOW_TIME + offset, 0, value);
}

const Error *CiA402::GetPositionActual( int32 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	int32 v;
	const Error *err = node->sdo.Upld32( CIA402_POS_ACT+offset, 0, v );
	value = PosIntl2User( v );
	return err;
}

const Error *CiA402::GetVelocityActual( int32 &value )
{
	int32 v;
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	const Error *err = node->sdo.Upld32( CIA402_VEL_ACT + offset, 0, v );
	value = VelIntl2User( RevVelocity(v) );

	return err;
}

const Error *CiA402::GetVelocityCommand( int32 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	int32 v;
	const Error *err = node->sdo.Upld32( CIA402_VEL_CMD+offset, 0, v );
	value = VelIntl2User( v );
	return err;
}

const Error *CiA402::GetCurrentRate(int32 &value)
{
	int16 offset = (int16)(0x800 * (GetDeviceID() - 1));
	RefObjLocker<MDNode> node(GetNodeRef());
	if (!node) return &DeviceError::NodeUnavailable;

	const Error *err = node->sdo.Upld32(CIA402_RATE_CURRENT + offset, 0, value);
	return err;
}

const Error *CiA402::GetCurrentActual( int16 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	return node->sdo.Upld16( CIA402_CRNT_ACT + offset, 0, value );
}

const Error *CiA402::GetDriverTemp( int16 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	return node->sdo.Upld16( CIA402_DRV_TEMP + offset, 0, value );
}

const Error *CiA402::SetIoConfig( IoCfg &cfg )
{
	uint8 inCt, outCt = 0;
	int i;

	const Error *err = GetInputCnt( inCt );
	//	if( !err ) err = sdo.Upld8( OBJID_OUTPUT_CFG, 0, outCt );
	//	if( !err ) err = SetIoPullup( cfg.inPullUpCfg );

	if( inCt > cfg.inputCt ) inCt = cfg.inputCt;
	//	if( outCt > cfg.outputCt ) outCt = cfg.outputCt;

	if( inCt  > MAX_INPUTS  ) inCt  = MAX_INPUTS;
	if( outCt > MAX_OUTPUTS ) outCt = MAX_OUTPUTS;

	for( i=0; i<inCt && !err; i++ )
	{
		err = SetInputConfig( i, cfg.inCfg[i] );
		//		if( !err )
		//			err = SetInputDebounce( i, cfg.inDebounce[i] );
	}

	//	for( i=0; i<outCt && !err; i++ )
	//		err = SetOutputConfig( i, cfg.outCfg[i], cfg.outMask[i], cfg.outMask1[i] );

	return err;
}

const Error *CiA402::GetIoConfig( IoCfg &cfg )
{
	int i;

	const Error *err = GetInputCnt( cfg.inputCt );
	//	if( !err ) err = sdo.Upld8( OBJID_OUTPUT_CFG, 0, cfg.outputCt );
	//	if( !err ) err = GetIoPullup( cfg.inPullUpCfg );

	if( cfg.inputCt  > MAX_INPUTS  ) cfg.inputCt  = MAX_INPUTS;
	//	if( cfg.outputCt > COPLEY_MAX_OUTPUTS ) cfg.outputCt = COPLEY_MAX_OUTPUTS;

	for( i=0; i<cfg.inputCt && !err; i++ )
	{
		err = GetInputConfig( i, cfg.inCfg[i] );
		//		if( !err )
		//			err = GetInputDebounce( i, cfg.inDebounce[i] );
	}

	//	for( i=0; i<cfg.outputCt && !err; i++ )
	//		err = GetOutputConfig( i, cfg.outCfg[i], cfg.outMask[i], cfg.outMask1[i] );

	return err;
}

const Error *CiA402::SetInputConfig( int8 pin, CIA402_2192_CODE cfg )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	return node->sdo.Dnld16( CIA402_INPUT_CFG + offset, pin+1, (uint16)cfg );
}

const Error *CiA402::GetInputConfig( int8 pin, CIA402_2192_CODE &cfg )
{
	uint16 c;
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	const Error *err = node->sdo.Upld16( CIA402_INPUT_CFG + offset, pin+1, c );
	cfg = (CIA402_2192_CODE)c;
	return err;
}

const Error *CiA402::GetInputs( uint16 &value, bool viaSDO )
{
	const Error *err = 0;
	if(viaSDO)
	{
		int16 offset = (int16)(0x800*(GetDeviceID() - 1));
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		uint32 i;
		err =  node->sdo.Upld32( CIA402_INPUTS + offset, 0, i );
		value = (uint16)(i>>16);
	}
	else
		value = (uint16)inputEvent_.getMask();

	return err;
}

const Error *CiA402::GetInputCnt( uint8 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	return node->sdo.Upld8( CIA402_INPUT_CFG + offset, 0, value );
}

const Error *CiA402::WaitInputEvent( Event &e, Timeout timeout, uint32 &match )
{
	const Error *err = e.Wait( inputEvent_, timeout );
	match = e.getMask();
	return err;
}

//const Error *CiA402::WaitInputHigh( uint32 inputs, Timeout timeout )
//{
//	EventAny e( inputs );
//	uint32 match;
//
//	return WaitInputEvent( e, timeout, match );
//}

//const Error *CiA402::WaitInputLow( uint32 inputs, Timeout timeout )
//{
//	EventAnyClear e( inputs );
//	uint32 match;
//
//	return WaitInputEvent( e, timeout, match );
//}

void CiA402::MoveAborted( void )
{

}

const Error *CiA402::SetTargetPos( int32 value )
{
    //ClearFaults();
    //Enable();
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	// 1.设置运动模式
    int16 type=0;
    const Error *err = node->sdo.Dnld16( 0x6086+offset, 0, type );
	if (err)
	{
		return err;
	}
    err = node->sdo.Upld16( 0x6086+offset, 0, type );
	if (err)
	{
		return err;
	}
    nspinfo <<"input type="<<type;

	// 2.设置目标高度
	int32 v=0;
	err = node->sdo.Dnld32( CIA402_TARGET_POS+offset, 0, value );
	if (err)
	{
		return err;
	}
    err = node->sdo.Upld32( CIA402_TARGET_POS+offset, 0, v );
	if (err)
	{
		return err;
	}
	nspinfo << "input pos=" << v;

	// 3.清除控制字的bit4
    int32 ctrl;
    err = node->sdo.Upld32( 0x6040+offset, 0, ctrl );
	if (err)
	{
		return err;
	}
	nspinfo << "ctrl read=" << ctrl;
    if ((ctrl&0x10) == 0x10)
    {
        ctrl&=(~0x00000010);
    }
    err = node->sdo.Dnld32( 0x6040+offset, 0, ctrl );
	if (err)
	{
		return err;
	}
	nspinfo << "ctrl write=" << ctrl;

	// 4.读取状态字的bit10(Target Reached)是否为1
    int32 reach;
    err = node->sdo.Upld32( 0x6041+offset, 0, reach );
	if (err)
	{
		return err;
	}
	nspinfo << "0x6041=" << reach << " Target Reached=1:" << ((reach & 0x400) ? true : false);

	// 5.设置控制字的bit4为1
    ctrl|=0x00000010;
	err = node->sdo.Dnld32( 0x6040+offset, 0, ctrl );
	if (err)
	{
		return err;
	}

	// 6.读取控制字的bit12(setpoint acknowledge)是否为1
    err = node->sdo.Upld32( 0x6041+offset, 0, reach );
	if (err)
	{
		return err;
	}
	nspinfo << "0x6041=" << reach << " setpoint acknowledge=1:" << ((reach & 0x1000) ? true : false);

	// 7.控制控制字的bit4(new setpoint)为0
    err = node->sdo.Upld32( 0x6040+offset, 0, ctrl );
	if (err)
	{
		return err;
	}
	nspinfo << "ctrl read=" << ctrl ;
    if ((ctrl&0x10) == 0x10)
    {
        ctrl&=(~0x00000010);
    }
    err = node->sdo.Dnld32( 0x6040+offset, 0, ctrl );
	if (err)
	{
		return err;
	}
	nspinfo << "ctrl write=" << ctrl;

	// 8.读取控制字的bit12(setpoint acknowledge)是否为0
    err = node->sdo.Upld32( 0x6041+offset, 0, reach );
	if (err)
	{
		return err;
	}
	nspinfo << "0x6041=" << reach << " setpoint acknowledge=0:" << ((reach & 0x1000) ? true : false);
	return err;
}

const Error *CiA402::GetTargetPos( int32 &value )
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	int32 v;
	const Error *err = node->sdo.Upld32( CIA402_POS_ACT+offset, 0, v );

	value = PosIntl2User( v );
	return err;
}

const Error *CiA402::GetTargetPos2( int32 &value)
{
	int16 offset = (int16)(0x800*(GetDeviceID() - 1));
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	int32 v;
	const Error *err = node->sdo.Upld32( CIA402_POS2_ACT+offset, 0, v );

	value = PosIntl2User( v );
	return err;
}

const Error* CiA402::PosIsReached(bool& b)
{
    b = false;
    int16 offset = (int16)(0x800*(GetDeviceID() - 1));
    RefObjLocker<MDNode> node( GetNodeRef() );
    if( !node ) return &DeviceError::NodeUnavailable;

    int32 reach;
    const Error *err =node->sdo.Upld32( 0x6041+offset, 0, reach );
    std::cout<<"0x6041="<<reach<<std::endl;

    b = (reach&0x0400) == 0x0400;

    return err;
}

const Error* CiA402::SetProfileVel( int32 value )
{
    int16 offset = (int16)(0x800*(GetDeviceID() - 1));
    RefObjLocker<MDNode> node( GetNodeRef() );
    if( !node ) return &DeviceError::NodeUnavailable;

    return node->sdo.Dnld32( CIA402_PROFILE_VELOCITY+offset, 0, VelUser2Intl(value) );
}