#ifndef _CIA402_
#define _CIA402_

#include "CML_Device.h"
#include "CML_Settings.h"
#include "CML_Node.h"
#include "CML_CiA301.h"
#include "CML_CiA402Def.h"
#include "CML_CiA402Stc.h"
CML_NAMESPACE_START()

class LinkEqpt;


class TPDO_PDO1: public TPDO
{
	// These variables are used to map objects to the PDO.
	// Different mapping objects are used for different data sizes.
	// Position is a 32-bit value, analog input reading is 16-bit
	// A maximum of 8-bytes can be mapped to any PDO.
	//PDO1 member
	Pmap32 actual_vel;
	Pmap32 target_vel;
public:

	// Default constructor does nothing
	TPDO_PDO1(){}

	// Called once at startup to map the PDO and configure CML to 
	// listen for it.
	const Error *Init( Node* node, int slot, int canID );

	// This function is called when the PDO is received
	virtual void Received( void );
	int32 getActualVel(){return actual_vel.Read();}
	int32 getTargetVel(){return target_vel.Read();}


};

class CiA402: public Device
{
public:
	CiA402( void );
	CiA402( MDNode &node, uint8 devID );
	CiA402( MDNode &node, uint8 devID, CiA402Settings &settings );
	virtual ~CiA402();

	virtual const Error *Init( MDNode &node, uint8 devID );
	virtual const Error *Init( MDNode &node, uint8 devID, CiA402Settings &settings );
	virtual const Error *UnInit( void );

	//	const Error *StartStatusPDO(uint16 slot);
	//	const Error *StopStatusPDO();
	const Error *WaitPDOEvent( PDO_EVENT event, Timeout timeout=-1 );
	//	const Error *WaitSyncPDOEvent( uint32 event, Timeout timeout=-1 );
	const Error *WaitEvent( Event &e, Timeout timeout=-1 );
	const Error *WaitEvent( Event &e, Timeout timeout, CIA402_EVENT &match );

	const Error *SetOperationMode(int8 mode);
	//	const Error *GetAmpMode( CIA402_MODE &mode );
	const Error *Disable( bool wait=true );
	const Error *Enable( bool wait=true );

	const Error *GetPositionActual( int32 &value );
	const Error *GetVelocityActual( int32 &value );
	const Error *GetVelocityCommand( int32 &value );
	const Error *GetCurrentRate(int32 &value);
	const Error *GetCurrentActual( int16 &value );
	const Error *GetDriverTemp( int16 &value );

	//	const Error *GetStatusWord( uint16 &value );
	const Error *SetControlWord( uint16 value );
	//	const Error *GetControlWord( uint16 &value );

	const Error *GetPdoEventMask( PDO_EVENT &e );
	const Error *GetEventMask( CIA402_EVENT &e );
	const Error *GetErrorStatus( bool noComm=false );
	const Error *GetErrorCode(int32 &error);

	const Error *ClearEventLatch( CIA402_EVENT_STATUS stat );
	const Error *ClearFaults( void );
	const Error *GetFaultsCnt( uint8 &value );
	const Error *GetFault( uint32 &value, uint8 idx );
	const Error *ClearErrHistory();

	/***************************************************************************/
	/** \name Input & Output pin control
	 */
	/***************************************************************************/
	//@{
	const Error *SetIoConfig( IoCfg &cfg );
	const Error *GetIoConfig( IoCfg &cfg );
	const Error *SetInputConfig( int8 pin, CIA402_2192_CODE cfg );
	const Error *GetInputConfig( int8 pin, CIA402_2192_CODE &cfg );
	//	const Error *SetOutputConfig( int8 pin, CIA402_OUTPUT_CONFIG cfg );
	//	const Error *GetOutputConfig( int8 pin, CIA402_OUTPUT_CONFIG &cfg );
	const Error *GetInputs( uint16 &value, bool viaSDO = true );
	const Error *GetInputCnt( uint8 &value );
	const Error *WaitInputEvent( Event &e, Timeout timeout, uint32 &match );
//	const Error *WaitInputHigh( uint32 inputs, Timeout timeout = -1 );
//	const Error *WaitInputLow( uint32 inputs, Timeout timeout = -1 );
	//@}

	/***************************************************************************/
	/** \name Control loop setup
	 */
	/***************************************************************************/
	//@{
	const Error *GetVelLoopConfig( VelLoopCfg &cfg );
	const Error *SetVelLoopConfig( VelLoopCfg &cfg );
	const Error *GetCrntLoopConfig( CrntLoopCfg &cfg );
	const Error *SetCrntLoopConfig( CrntLoopCfg &cfg );

	const Error *GetVelLoopP( uint16 &value );
	const Error *SetVelLoopP( uint16 value );
	const Error *GetVelLoopI( uint16 &value );
	const Error *SetVelLoopI( uint16 value );
	const Error *GetVelLoopMax( int32 &value );
	const Error *SetVelLoopMax( int32 value );

	const Error *SetProfileAcc( int32 value );
	const Error *GetProfileAcc( int32 &value );
	const Error *SetProfileDec( int32 value );
	const Error *GetProfileDec( int32 &value );

	const Error *GetCrntLoopP( uint16 &value );
	const Error *SetCrntLoopP( uint16 value );
	const Error *GetCrntLoopI( uint16 &value );
	const Error *SetCrntLoopI( uint16 value );

	const Error *SetCrntOffsetA( int16 value );
	const Error *GetCrntOffsetA( int16 &value );
	const Error *SetCrntOffsetB( int16 value );
	const Error *GetCrntOffsetB( int16 &value );

	const Error *SetIitPeak( int16 value );
	const Error *GetIitPeak( int16 &value );
	const Error *SetIitCont( int16 value );
	const Error *GetIitCont( int16 &value );
	const Error *SetIitTime( uint16 value );
	const Error *GetIitTime( uint16 &value );

	const Error *SetProfileType( CIA402_6086_CODE type );
	const Error *GetProfileType( CIA402_6086_CODE &type );
	//@}


	/***************************************************************************/
	/** \name Homing mode.
	 */
	/***************************************************************************/
	//	@{
	const Error *GoHome( void );
	const Error *GoHome( HomeCfg &cfg );
	const Error *SetHomeConfig( HomeCfg &cfg );
	const Error *GetHomeConfig( HomeCfg &cfg );
	const Error *SetHomeMethod( CIA402_6098_CODE method );
	const Error *GetHomeMethod( CIA402_6098_CODE &method );
	const Error *SetHomeOffset( int32 value  );
	const Error *GetHomeOffset( int32 &value );
	//	@}


	/***************************************************************************/
	/** \name General parameter setup.
	 */
	/***************************************************************************/
	//@{
	const Error *GetCiA402Config( CiA402Cfg &cfg );
	const Error *SetCiA402Config( CiA402Cfg &cfg );
	//	const Error *SaveAmpConfig( AmpConfig &cfg );
	//	const Error *SaveAmpConfig( void );
	//	const Error *LoadFromFile( const char *name, int &line );

	//	const Error *GetMtrInfo( MtrCfg &cfg );
	//	const Error *SetMtrInfo( MtrCfg &cfg );
	//@}

	/***************************************************************************/
	/** \name Position and velocity windows
	 */
	/***************************************************************************/
	//@{
	const Error *SetTrackingWindows( TrackWinCfg &cfg );
	const Error *GetTrackingWindows( TrackWinCfg &cfg );
	const Error *SetVelWindow( uint16 value );
	const Error *GetVelWindow( uint16 &value );
	const Error *SetVelWinTime( uint16 value );
	const Error *GetVelWinTime( uint16 &value );
	//@}

	const Error *SetTargetVel( int32 value );
	const Error *GetTargetVel( int32 &value );

	const Error *GetPositionWindow( int32 &value );
	const Error *SetPositionWindow( int32 value);
	const Error *GetPositionWindowTime( int32 &value );
	const Error *SetPositionWindowTime( int32 value);

	const Error *SetTargetPos( int32 value);
	const Error *GetTargetPos( int32 &value);
	const Error *GetTargetPos2( int32 &value);
	const Error* SetProfileVel( int32 value);

	const Error *SetQuickStop( CIA402_QUICK_STOP_MODE mode );
	const Error *SetHaltMode( CIA402_H_MODE mode );

	//	virtual const Error *SetCountsPerUnit( uunit cts );
	//	virtual const Error *GetCountsPerUnit( uunit &cts );

	void GetFactorConfig( FactorCfg &cfg );
	void SetFactorConfig( FactorCfg &cfg );

	void SetPosFctNum( uint32 num );
	void GetPosFctNum( uint32 &num );
	void SetPosFctDiv( uint32 div );
	void GetPosFctDiv( uint32 &div );

	void SetVelFctNum( uint32 num );
	void GetVelFctNum( uint32 &num );
	void SetVelFctDiv( uint32 div );
	void GetVelFctDiv( uint32 &div );

	void SetAccFctNum( uint32 num );
	void GetAccFctNum( uint32 &num );
	void SetAccFctDiv( uint32 div );
	void GetAccFctDiv( uint32 &div );

	void SetPolarity( uint8 val );
	void GetPolarity( uint8 &val );
	int32 RevPosition( int32 pos );
	int32 RevVelocity( int32 vel );
	void SetPosPolarity( bool val );
	bool GetPosPolarity( void );
	void SetVelPolarity( bool val );
	bool GetVelPolarity( void );

	int32 PosUser2Intl( int32 pos );
	int32 VelUser2Intl( int32 vel );
	int32 AccUser2Intl( int32 acc );
	int32 PosIntl2User( int32 pos );
	int32 VelIntl2User( int32 vel );
	int32 AccIntl2User( int32 acc );

    const Error *PosIsReached(bool& b);
	virtual void PostPDOEvent( PDO_EVENT event )
	{
		pdoMap_.setBits( (uint32)event );
		pdoMap_.clrBits( (uint32)event );
	}

	TPDO_Data dataTPDO_[3];
	friend class TPDO_Data;
	RPDO_Data dataRPDO_[3];
	friend class RPDO_Data;
	TPDO_402Status statusPDO_;
	friend class TPDO_402Status;
	TPDO_PDO1 tpdo1;

protected:
	EventMap cia402Event_;
	EventMap inputEvent_;
	EventMap pdoMap_;

	void MoveAborted( void );


private:
#ifdef CML_ENABLE_USER_UNITS
	FactorCfg factor_;
	// Load encoder unit conversion scaling factors
	//	double u2lPos; ///< Used to convert position from user units to load units
	//	double u2lVel; ///< Used to convert velocity from user units to load units
	//	double u2lAcc; ///< Used to convert acceleration from user units to load units
	//	double u2lJrk; ///< Used to convert jerk from user units to load units
	//	double l2uPos; ///< Used to convert position from load units to user units
	//	double l2uVel; ///< Used to convert velocity from load units to user units
	//	double l2uAcc; ///< Used to convert acceleration from load units to user units
	//	double l2uJrk; ///< Used to convert jerk from user load to user units

	// Motor encoder unit conversion scaling factors
	//	double u2mPos; ///< Used to convert position from user units to motor units
	//	double u2mVel; ///< Used to convert velocity from user units to motor units
	//	double u2mAcc; ///< Used to convert acceleration from user units to motor units
	//	double m2uPos; ///< Used to convert position from motor units to user units
	//	double m2uVel; ///< Used to convert velocity from motor units to user units
	//	double m2uAcc; ///< Used to convert acceleration from motor units to user units
#endif

	CiA402( const CiA402& );
	CiA402& operator=( const CiA402& );
};





CML_NAMESPACE_END()
#endif //_CIA402_
