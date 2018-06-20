#ifndef _CIA301_
#define _CIA301_

#include "CML_PDO.h"
#include "CML_Device.h"

CML_NAMESPACE_START()

class TPDO_Data: public TPDO
{
	//	uint32 devRef_;	 ///< Points to the amplifier to which this PDO belongs
	class Device *device_;
	PDO_EVENT eventMask_;
	Pmap32 data32_[2];
	Pmap16 data16_[4];
	int i16, i32;

	uint16 slot_;
	Mutex mtx_;

public:
	/// Default constructor for this PDO
	TPDO_Data();
	~TPDO_Data();
	const Error *Init( Device &device );
	const Error *AddMap(uint32 pdoMap);
	const Error *Start(uint16 slot, byte type = 1);
	const Error *Stop();

	virtual int16 GetData16(uint32 mapCode);
	virtual int32 GetData32(uint32 mapCode);

private:
	void Received( void );
	TPDO_Data( const TPDO_Data& );
	TPDO_Data& operator=( const TPDO_Data& );
};


class RPDO_Data: public RPDO
{
	class Device *device_;
	Pmap32 data32_[2];
	Pmap16 data16_[4];
	int i16, i32;

	uint16 slot_;
	Mutex mtx_;

public:
	/// Default constructor for this PDO
	RPDO_Data();
	~RPDO_Data();
	const Error *Init( Device &device, uint16 slot );
	const Error *AddMap(uint32 pdoMap);
	const Error *Start(byte type = 0xff);
	const Error *Stop();

	void SetData16(uint32 mapCode, int16 val);
	void SetData32(uint32 mapCode, int32 val);

	const Error *Transmit( void );

private:
	RPDO_Data( const RPDO_Data& );
	RPDO_Data& operator=( const RPDO_Data& );
};

CML_NAMESPACE_END()
#endif //_CIA301_

