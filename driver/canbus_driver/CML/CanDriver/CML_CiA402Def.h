#ifndef _CIA402DEF_
#define _CIA402DEF_

#include "CML_Settings.h"

#define COPLEY 0


CML_NAMESPACE_START()

/****************************************************************
 * CANopen drive status bits (CANopen 402 Object 0x6041)
 *****************************************************************/
#define CIA402STAT_RTSO                      0x0001   // Ready to switch on
#define CIA402STAT_ON                        0x0002   // Switched on
#define CIA402STAT_ENA                       0x0004   // Operation enabled
#define CIA402STAT_FAULT                     0x0008   // A fault has occurred on the drive
#define CIA402STAT_VENA                      0x0010   // Voltage enabled
#define CIA402STAT_QSTOP                     0x0020   // Doing quick stop when CLEAR
#define CIA402STAT_SOD                       0x0040   // Switch-on disabled
#define CIA402STAT_WARN                      0x0080   // A warning condition is in effect
#define CIA402STAT_ABORT                     0x0100   // Trajectory aborted
#define CIA402STAT_REMOTE                    0x0200   // Controlled by CANopen network if set
#define CIA402STAT_MOVEDONE                  0x0400   // Target reached
#define CIA402STAT_LIMIT                     0x0800   // Internal limit active
#define CIA402STAT_SPACK                     0x1000   // pp mode - setpoint acknowledge
#define CIA402STAT_TRACKERR                  0x2000   // pp mode - following error
#define CIA402STAT_REFERENCED                0x1000   // home mode - drive has been referenced
#define CIA402STAT_HOMEERR                   0x2000   // home mode - homing error
#define CIA402STAT_TRJ                       0x4000   // Trajectory active when set
#define CIA402STAT_HOMECAP                   0x8000   // Home position captured (requires 4.77+ firmware)

// Event status bits that are considered amplifier errors
#define CIA402ERR_EVENTS (CIA402_ESTAT_SHORT_CRCT | CIA402_ESTAT_AMP_TEMP | CIA402_ESTAT_MTR_TEMP | CIA402_ESTAT_ENCODER_PWR | \
		CIA402_ESTAT_PHASE_ERR | CIA402_ESTAT_TRK_ERR | CIA402_ESTAT_UNDER_VOLT | CIA402_ESTAT_OVER_VOLT)

//enum CIA402_PDO_EVENT
//{
//	PDO0				= 0x00000001,
//	PDO1                = 0x00000002,
//	PDO2                = 0x00000004,
//	PDO3                = 0x00000008
//};



/***************************************************************************/
/**
This enumeration holds the object identifiers of all of the objects in the
amplifier's object dictionary.
 */
/***************************************************************************/
enum CIA402_OBJID
{
	CIA402_CONTROL               = 0x6040,
	CIA402_STATUS                = 0x6041,
	CIA402_OP_MODE               = 0x6060,
	CIA402_OP_MODE_DISP          = 0x6061,
	CIA402_EVENT_STAT            = 0x1002,
	CIA402_EVENT_STAT_LATCH      = 0x2181,

	CIA402_VEL_CMD				 = 0x606B,
#if COPLEY
	CIA402_VEL_ACT               = 0x606C,
#else
	CIA402_VEL_ACT              =  0x6069,
#endif

	CIA402_VEL_LIM               = 0x2230,
	CIA402_TARGET_VEL            = 0x60FF,

	CIA402_POS_ACT               = 0x6064,
	CIA402_POS2_ACT              = 0x6063,
	CIA402_POS_WINDOW			 = 0x6067,
	CIA402_POS_WINDOW_TIME		 = 0x6068,

	//add by seven
	CIA402_TARGET_POS            = 0x607A, 

	CIA402_VELWARN_WIN           = 0x606D,
	CIA402_VELWARN_TIME          = 0x606E,

#if COPLEY
	CIA402_CRNT_ACT              = 0x221c,
#else
	CIA402_CRNT_ACT              = 0x6078,
	CIA402_RATE_CURRENT			 = 0x6075,
#endif

	CIA402_CRNT_LIM              = 0x221E,

	CIA402_INPUTS                = 0x60FD,
	CIA402_INPUTS_16B            = 0x2190,

	CIA402_INPUT_CFG             = 0x2192,
	CIA402_OUTPUT_CFG            = 0x2193,
	CIA402_OUTPUTS               = 0x60FE,

	CIA402_PROFILE_QSTOP         = 0x6085,
	CIA402_QSTOP_MODE            = 0x605A,
	CIA402_HALT_MODE             = 0x605D,

	CIA402_CRNTLOOP              = 0x60F6,
	CIA402_VELLOOP               = 0x60F9,

	CIA402_HOME_OFFSET			 = 0x607c,
	CIA402_HOME_METHOD			 = 0x6098,

	//	CIA402_

	CIA402_VELLIM_MAXACC         = 0x6083,
	CIA402_VELLIM_MAXDEC         = 0x6084,
	CIA402_VELLIM_ESTOP          = 0x2102,
	CIA402_VELLIM_MAXVEL         = 0x2103,

	CIA402_PROFILE_VELOCITY		 = 0x6081,
	CIA402_PROFILE_ACC			 = 0x6083,
	CIA402_PROFILE_DCC			 = 0x6084,

	CIA402_PROFILE_TYPE          = 0x6086,

	CIA402_CRNTLIM_PEAK          = 0x2110,
	CIA402_CRNTLIM_CONT          = 0x2111,
	CIA402_CRNTLIM_TIME          = 0x2112,

	CIA402_CRNTOFFSET_A          = 0x2210,
	CIA402_CRNTOFFSET_B          = 0x2211,

	CIA402_MOTOR_INFO            = 0x6410,
	CIA402_AMP_INFO              = 0x6510,

	CIA402_PWM_MIN             	 = 0x2323,
	CIA402_PWM_MAX               = 0x2324,
	CIA402_PWM_CMD               = 0x2325,

	CIA402_CANID_CFG             = 0x21B0,

	CIA402_CAN_QSTOP             = 0x605A,
	CIA402_CAN_HALT              = 0x605D,

	CIA402_CAN_ERR_HISTORY		 = 0x1003,
	CIA402_CAN_HARTBEAT_TIME     = 0x1017,
	CIA402_CAN_NODEGUARD_TIME    = 0x100C,
	CIA402_CAN_NODEGUARD_LIFE    = 0x100D,

	CIA402_DRV_TEMP				 = 0x2202,
};


/***************************************************************************/
/**
Amplifier event status word bit definitions. Manufacturer status register 0x1002.
 */
/***************************************************************************/
enum CIA402_EVENT_STATUS
{
	CIA402_ESTAT_SHORT_CRCT    = 0x00000001,  ///< Amplifier short circuit
	CIA402_ESTAT_AMP_TEMP      = 0x00000002,  ///< Amplifier over temperature
	CIA402_ESTAT_OVER_VOLT     = 0x00000004,  ///< Amplifier over voltage
	CIA402_ESTAT_UNDER_VOLT    = 0x00000008,  ///< Amplifier under voltage
	CIA402_ESTAT_MTR_TEMP      = 0x00000010,  ///< Motor over temperature
	CIA402_ESTAT_ENCODER_PWR   = 0x00000020,  ///< Encoder power error
	CIA402_ESTAT_PHASE_ERR     = 0x00000040,  ///< Phasing error
	CIA402_ESTAT_CRNT_LIM      = 0x00000080,  ///< Current limited
	CIA402_ESTAT_VOLT_LIM      = 0x00000100,  ///< Voltage limited
	CIA402_ESTAT_POSLIM        = 0x00000200,  ///< Positive limit switch triggered
	CIA402_ESTAT_NEGLIM        = 0x00000400,  ///< Negative limit switch triggered
	CIA402_ESTAT_DISABLE_INPUT = 0x00000800,  ///< Enable input pin not set
	CIA402_ESTAT_SOFT_DISABLE  = 0x00001000,  ///< Disabled due to software request
	CIA402_ESTAT_STOP          = 0x00002000,  ///< Try to stop motor (after disable, before brake)
	CIA402_ESTAT_BRAKE         = 0x00004000,  ///< Brake actuated
	CIA402_ESTAT_PWM_DISABLE   = 0x00008000,  ///< PWM outputs disabled
	CIA402_ESTAT_SOFTLIM_POS   = 0x00010000,  ///< Positive software limit reached
	CIA402_ESTAT_SOFTLIM_NEG   = 0x00020000,  ///< Negative software limit reached
	CIA402_ESTAT_TRK_ERR       = 0x00040000,  ///< Tracking error
	CIA402_ESTAT_TRK_WARN      = 0x00080000,  ///< Tracking warning
	CIA402_ESTAT_RESET         = 0x00100000,  ///< Amplifier has been reset
	CIA402_ESTAT_POSWRAP       = 0x00200000,  ///< Encoder position wrapped (rotory) or hit limit (linear).
	CIA402_ESTAT_FAULT         = 0x00400000,  ///< Latching fault in effect
	CIA402_ESTAT_VEL_LIMIT     = 0x00800000,  ///< Velocity is at limit
	CIA402_ESTAT_ACC_LIMIT     = 0x01000000,  ///< Acceleration is at limit
	CIA402_ESTAT_TRK_WIN       = 0x02000000,  ///< Not in tracking window if set
	CIA402_ESTAT_HOME          = 0x04000000,  ///< Home switch is active
	CIA402_ESTAT_MOVING        = 0x08000000,  ///< Trajectory generator active OR not yet settled
	CIA402_ESTAT_VEL_WIN       = 0x10000000,  ///< Velocity error outside of velocity window when set.
	CIA402_ESTAT_PHASE_INIT    = 0x20000000   ///< Set when using algorithmic phase init mode & phase not initialized.
};


/***************************************************************************/
/**
Latching Amplifier faults conditions.  Once a fault is detected in the
amplifier, the amp will be disabled until the fault condition has been
cleared.

Use Amp::GetFaults to get a list of any active fault conditions, and
Amp::ClearFaults to clear one or more faults.
 */
/***************************************************************************/
//enum CIA402_2183_MAP
//{
//	CIA402_FAULT_DATAFLASH          = 0x0001,   ///< Fatal hardware error: the flash data is corrupt.
//	CIA402_FAULT_ADCOFFSET          = 0x0002,   ///< Fatal hardware error: An A/D offset error has occurred.
//	CIA402_FAULT_SHORT_CRCT         = 0x0004,   ///< The amplifier detected a short circuit condition
//	CIA402_FAULT_AMP_TEMP           = 0x0008,   ///< The amplifier is over temperature
//	CIA402_FAULT_MTR_TEMP           = 0x0010,   ///< A motor temperature error was detected
//	CIA402_FAULT_OVER_VOLT          = 0x0020,   ///< The amplifier bus voltage is over the acceptable limit
//	CIA402_FAULT_UNDER_VOLT         = 0x0040,   ///< The amplifier bus voltage is below the acceptable limit
//	CIA402_FAULT_ENCODER_PWR        = 0x0080,   ///< Over current on the encoder power supply
//	CIA402_FAULT_PHASE_ERR          = 0x0100,   ///< Amplifier phasing error
//	CIA402_FAULT_TRK_ERR            = 0x0200,   ///< Tracking error, the position error is too large.
//	CIA402_FAULT_I2T_ERR            = 0x0400    ///< Current limited by i^2t algorithm.
//};

enum CIA402_ERRORCODE
{
//	CIA402_FAULT_DATAFLASH          = 0x0001,   ///< Fatal hardware error: the flash data is corrupt.
//	CIA402_FAULT_ADCOFFSET          = 0x0002,   ///< Fatal hardware error: An A/D offset error has occurred.
	CIA402_FAULT_SHORT_CRCT         = 0x2320,   ///< The amplifier detected a short circuit condition
	CIA402_FAULT_AMP_TEMP           = 0x4210,   ///< The amplifier is over temperature
	CIA402_FAULT_MTR_TEMP           = 0x4300,   ///< A motor temperature error was detected
	CIA402_FAULT_OVER_VOLT          = 0x3110,   ///< The amplifier bus voltage is over the acceptable limit
	CIA402_FAULT_UNDER_VOLT         = 0x3120,   ///< The amplifier bus voltage is below the acceptable limit
	CIA402_FAULT_ENCODER_PWR        = 0x2280,   ///< Over current on the encoder power supply
	CIA402_FAULT_PHASE_ERR          = 0x7122,   ///< Amplifier phasing error
	CIA402_FAULT_TRK_ERR            = 0x7390,   ///< Tracking error, the position error is too large.
	CIA402_FAULT_I2T_ERR            = 0x2310,   ///< Current limited by i^2t algorithm.
	CIA402_FAULT_POS_LIMIT			= 0x7380,	///< Positive Limit Switch.
	CIA402_FAULT_NEG_LIMIT			= 0x7381	///< Negative Limit Switch.
};

enum CIA402_60FD_MAP
{
	CIA402_NEG_LIMIT		= 0x0001,   ///< Negative limit switch is active when set.
	CIA402_POS_LIMIT        = 0x0002,   ///< Positive limit switch is active when set.
	CIA402_HOME_SWITCH      = 0x0004,   ///< Home switch is active when set.
	CIA402_DRIVE_ENABLE     = 0x0008,   ///< Amplifier enable input is active when set.
};

/****************************************************************
 * CANopen drive status bits (CANopen 402 Object 0x6041)
 *****************************************************************/
enum CIA402_6041_MAP
{
	CIA402_STAT_RTSO		= 0x0001,   // Ready to switch on
	CIA402_STAT_ON          = 0x0002,   // Switched on
	CIA402_STAT_ENA         = 0x0004,   // Operation enabled
	CIA402_STAT_FAULT       = 0x0008,   // A fault has occurred on the drive
	CIA402_STAT_VENA        = 0x0010,   // Voltage enabled
	CIA402_STAT_QSTOP       = 0x0020,   // Doing quick stop when CLEAR
	CIA402_STAT_SOD         = 0x0040,   // Switch-on disabled
	CIA402_STAT_WARN        = 0x0080,   // A warning condition is in effect
	CIA402_STAT_ABORT       = 0x0100,   // Trajectory aborted
	CIA402_STAT_REMOTE      = 0x0200,   // Controlled by CANopen network if set
	CIA402_STAT_MOVEDONE    = 0x0400,   // Target reached
	CIA402_STAT_LIMIT       = 0x0800,   // Internal limit active
	CIA402_STAT_SPACK       = 0x1000,   // pp mode - setpoint acknowledge
	CIA402_STAT_TRACKERR    = 0x2000,   // pp mode - following error
	CIA402_STAT_REFERENCED	= 0x1000,   // home mode - drive has been referenced
	CIA402_STAT_HOMEERR     = 0x2000,   // home mode - homing error
	CIA402_STAT_TRJ         = 0x4000,   // Trajectory active when set
	CIA402_STAT_HOMECAP     = 0x8000   // Home position captured (requires 4.77+ firmware)
};

/***************************************************************************/
/**
Amplifier events.  This enumeration provides a list of events that can be
used to wait on amplifier conditions.
 */
/***************************************************************************/
enum CIA402_EVENT
{
	/// Set when a move is finished and the amplifier has settled in to position
	/// at the end of the move.  Cleared when a new move is started.
	CIA402EVENT_MOVEDONE      = 0x00000001,

	/// Set when the trajectory generator finishes a move.  The motor may not
	/// have settled into position at this point.  Cleared when a new move is
	/// started.
	CIA402EVENT_TRJDONE       = 0x00000002,

	/// A node guarding (or heartbeat) error has occurred.  This indicates that
	/// the amplifier failed to respond within the expected amount of time for
	/// either a heartbeat or node guarding message.  This could be caused by
	/// a network wiring problem, amplifier power down, amp reset, etc.
	/// This bit is set when the error occurs, and is cleared by a call to the
	/// function Amp::ClearNodeGuardEvent.
	CIA402EVENT_NODEGUARD     = 0x00000004,

	/// This event bit is used internally by the amplifier object.  It is set
	/// when the amp acknowledges a new move start.
	CIA402EVENT_SPACK         = 0x00000008,

	/// A latching amplifier fault has occurred.  The specifics of what caused
	/// the fault can be obtained by calling Amp::GetFaults, and the fault conditions
	/// can be cleared by calling Amp::ClearFaults.
	CIA402EVENT_FAULT         = 0x00000010,

	/// A non-latching amplifier error has occurred.
	CIA402EVENT_ERROR         = 0x00000020,

	/// The amplifier's absolute position error is greater then the window
	/// set with Amp::SetPositionWarnWindow.
	CIA402EVENT_POSWARN       = 0x00000040,

	/// The amplifier's absolute position error is greater then the window
	/// set with Amp::SetSettlingWindow.
	CIA402EVENT_POSWIN        = 0x00000080,

	/// The amplifier's absolute velocity error is greater then the window
	/// set with Amp::SetVeliocityWarnWindow
	CIA402EVENT_VELWIN        = 0x00000100,

	/// The amplifier's outputs are disabled.  The reason for the disable
	/// can be determined by Amp::GetEventStatus
	CIA402EVENT_DISABLED      = 0x00000200,

	/// The positive limit switch is currently active
	CIA402EVENT_POSLIM        = 0x00000400,

	/// The negative limit switch is currently active
	CIA402EVENT_NEGLIM        = 0x00000800,

	/// The positive software limit is currently active
	CIA402EVENT_SOFTLIM_POS   = 0x00001000,

	/// The negative software limit is currently active
	CIA402EVENT_SOFTLIM_NEG   = 0x00002000,

	/// The amplifier is presently performing a quick stop sequence
	CIA402EVENT_QUICKSTOP     = 0x00004000,

	/// The last profile was aborted without finishing
	CIA402EVENT_ABORT         = 0x00008000,

	/// The amplifier is software disabled
	CIA402EVENT_SOFTDISABLE   = 0x00010000,

	/// A new home position has been captured.
	/// Note that this features requires firmware version >= 4.77
	CIA402EVENT_HOME_CAPTURE  = 0x00020000,

	/// PVT buffer is empty.
	CIA402EVENT_PVT_EMPTY     = 0x00040000,

	/// Amplifier is currently performing a phase initialization.
	CIA402EVENT_PHASE_INIT    = 0x00080000,

	/// This amplifier's event mask has not yet been initialized.
	/// This event is for internal use only.
	CIA402EVENT_NOT_INIT      = 0x80000000
};


enum CIA402_6086_CODE
{
	/// Velocity profile.  In this profile mode the velocity, acceleration and deceleration
	/// values are used.  The position value is also used, but it only defines the direction
	/// of motion ( positive is position is >= 0, negative if position is < 0 ).
	CIA402PROFILE_VEL = -1,

	/// Trapezoidal profile.  In this profile mode a position, velocity, acceleration
	/// and deceleration may be specified.
	/// This profile mode allows any of it's parameters (position, vel, accel, decel)
	/// to be changed during the course of a move.
	CIA402PROFILE_TRAP = 0,

	/// Jerk limited (S-curve) profile.  In this mode, position, velocity, acceleration,
	/// and jerk (rate of change of acceleration) may be specified.
	CIA402PROFILE_SCURVE = 3
};

enum CIA402_6098_CODE
{
	/// Move into the negative limit switch, then back out to the
	/// first encoder index pulse beyond it.  The index position
	/// is home.
	HM_NLIM_ONDX      = 1,

	/// Move into the positive limit switch, then back out to the
	/// first encoder index pulse beyond it.  The index position
	/// is home.
	HM_PLIM_ONDX      = 2,

	/// Move to a positive home switch, then back out of it to the
	/// first encoder index outside the home region.  The index
	/// position is home.
	HM_PHOME_ONDX     = 3,

	/// Move to a positive home switch, and continue into it to
	/// the first encoder index inside the home region.  The index
	/// position is home.
	HM_PHOME_INDX     = 4,

	/// Move to a negative home switch, then back out of it to the
	/// first encoder index outside the home region.  The index
	/// position is home.
	HM_NHOME_ONDX     = 5,

	/// Move to a negative home switch, and continue into it to
	/// the first encoder index inside the home region.  The index
	/// position is home.
	HM_NHOME_INDX     = 6,

	/// Move to the lower side of a momentary home switch.  Then
	/// find the first encoder index pulse outside the home region.
	/// If the home switch is not active when the home sequence
	/// starts, then the initial move will be positive.
	HM_LHOME_ONDX_POS = 7,

	/// Move to the lower side of a momentary home switch.  Then
	/// find the first encoder index pulse inside the home region.
	/// If the home switch is not active when the home sequence
	/// starts, then the initial move will be positive.
	HM_LHOME_INDX_POS = 8,

	/// Move to the upper side of a momentary home switch.  Then
	/// find the first encoder index pulse inside the home region.
	/// If the home switch is not active when the home sequence
	/// starts, then the initial move will be positive.
	HM_UHOME_INDX_POS = 9,

	/// Move to the upper side of a momentary home switch.  Then
	/// find the first encoder index pulse outside the home region.
	/// If the home switch is not active when the home sequence
	/// starts, then the initial move will be positive.
	HM_UHOME_ONDX_POS = 10,

	/// Move to the upper side of a momentary home switch.  Then
	/// find the first encoder index pulse outside the home region.
	/// If the home switch is not active when the home sequence
	/// starts, then the initial move will be negative.
	HM_UHOME_ONDX_NEG = 11,

	/// Move to the upper side of a momentary home switch.  Then
	/// find the first encoder index pulse inside the home region.
	/// If the home switch is not active when the home sequence
	/// starts, then the initial move will be negative.
	HM_UHOME_INDX_NEG = 12,

	/// Move to the lower side of a momentary home switch.  Then
	/// find the first encoder index pulse inside the home region.
	/// If the home switch is not active when the home sequence
	/// starts, then the initial move will be negative.
	HM_LHOME_INDX_NEG = 13,

	/// Move to the lower side of a momentary home switch.  Then
	/// find the first encoder index pulse outside the home region.
	/// If the home switch is not active when the home sequence
	/// starts, then the initial move will be negative.
	HM_LHOME_ONDX_NEG = 14,

	/// Move into the negative limit switch.  The edge of the
	/// limit is home.
	HM_NLIM           = 17,

	/// Move into the positive limit switch.  The edge of the
	/// limit is home.
	HM_PLIM           = 18,

	/// Move to a positive home switch.  The edge of the home
	/// region is home.
	HM_PHOME          = 19,

	/// Move to a negative home switch.  The edge of the home
	/// region is home.
	HM_NHOME          = 21,

	/// Move to the lower side of a momentary home switch.
	/// The edge of the home region is home.
	/// If the home switch is not active when the home sequence
	/// starts, then the initial move will be positive.
	HM_LHOME_POS      = 23,

	/// Move to the upper side of a momentary home switch.
	/// The edge of the home region is home.
	/// If the home switch is not active when the home sequence
	/// starts, then the initial move will be positive.
	HM_UHOME_POS      = 25,

	/// Move to the upper side of a momentary home switch.
	/// The edge of the home region is home.
	/// If the home switch is not active when the home sequence
	/// starts, then the initial move will be negative.
	HM_UHOME_NEG      = 27,

	/// Move to the lower side of a momentary home switch.
	/// The edge of the home region is home.
	/// If the home switch is not active when the home sequence
	/// starts, then the initial move will be negative.
	HM_LHOME_NEG      = 29,

	/// Move in the negative direction until the first encoder
	/// index pulse is found.  The index position is home.
	HM_NDX_NEG        = 33,

	/// Move in the positive direction until the first encoder
	/// index pulse is found.  The index position is home.
	HM_NDX_POS        = 34,

	/// Set the current position to home.
	HM_NONE           = 35,
};

/***************************************************************************/
/**
This enumeration is used to specify the mode of operation of the amplifier.

The amplifier mode of operation specifies the control method to be used by
the amplifier, as well as the source of input to that control structure.

The amplifier can be controlled in servo mode or in microstepping mode.  When
running in servo mode the amplifier uses up to three nested control loops.
These loops control current, velocity and position.  In microstepping mode
the low level current loop is retained, but the upper level loops are replaced
with a simple position command.

The command source of the amplifier will normally be the CANopen network itself.
However, the amplifier also supports several low level control methods in which
commands are received through analog or digital input pins, or even from an
internal function generator.

Normally, only the CANopen modes of operation will be used when running over
the CANopen network.  These modes are AMPMODE_CAN_PROFILE, AMPMODE_CAN_VELOCITY,
AMPMODE_CAN_HOMING, and AMPMODE_CAN_PVT.  Each of these modes can be used on
either servo or microstepping drives.  It's typically not necessary to
specify the type of control method (servo or microstepping) to be used with
these modes as it can be determined by the type of amplifier being used.  Servo
amplifier's (such as Accelnet) default to servo mode, and microstepping amplifiers
(such as Stepnet) will default to microstepping mode.  If this default is not
appropriate for the application, then the control method may be forced by ORing
in one of the following two values; AMPMODE_CAN_SERVO and AMPMODE_CAN_USTEP.

 */
/***************************************************************************/
enum CIA402_6060_CODE
{
	CIA402MODE_DISABLED         = 0,

	/// In this mode the CANopen network sends move commands to the amplifier,
	/// and the amplifier uses it's internal trajectory generator to perform
	/// the moves.
	/// This mode conforms to the CANopen device profile for motion control
	/// (DSP-402) profile position mode.
	CIA402MODE_CAN_PP      = 0x01,

	/// In this mode the CANopen network commands target velocity values to the
	/// amplifier.  The amplifier uses it's programmed acceleration and deceleration
	/// values to ramp the velocity up/down to the target.
	///
	/// Note that support for profile velocity mode was added in amplifier firmware
	/// version 3.06.  Earlier versions of firmware will report an error if this
	/// mode is selected.
	CIA402MODE_CAN_PV     = 0x03,

	/// In this mode the CANopen network commands torque values to the amplifier.
	///
	/// Note that support for profile torque mode was added in amplifier firmware
	/// version 3.34.  Earlier versions of firmware will report an error if this
	/// mode is selected.
	CIA402MODE_CAN_PT       = 0x04,

	/// This mode is used to home the motor (i.e. find the motor zero position)
	/// under the control of the CANopen network.
	/// This mode conforms to the CANopen device profile for motion control
	/// (DSP-402) homing mode.
	CIA402MODE_CAN_HOMING       = 0x06,

	/// In this mode the CANopen master calculates the motor trajectory and
	/// streams it over the CANopen network as a set of points that the
	/// amplifier interpolated between.
	/// This mode conforms to the CANopen device profile for motion control
	/// (DSP-402) interpolated position mode.
	CIA402MODE_CAN_PVT          = 0x07,

	CIA402MODE_CAN_CSP          = 0x08,
	CIA402MODE_CAN_CSV        	= 0x09,
	CIA402MODE_CAN_CST          = 0x0A,

	CIA402MODE_PROG_VEL         = 0x0B,
};


/***************************************************************************/
/**
Input pin configuration settings.  The digital input pins located on an
amplifier can be programmed to perform some action.  This enumeration
provides a list of the possible settings for an input pin.

Note that it is perfectly legal to program more then one input pin to
perform the same action.  It's often useful to have two hardware disable
inputs for example.  If either of these inputs becomes active, the amplifier
will be disabled.

Whether the inputs are configured to perform some action or not, it's still
possible to read them directly using the Amp::GetInputs function.
 */
/***************************************************************************/
enum CIA402_2192_CODE
{
	CIA402INCFG_NONE                  = 0x0000, ///< No function assigned to the input
	CIA402INCFG_RESET_R               = 0x0002, ///< Reset the amplifier on the Rising edge of the input
	CIA402INCFG_RESET_F               = 0x0003, ///< Reset the amplifier on the Falling edge of the input
	CIA402INCFG_POSLIM_H              = 0x0004, ///< Positive limit switch, active High.
	CIA402INCFG_POSLIM_L              = 0x0005, ///< Positive limit switch, active Low.
	CIA402INCFG_NEGLIM_H              = 0x0006, ///< Negative limit switch, active High.
	CIA402INCFG_NEGLIM_L              = 0x0007, ///< Negative limit switch, active Low.
	CIA402INCFG_MOTOR_TEMP_H          = 0x0008, ///< Motor temp sensor active high
	CIA402INCFG_MOTOR_TEMP_L          = 0x0009, ///< Motor temp sensor active low
	CIA402INCFG_CLR_FAULTS_H          = 0x000A, ///< Clear faults on edge, disable while high
	CIA402INCFG_CLR_FAULTS_L          = 0x000B, ///< Clear faults on edge, disable while low
	CIA402INCFG_RESET_DISABLE_R       = 0x000C, ///< Reset on rising edge, disable while high
	CIA402INCFG_RESET_DISABLE_F       = 0x000D, ///< Reset on falling edge, disable while low
	CIA402INCFG_HOME_H                = 0x000E, ///< Home switch, active high
	CIA402INCFG_HOME_L                = 0x000F, ///< Home switch, active low
	CIA402INCFG_DISABLE_H             = 0x0010, ///< Amplifier disable active high
	CIA402INCFG_DISABLE_L             = 0x0011, ///< Amplifier disable active low
	CIA402INCFG_PWM_SYNC_H            = 0x0013, ///< Sync input on falling edge, valid only on high speed inputs
	CIA402INCFG_MOTION_ABORT_H        = 0x0014, ///< Abort motion active high
	CIA402INCFG_MOTION_ABORT_L        = 0x0015, ///< Abort motion active low
	CIA402INCFG_SCALE_ADC_H           = 0x0016, ///< Scale analog reference input by a factor of 8 when high
	CIA402INCFG_SCALE_ADC_L           = 0x0017, ///< Scale analog reference input by a factor of 8 when low
	CIA402INCFG_HIGHSPEED_CAPTURE_R   = 0x0018, ///< High speed position capture on rising edge
	CIA402INCFG_HIGHSPEED_CAPTURE_F   = 0x0019, ///< High speed position capture on falling edge
	CIA402INCFG_COUNT_EDGES_R         = 0x001A, ///< Count rising edges of input, store the results to an indexer register
	CIA402INCFG_COUNT_EDGES_F         = 0x001B, ///< Count falling edges of input, store the results to an indexer register
	CIA402INCFG_ABORT_WINDOW_R        = 0x0024, ///< Abort move on rising edge if not within N counts of destination position.
	CIA402INCFG_ABORT_WINDOW_F        = 0x0025  ///< Abort move on falling edge if not within N counts of destination position.
};


enum CIA402_OUTPUT_CONFIG
{
	CIA402OUTCFG_EVENT_STATUS_L       = 0x0000,
	CIA402OUTCFG_EVENT_STATUS_H       = 0x0100,
	CIA402OUTCFG_EVENT_LATCH_L        = 0x0001,
	CIA402OUTCFG_EVENT_LATCH_H        = 0x0101,
	CIA402OUTCFG_MANUAL_L             = 0x0002,
	CIA402OUTCFG_MANUAL_H             = 0x0102,
	CIA402OUTCFG_TRJ_STATUS           = 0x0003,
	CIA402OUTCFG_POSITION_WINDOW      = 0x0004,
	CIA402OUTCFG_POSITION_TRIG_LOW2HIGH = 0x0005,
	CIA402OUTCFG_POSITION_TRIG_HIGH2LOW = 0x0006,
	CIA402OUTCFG_POSITION_TRIG        = 0x0007,
	CIA402OUTCFG_POSITION_TRIG_LIST   = 0x0009,
	CIA402OUTCFG_SYNC_OUTPUT	       = 0x0200,
	CIA402OUTCFG_ACTIVE_HIGH          = 0x0100
};


/***************************************************************************/
/**
The amplifier's halt mode defines it's action when a halt command is issued
(Amp::HaltMove).  When the halt command is issued, the move in progress will
be terminated using the method defined in this mode.  Unless the HALT_DISABLE
method is selected, the amplifier will remain enabled and holding position
at the end of the halt sequence.
 */
/***************************************************************************/
enum CIA402_H_MODE
{
	CIA402HALT_DISABLE             = 0,        ///< Disable the amplifier immediately
	CIA402HALT_DECEL               = 1,        ///< Slow down using the profile deceleration
	CIA402HALT_QUICKSTOP           = 2,        ///< Slow down using the quick stop deceleration
	CIA402HALT_ABRUPT              = 3         ///< Slow down with unlimited deceleration
};

/***************************************************************************/
/**
The amplifier's quick stop mode defines it's action when a quick stop command
is issued (Amp::QuickStop).

The quick stop command differs from the halt command in that the amplifier
is always disabled at the end of the sequence.  For some modes, the amplifier
automatically disables after halting the move.  For others, the amplifier
halts the move and holds in the quick stop state.  No new moves may be started
until the amplifier has manually been disabled.
 */
/***************************************************************************/
enum CIA402_QUICK_STOP_MODE
{
	CIA402QSTOP_DISABLE            = 0,        ///< Disable the amplifier immediately
	CIA402QSTOP_DECEL              = 1,        ///< Slow down using the profile deceleration then disable
	CIA402QSTOP_QUICKSTOP          = 2,        ///< Slow down using the quick stop deceleration then disable
	CIA402QSTOP_ABRUPT             = 3,        ///< Slow down with unlimited deceleration then disable
	CIA402QSTOP_DECEL_HOLD         = 5,        ///< Slow down and hold
	CIA402QSTOP_QUICKSTOP_HOLD     = 6,        ///< Quick stop and hold
	CIA402QSTOP_ABRUPT_HOLD        = 7         ///< Abrupt stop and hold
};

CML_NAMESPACE_END()
#endif //_CIA402DEF_
