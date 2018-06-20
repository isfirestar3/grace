#ifndef _CIA402STC_
#define _CIA402STC_

#include "CML_Device.h"
#include "CML_Settings.h"
#include "CML_Node.h"
#include "CML_PDO.h"
#include "CML_CiA301.h"
#include "CML_CiA402Def.h"
#include "CML_CiA402.h"
CML_NAMESPACE_START()

class CiA402Error: public DeviceError
{
public:
	static const CiA402Error Fault;         ///< Latching fault is active
	static const CiA402Error ShortCircuit;  ///< Short circuit detected
	static const CiA402Error OverVolt;      ///< Amplifier over voltage
	static const CiA402Error EncoderPower;  ///< Encoder power error
	static const CiA402Error PhaseErr;      ///< Motor phasing error
	static const CiA402Error TrackErr;      ///< Position tracking error
	static const CiA402Error badMoveParam;  ///< Bad parameter specified to move command
	static const CiA402Error InMotion;      ///< Amplifier is currently in motion
	static const CiA402Error PosLim;        ///< Positive limit switch is active
	static const CiA402Error NegLim;        ///< Negative limit switch is active
	static const CiA402Error PosSoftLim;    ///< Positive software limit is active
	static const CiA402Error NegSoftLim;    ///< Negative software limit is active
	static const CiA402Error Unknown;       ///< An error occurred, but went away before it could be decoded.
	static const CiA402Error Reset;         ///< The amplifier has been reset.
	static const CiA402Error Disabled;      ///< The amplifier is disabled
	static const CiA402Error QuickStopMode; ///< The amplifier is doing a quick stop
	static const CiA402Error NoUserUnits;   ///< User units are not available (see CML_Settings.h)
	static const CiA402Error Abort;   ///< Trajectory aborted
	static const CiA402Error VelWin;        ///< Velocity tracking window exceeded
	static const CiA402Error PhaseInit;     ///< Amplifier is currently performing a phase initialization.

	static const CiA402Error *DecodeStatus( CIA402_EVENT_STATUS stat );
protected:
	/// Standard protected constructor
	CiA402Error( uint16 id, const char *desc ): DeviceError( id, desc ){}
};


/***************************************************************************/
/**
This class represents latching amplifier fault conditions.
 */
/***************************************************************************/
class CiA402Fault: public CiA402Error
{
public:
	static const CiA402Fault Memory;         ///< Fatal hardware error: the flash data is corrupt.
	static const CiA402Fault ADC;            ///< Fatal hardware error: An A/D offset error has occurred.
	static const CiA402Fault ShortCircuit;   ///< The amplifier detected a short circuit condition
	static const CiA402Fault AmpTemp;        ///< The amplifier is over temperature
	static const CiA402Fault MotorTemp;      ///< A motor temperature error was detected
	static const CiA402Fault OverVolt;       ///< The amplifier bus voltage is over the acceptable limit
	static const CiA402Fault UnderVolt;      ///< The amplifier bus voltage is below the acceptable limit
	static const CiA402Fault EncoderPower;   ///< Over current on the encoder power supply
	static const CiA402Fault PhaseErr;       ///< Amplifier phasing error
	static const CiA402Fault TrackErr;       ///< Tracking error, the position error is too large.
	static const CiA402Fault I2TLimit;       ///< Current limited by I^2 algorithm.
	static const CiA402Fault PosLimit;       ///< Positive limit switch is active.
	static const CiA402Fault NegLimit;       ///< Negative limit switch is active.
	static const CiA402Fault Unknown;		  ///< Some unknown amplifier fault has occurred

	static const CiA402Fault *DecodeFault( uint32 fault );
protected:
	/// Standard protected constructor
	CiA402Fault( uint16 id, const char *desc ): CiA402Error( id, desc ){}
};


struct FactorCfg
{
	uint32 posFctNum;	//0x6093, 1
	uint32 posFctDiv;	//0x6093, 2

	uint32 velFctNum;	//0x6094, 1
	uint32 velFctDiv;	//0x6094, 2

	uint32 accFctNum;	//0x6097, 1
	uint32 accFctDiv;	//0x6097, 2

	uint8 polarity;		//0x607e

	FactorCfg(void);
};

class CiA402Settings
{
public:

	FactorCfg	factor;
	//	uunit cts;
	bool resetNodeOnInit;
	CIA402_6060_CODE initialMode;
	CiA402Settings();
};

/***************************************************************************/
/**
  Programmable I/O pin configuration
 */
/***************************************************************************/
struct IoCfg
{
	/// Maximum available on any amplifier
#define MAX_INPUTS         16
#define MAX_OUTPUTS        8

	/// Number of programmable inputs available on this amplifier.
	/// This is a read only parameter, writes have no effect.
	uint8 inputCt;

	/// Number of programmable outputs available on this amplifier.
	/// This is a read only parameter, writes have no effect.
	uint8 outputCt;

	/// Input pin pull-up / pull-down resister configuration.
	/// See Amp::SetIoPullup for more information
	uint16 inPullUpCfg;

	/// Input pin configuration for each pin.
	/// See Amp::SetInputConfig for more information.
	CIA402_2192_CODE inCfg[ MAX_INPUTS ];

	/// Input pin debounce time (milliseconds)
	int16 inDebounce[ MAX_INPUTS ];

	/// Output pin configuration for each output pin
	/// See Amp::SetOutputConfig for more information
	CIA402_OUTPUT_CONFIG outCfg[ MAX_OUTPUTS ];

	/// Output pin configuration mask for each pin.
	/// See Amp::SetOutputConfig for more information
	uint32 outMask[ MAX_OUTPUTS ];

	/// Output pin configuration mask for each pin.
	/// See Amp::SetOutputConfig for more information
	uint32 outMask1[ MAX_OUTPUTS ];

	IoCfg( void );
};

/***************************************************************************/
/**
  Motor information structure.  This structure holds information about the
  motor connected to the amplifier.

  The amplifier uses the information in this structure when controlling the
  motor.  It is very important that the information provided to the amplifier
  be as accurate as possible for proper motor control.

  Use the methods Amp::GetMtrInfo and Amp::SetMtrInfo to upload / download
  the information contained in this structure.

  Note that unlike many amplifier parameters, motor parameters are always
  stored in non-volatile flash memory.
 */
/***************************************************************************/
struct MtrCfg
{
	/// Motor type
	uint16 type;

	/// Name of the motor manufacturer
	//   char mfgName[ COPLEY_MAX_STRING ];

	/// Motor model number
	//   char model[ COPLEY_MAX_STRING ];

	/// Number of pole pairs (i.e. number of electrical phases)
	/// per rotation.  Used for rotory motors only.
	int16 poles;

	/// Motor resistance (10 milliohm units)
	uint16 resistance;

	/// Motor inductance (10 microhenry units)
	uint16 inductance;

	/// Peak torque (0.01 Newton millimeters)
	uint32 trqPeak;

	/// Continuous torque (0.01 Newton millimeters)
	uint32 trqCont;

	/// Torque constant (0.01 Newton millimeters / Amp)
	uint32 trqConst;

	/// Max velocity
	//   uunit  velMax;
	int32 velMax;

	/// Back EMF constant (10 millivolts / KRPM)
	uint32 backEMF;

	/// Inertia
	uint32 inertia;

	/// Motor has a temperature sensor (true/false)
	bool   tempSensor;

	/// Reverse motor wiring if true
	bool   mtrReverse;

	/// Reverse encoder direction if true
	bool   encReverse;

	/// Type of hall sensors on motor.  See documentation for details
	int16 hallType;

	/// Hall wiring code, see documentation for details
	int16 hallWiring;

	/// Hall offset (degrees)
	int16 hallOffset;

	/// Motor has a brake if true
	bool   hasBrake;

	/// Delay (milliseconds) between disabling amp & applying brake
	/// During this time the amp will attempt to actively stop motor
	int16 stopTime;

	/// Delay (milliseconds) between applying brake & disabling PWM
	int16 brakeDelay;

	/// Velocity below which brake will be applied.
	uunit brakeVel;

	/// Encoder type.  See documentation for details
	int16 encType;

	/// Encoder counts / revolution (rotory motors only)
	int32 ctsPerRev;

	/// Encoder units (linear motor only)
	//   int16 encUnits;

	/// Encoder resolution (encoder units / count) - linear motors only
	//   int16 encRes;

	/// Motor electrical distance (encoder units / electrical phase) - linear only
	//   int32 eleDist;

	/// Motor units (used by CME program)
	int16 mtrUnits;

	/// Microsteps / motor rotation (used for Stepnet amplifiers)
	//   int32 stepsPerRev;

	/// Analog Encoder shift value (used only with Analog encoders)
	//   int16 encShift;

	/// Index mark distance (reserved for future use)
	//   int32 ndxDist;

	/// Load encoder type (0 for none).  See amplifier documentation
	/// for possible values.
	//   int16 loadEncType;

	/// Load encoder resolution.  This is encoder counts/rev for rotory
	/// encoders, or nanometers/count for linear encoders.
	//   int32 loadEncRes;

	/// Reverse load encoder if true.
	//   bool  loadEncReverse;

	/// Load encoder gear ratio.  This parameter is used by the CME software
	/// and gives a ratio of motor encoder counts to load encoder counts.
	//   int32 gearRatio;

	/// Resolver cycles / rev.  This parameter gives the number of
	/// resolver cycles / motor rev.  It's only used on systems that
	/// use a resolver for position feedback.  Default is 1 cycle/rev.
	//   uint16 resolverCycles;

	/// Hall velocity shift value.  This parameter is only used on servo systems where
	/// there is no encoder and digital hall sensors are used for velocity feedback.
	/// In that case, this shift value can be used to scale up the calculated velocity.
	//   int16 hallVelShift;

	/// Motor Encoder options.  This bitmapped parameter is used to specify various
	/// configuration options for the motor encoder.  See documentation.
	uint32 mtrEncOptions;

	/// Load Encoder options.  This bitmapped parameter is used to specify various
	/// configuration options for the load encoder.  See documentation.
	//   uint32 loadEncOptions;

	MtrCfg( void );
};

/***************************************************************************/
/**
  This structure holds the velocity loop configuration parameters
  specific to the Copley amplifier.

  The velocity loop is one of three servo control loops used by the amplifier
  to control a motor.  The configuration parameters used by this control loop
  allow the servo performance to be 'tuned' for various motors and loads.

  The amplifier member functions Amp::GetVelLoopConfig and Amp::SetVelLoopConfig
  are used to read and write this data to the amplifier.
 */
/***************************************************************************/
struct VelLoopCfg
{
	/// Proportional gain
	uint16 kp;

	/// Integral gain
	uint16 ki;

	/// Acceleration feed forward
	int16 kaff;

	/// Output shift value.  The output of the loop is downshifted
	/// this many bits to get the current loop command
	int16 shift;

	/// Velocity loop drain value for integral sum. Set to 0 to disable
	int16 viDrain;

	/// Maximum allowed velocity.
	/// This value is used to limit the velocity command before the
	/// velocity loop uses it to calculate output current.
	/// When running in a position mode (normal for CAN operation)
	/// The velocity command is the output from the position loop.
	/// This command is clipped by this value before it is passed
	/// to the velocity loop.
	///
	/// This parameter is specified in "user units".  See
	/// Amp::SetCountsPerUnit for details.
	//   uunit maxVel;
	int32 maxVel;

	/// Maximum allowed acceleration
	///
	/// This value limits the rate of change of the velocity command
	/// input to the velocity loop.  It is used when the magnitude of
	/// the command is increasing.
	///
	/// Note that the acceleration & deceleration limits are NOT used
	/// when the position loop is driving the velocity loop.
	///
	/// This parameter is specified in "user units".  See
	/// Amp::SetCountsPerUnit for details.
	//   uunit maxAcc;
	int32 maxAcc;

	/// Maximum allowed deceleration
	/// This value limits the rate of change of the velocity command
	/// input to the velocity loop.  It is used when the magnitude of
	/// the command is decreasing.
	///
	/// Note that the acceleration & deceleration limits are not used
	/// when the position loop is driving the velocity loop.
	///
	/// This parameter is specified in "user units".  See
	/// Amp::SetCountsPerUnit for details.
	//   uunit maxDec;
	int32 maxDec;

	/// Deceleration used for emergency stop
	/// When the position loop is driving the velocity loop this value
	/// is only used for tracking error conditions.  If a tracking error
	/// occurs, the velocity loop takes over control and drives to zero
	/// velocity using this deceleration value.
	///
	/// Setting this value to zero indicates that the deceleration is
	/// unlimited.
	///
	/// This parameter is specified in "user units".  See
	/// Amp::SetCountsPerUnit for details.
	//   uunit estopDec;
	int32 estopDec;

	/// Default constructor.
	/// Simply initializes all members to zero.
	VelLoopCfg( void )
	{
		kp = 0;
		ki = 0;
		kaff = 0;
		shift = 0;
		viDrain = 0;
		maxVel = 0;
		maxAcc = 0;
		maxDec = 0;
		estopDec = 0;
	}
};


/***************************************************************************/
/**
  This structure holds the current loop configuration parameters.

  The current loop is one of three servo control loops used by the amplifier
  to control a motor.  The configuration parameters used by this control loop
  allow the servo performance to be 'tuned' for various motors and loads.

  This structure also holds the parameters used to control current limiting.
  The current limiting acts on the commanded current before it is sent to the
  current loop.

  The amplifier member functions Amp::GetCrntLoopConfig and Amp::SetCrntLoopConfig
  are used to read and write this data to the amplifier.
 */
/***************************************************************************/
struct CrntLoopCfg
{
	/// Proportional gain
	uint16 kp;

	/// Integral gain
	uint16 ki;

	/// Current offset
	int16 offset;

	/// A calibration offset value, calculated at startup, and applied to the
	/// winding A and B current reading.
	int16 offsetA;	// 0x2210
	int16 offsetB;	// 0x2211

	/// Peak current limit (0.01 amp units)
	/// This is the maximum current that can be applied to the
	/// motor at any time
	/// Also used as Boost current in stepper mode
	int16 peakLim;

	/// Continuous current limit (0.01 amp units)
	/// This is the maximum current that can continuously be
	/// applied to the load.
	/// Also used as Run current in stepper mode.
	int16 contLim;

	/// Time at peak current limit (milliseconds)
	/// If peak current is requested, it will fall back to the
	/// continuous limit within this amount of time.
	/// Also used as Boost current time in stepper mode.
	uint16 peakTime;

	/// Stepper hold current (0.01 amps).
	/// Current used to hold the motor at rest.
	/// Used in stepper mode only.
	uint16 stepHoldCurrent;

	/// Run to hold time(milliseconds)
	/// The period of time, beginning when a move is complete,
	/// to when output current switched to hold current.
	/// Used in stepper mode only.
	uint16 stepRun2HoldTime;

	/// Voltage control mode time delay (milliseconds)
	/// Time delay to enter into a special voltage control mode.
	/// If set to zero this feature is disabled.
	/// Used for stepper mode only.
	uint16 stepVolControlDelayTime;

	/// Rate of change of current command (milliamps/sec).
	/// This parameter is only used when running in the low level
	/// programmed current mode (AMPMODE_PROG_CRNT), or in the
	/// CANopen profile torque mode (AMPMODE_CAN_TORQUE).  In
	/// other modes this parameter is ignored and no limit is placed
	/// on the slope of the current command.
	///
	/// If this parameter is set to zero (default) it is not used
	/// in any mode of operation.
	///
	/// Note that this parameter is internally the same as the torque
	/// slope parameter which can be set using the function
	/// Amp::SetTorqueSlope.  The units are different however as this
	/// parameter controls slope in units of current and the torque slope
	/// function adjusts in units of torque.
	int32 slope;

	/// Default constructor.
	/// Simply initializes all members to zero.
	CrntLoopCfg( void )
	{
		kp = 0;
		ki = 0;
		offset = 0;
		slope = 0;

		peakLim = 0;
		contLim = 0;
		peakTime = 0;

		stepHoldCurrent = 0;
		stepRun2HoldTime = 0;
		stepVolControlDelayTime = 0;
	}
};


/***************************************************************************/
/**
  Position and velocity error windows.
 */
/***************************************************************************/
struct TrackWinCfg
{
	/// Tracking error window.
	/// See Amp::SetPositionErrorWindow for more information
	//   uunit trackErr;

	/// Position warning window.
	/// See Amp::SetPositionWarnWindow for more information
	//   uunit trackWarn;

	/// Position tracking & settling window.
	/// See Amp::SetSettlingWindow for more information
	//   uunit settlingWin;

	/// Position tracking & settling time (ms).
	/// See Amp::SetSettlingTime for more info
	//   uint16 settlingTime;

	/// Velocity warning window
	/// See Amp::SetVelocityWarnWindow for more information
	//   uunit velWarnWin;
	uint16 velWarnWin;

	/// Velocity warning window time
	/// See Amp::SetVelocityWarnTime for more information
	uint16 velWarnTime;

	TrackWinCfg( void )
	{
		//	   trackErr     = 0;
		//	   trackWarn    = 0;
		//	   settlingWin  = 0;
		//	   settlingTime = 0;
		velWarnWin   = 0;
		velWarnTime  = 0;
	};
};


/***************************************************************************/
/**
  Amplifier configuration structure.  This structure contains all user
  configurable parameters used by an amplifier which may be stored in non-volatile
  memory.
 */
/***************************************************************************/
struct CiA402Cfg
{
	/// Velocity loop configuration
	VelLoopCfg    vLoop;

	/// Current loop configuration
	CrntLoopCfg   cLoop;

	/// Motor information
	MtrCfg          motor;

	/// Tracking window settings
	TrackWinCfg  window;

	/// General purpose I/O pin configuration
	IoCfg         io;

	/// Default constructor.  Simply sets all members to zero.
	CiA402Cfg()
	{
		//      name[0] = 0;
		//      controlMode = AMPMODE_DISABLED;
		//      phaseMode = PHASE_MODE_ENCODER;
		//      pwmMode = PWM_MODE_STANDARD;
		//      CME_Config[0] = 0;
		//      faultMask = (AMP_FAULT)0;
		//      progVel = 0;
		//      progCrnt = 0;
		//      options = 0;
		//      stepRate = 0;
		//      capCtrl = 0;
		//      limitBitMask = (EVENT_STATUS)0;
		//      encoderOutCfg = 0;
	}
};


/***************************************************************************/
/**
  Homing parameter structure.  This structure allows all homing parameters
  to be grouped together and passed to the amplifier for convenience.
 */
/***************************************************************************/
struct HomeCfg
{
	/// Homing method to use
	CIA402_6098_CODE method;

	/// Offset from located home position to zero position.
	/// After the home position is found as defined by the home method,
	/// this offset will be added to it and the resulting position will
	/// be considered the zero position.
	/// This parameter is specified in "user units".  See
	/// Amp::SetCountsPerUnit for details.
	int32 offset;

	HomeCfg( void )
	{
		method = HM_NONE;
		offset  = 0;
	}
};



class TPDO_402Status: public TPDO
{
	class CiA402 *device_;                ///< Points to the amplifier to which this PDO belongs
	//	uint32 devRef_;	 ///< Points to the amplifier to which this PDO belongs
	CML::uint16 oldStatus_;
	CML::uint32 oldInputs_;
	bool first_;

	PDO_EVENT eventMask_;

	Pmap16 status_;                    ///< Used to map the CAN status word passed in the PDO
	Pmap32 inputs_;

	uint16 slot_;

	Mutex mtx_;

public:
	/// Default constructor for this PDO
	TPDO_402Status();
	virtual ~TPDO_402Status();
	const Error *Init(CiA402 &device);
	const Error *Start(uint16 slot = 0);
	const Error *Stop();
	uint16 GetStatus();
	uint32 GetInputs();

private:
	virtual void Received( void );
	void UpdateEvents( uint16 stat, uint32 inputs );

	/// Private copy constructor (not supported)
	TPDO_402Status( const TPDO_402Status& );

	/// Private assignment operator (not supported)
	TPDO_402Status& operator=( const TPDO_402Status& );
};


CML_NAMESPACE_END()
#endif //_CIA402STC_
