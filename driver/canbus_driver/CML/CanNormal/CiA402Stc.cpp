#include "CML.h"
#include <stdio.h>
CML_NAMESPACE_USE();

// Amplifier specific error objects
CML_NEW_ERROR( CiA402Error, badMoveParam,  "Bad parameter passed to move function" );
CML_NEW_ERROR( CiA402Error, InMotion,      "The amplifier is currently executing a move" );
CML_NEW_ERROR( CiA402Error, Fault,         "The amplifier detected a latching fault" );
CML_NEW_ERROR( CiA402Error, ShortCircuit,  "The amplifier detected a short circuit condition" );
CML_NEW_ERROR( CiA402Error, OverVolt,      "The amplifier detected an over voltage condition" );
CML_NEW_ERROR( CiA402Error, EncoderPower,  "The amplifier detected an encoder power error" );
CML_NEW_ERROR( CiA402Error, PhaseErr,      "The amplifier detected a phasing error" );
CML_NEW_ERROR( CiA402Error, TrackErr,      "The amplifier detected a tracking error." );
CML_NEW_ERROR( CiA402Error, PosLim,        "Positive limit switch is active" );
CML_NEW_ERROR( CiA402Error, NegLim,        "Negative limit switch is active" );
CML_NEW_ERROR( CiA402Error, PosSoftLim,    "Positive software limit is active" );
CML_NEW_ERROR( CiA402Error, NegSoftLim,    "Negative software limit is active" );
CML_NEW_ERROR( CiA402Error, Unknown,       "An unknown amplifier error occurred" );
CML_NEW_ERROR( CiA402Error, Reset,         "An amplifier reset was detected" );
CML_NEW_ERROR( CiA402Error, Disabled,      "The amplifier is currently disabled" );
CML_NEW_ERROR( CiA402Error, QuickStopMode, "The amplifier is currently in quick stop mode" );
CML_NEW_ERROR( CiA402Error, NoUserUnits,   "User units are not enabled in CML_Settings.h" );
CML_NEW_ERROR( CiA402Error, Abort,         "Trajectory aborted" );
CML_NEW_ERROR( CiA402Error, PhaseInit,     "Amplifier is currently performing a phase initialization." );

CML_NEW_ERROR( CiA402Fault, Memory,        "Fatal hardware error: Amplifier flash data is corrupt." );
CML_NEW_ERROR( CiA402Fault, ADC,           "Fatal hardware error: An A/D offset error has occurred." );
CML_NEW_ERROR( CiA402Fault, ShortCircuit,  "The amplifier latched a short circuit condition" );
CML_NEW_ERROR( CiA402Fault, AmpTemp,       "The amplifier latched an over temperature error" );
CML_NEW_ERROR( CiA402Fault, MotorTemp,     "The amplifier latched a motor temperature error" );
CML_NEW_ERROR( CiA402Fault, OverVolt,      "The amplifier latched an over voltage condition" );
CML_NEW_ERROR( CiA402Fault, UnderVolt,     "The amplifier latched an under voltage condition" );
CML_NEW_ERROR( CiA402Fault, EncoderPower,  "The amplifier latched an encoder power error" );
CML_NEW_ERROR( CiA402Fault, PhaseErr,      "The amplifier latched a phasing error" );
CML_NEW_ERROR( CiA402Fault, TrackErr,      "The amplifier latched a tracking error." );
CML_NEW_ERROR( CiA402Fault, I2TLimit,      "Current limited by i^2t algorithm." );
CML_NEW_ERROR( CiA402Fault, PosLimit,      "Positive limit switch is active." );
CML_NEW_ERROR( CiA402Fault, NegLimit,      "Negative limit switch is active." );
CML_NEW_ERROR( CiA402Fault, Unknown, 	"Some unknown amplifier latched fault has occurred" );

/***************************************************************************/
/**
  Decode the passed event status word and return an appropriate error object.
  @param stat The amplifier event status register
  @return A pointer to an error object, or NULL if there is no error.
 */
/***************************************************************************/
const CiA402Error *CiA402Error::DecodeStatus( CIA402_EVENT_STATUS stat )
{
	if( stat & CIA402_ESTAT_SHORT_CRCT  ) return &CiA402Error::ShortCircuit;
	if( stat & CIA402_ESTAT_ENCODER_PWR ) return &CiA402Error::EncoderPower;
	if( stat & CIA402_ESTAT_PHASE_ERR   ) return &CiA402Error::PhaseErr;
	if( stat & CIA402_ESTAT_TRK_ERR     ) return &CiA402Error::TrackErr;
	if( stat & CIA402_ESTAT_OVER_VOLT   ) return &CiA402Error::OverVolt;
	if( stat & CIA402_ESTAT_FAULT       ) return &CiA402Error::Fault;
	if( stat & CIA402_ESTAT_POSLIM      ) return &CiA402Error::PosLim;
	if( stat & CIA402_ESTAT_NEGLIM      ) return &CiA402Error::NegLim;
	if( stat & CIA402_ESTAT_SOFTLIM_POS ) return &CiA402Error::PosSoftLim;
	if( stat & CIA402_ESTAT_SOFTLIM_NEG ) return &CiA402Error::NegSoftLim;

	return 0;
}

/***************************************************************************/
/**
  Return an appropriate fault object based on the amplifier fault mask.
  @param fault The amplifier fault mask.
  @return A pointer to the fault object, NULL if there is no fault.
 */
/***************************************************************************/
const CiA402Fault *CiA402Fault::DecodeFault( uint32 fault )
{
	if( fault == 0 ) return 0;

//	if( fault & CIA402_FAULT_DATAFLASH   ) return &CiA402Fault::Memory;
//	if( fault & CIA402_FAULT_ADCOFFSET   ) return &CiA402Fault::ADC;
	if( fault == CIA402_FAULT_SHORT_CRCT  ) return &CiA402Fault::ShortCircuit;
	if( fault == CIA402_FAULT_AMP_TEMP    ) return &CiA402Fault::AmpTemp;
	if( fault == CIA402_FAULT_MTR_TEMP    ) return &CiA402Fault::MotorTemp;
	if( fault == CIA402_FAULT_OVER_VOLT   ) return &CiA402Fault::OverVolt;
	if( fault == CIA402_FAULT_UNDER_VOLT  ) return &CiA402Fault::UnderVolt;
	if( fault == CIA402_FAULT_ENCODER_PWR ) return &CiA402Fault::EncoderPower;
	if( fault == CIA402_FAULT_PHASE_ERR   ) return &CiA402Fault::PhaseErr;
	if( fault == CIA402_FAULT_TRK_ERR     ) return &CiA402Fault::TrackErr;
	if( fault == CIA402_FAULT_I2T_ERR     ) return &CiA402Fault::I2TLimit;
	if( fault == CIA402_FAULT_POS_LIMIT   ) return &CiA402Fault::PosLimit;
	if( fault == CIA402_FAULT_NEG_LIMIT   ) return &CiA402Fault::NegLimit;

	return &CiA402Fault::Unknown;
}

CiA402Settings::CiA402Settings()
{
//	cts = 1.0;
	resetNodeOnInit = false;
	initialMode = CIA402MODE_CAN_PV;
}

FactorCfg::FactorCfg()
{
	posFctNum = 1;
	posFctDiv = 1;
	velFctNum = 1;
	velFctDiv = 1;
	accFctNum = 1;
	accFctDiv = 1;
	polarity = 0;
}


IoCfg::IoCfg( void )
{
   int i;

   inputCt = 0;
   outputCt = 0;
   inPullUpCfg = 0;

   for( i=0; i<MAX_INPUTS; i++ )
   {
      inCfg[i] = (CIA402_2192_CODE)0;
      inDebounce[i] = 0;
   }

   for( i=0; i<MAX_OUTPUTS; i++ )
   {
      outCfg[i] = (CIA402_OUTPUT_CONFIG)0;
      outMask[i] = 0;
	  outMask1[i] = 0;
   }
}


/***************************************************************************/
/**
  Motor info structure default constructor.  This simply initializes all members
  to legal default values.
  */
/***************************************************************************/
MtrCfg::MtrCfg( void )
{
//   mfgName[0] = 0;
//   model[0] = 0;

   hasBrake   = false;
   tempSensor = false;
   mtrReverse = false;
   encReverse = false;

   resistance = 100;
   inductance = 100;
   trqPeak = 10;
   trqCont = 10;
   trqConst = 100;
   backEMF = 1;
   inertia = 10;

   type  = 0;
   poles = 2;
   velMax = 1;
   hallType   = 1;
   hallWiring = 0;
   hallOffset = 0;
   stopTime = 0;
   brakeDelay = 0;
   brakeVel = 0;
   encType = 0;
   ctsPerRev = 4000;
//   encUnits = 0;
//   encRes = 100;
//   eleDist = 100000;
   mtrUnits = 0;
//   stepsPerRev = 4000;
//   encShift = 0;
//   ndxDist = 0;
//
//   loadEncType = 0;
//   loadEncRes = 0;
//   loadEncReverse = false;
//
//   gearRatio = 0x00010001;
//   resolverCycles = 1;
//   hallVelShift = 1;
}


TPDO_402Status::TPDO_402Status()
{
	SetRefName( "TPDO_Status" );
	device_ = 0;
}

TPDO_402Status::~TPDO_402Status()
{
	if(GetBitCt())
		Stop();
	KillRef();
}

const Error *TPDO_402Status::Init( CiA402 &device )
{
	device_ = &device;
	return 0;
}

const Error *TPDO_402Status::Start(uint16 slot)
{

	int16 offset = (int16)(0x800*(device_->GetDeviceID() - 1));
	RefObjLocker<MDNode> node( device_->GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	// Set transmit type to transmit on events
	eventMask_ = (CML::PDO_EVENT)(1<<slot);
	slot_ = slot;
	first_ = true;
	const Error *err = SetType( 0xff );

	// Let the various mapped variables know which
	// objects in the amp's object dictionary they
	// are linked to.
	if( !err ) err = status_.Init( CIA402_STATUS+offset, 0 );
	if( !err ) err = inputs_.Init( CIA402_INPUTS+offset, 0 );
	//	if( !err ) err = estat_.Init( CIA402_EVENT_STAT+offset, 0 );

	uint16 value16;
	uint32 value32;
	if( !err ) err = node->sdo.Upld16( CIA402_STATUS+offset, 0, value16 );
	if( !err ) status_.Write(value16);
	if( !err ) err = node->sdo.Upld32( CIA402_INPUTS+offset, 0, value32 );
	if( !err ) inputs_.Write(value32);


	// Add the mapped variables
	if( !err ) err = AddVar( status_ );
	if( !err ) err = AddVar( inputs_ );

	// Set the amplifier's event map to the uninitialized state
//	device_->cia402Event_.setMask( CIA402EVENT_NOT_INIT );
	UpdateEvents(value16, value32);

	if( !err ) err = node->PdoSet( slot_, *this );

	return err;
}

const Error *TPDO_402Status::Stop()
{
	const Error *err = ClearMap();
	printf("uninit pdo\n");

	RefObjLocker<MDNode> node( device_->GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	if( !err ) err = node->PdoDisable( slot_, *this );
	return err;
}

uint16 TPDO_402Status::GetStatus()
{
	return status_.Read();
}

uint32 TPDO_402Status::GetInputs()
{
	return inputs_.Read();
}

void TPDO_402Status::Received( void )
{
//	device_->PostPDOEvent(eventMask_);

	uint16 status = status_.Read();
	uint32 inputs = inputs_.Read();

	if( !first_ && (status==oldStatus_) && (inputs==oldInputs_) )
		return;

	UpdateEvents(status, inputs);

	oldStatus_ = status;
	oldInputs_ = inputs;
	first_ = false;
}

void TPDO_402Status::UpdateEvents( uint16 status, uint32 inputs )
{
	uint32 mask = 0;

	if(  status & CIA402_STAT_SPACK    ) mask |= CIA402EVENT_SPACK;
	if(  status & CIA402_STAT_MOVEDONE ) mask |= CIA402EVENT_MOVEDONE;
	if( ~status & CIA402_STAT_TRJ      ) mask |= CIA402EVENT_TRJDONE;
	if( ~status & CIA402_STAT_QSTOP    ) mask |= CIA402EVENT_QUICKSTOP;
	if(  status & CIA402_STAT_ABORT    ) mask |= CIA402EVENT_ABORT;
	if(  status & CIA402_STAT_HOMECAP  ) mask |= CIA402EVENT_HOME_CAPTURE;

	if(  status & CIA402_STAT_FAULT    ) mask |= CIA402EVENT_FAULT;
//	if(  status & ESTAT_TRK_WARN    ) mask |= CIA402EVENT_POSWARN;
//	if(  status & ESTAT_TRK_WIN     ) mask |= CIA402EVENT_POSWIN;
//	if(  status & ESTAT_VEL_WIN     ) mask |= CIA402EVENT_VELWIN;
	if(  status & CIA402_STAT_SOD  	   ) mask |= CIA402EVENT_DISABLED;
//	if(  status & ESTAT_POSLIM      ) mask |= CIA402EVENT_POSLIM;
//	if(  status & ESTAT_NEGLIM      ) mask |= CIA402EVENT_NEGLIM;
	if(  inputs & CIA402_POS_LIMIT 	   ) mask |= CIA402EVENT_SOFTLIM_POS;
	if(  inputs & CIA402_NEG_LIMIT 	   ) mask |= CIA402EVENT_SOFTLIM_NEG;
	if( ~inputs & CIA402_DRIVE_ENABLE  ) mask |= CIA402EVENT_SOFTDISABLE;
//	if( status & ESTAT_PHASE_INIT  ) mask |= CIA402EVENTT_PHASE_INIT;

	device_->cia402Event_.changeBits( ~(CIA402EVENT_PVT_EMPTY|CIA402EVENT_NODEGUARD), mask );
	device_->inputEvent_.setMask( inputs>>16 );
}
