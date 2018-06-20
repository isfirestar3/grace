#ifndef _CIA401_
#define _CIA401_

#include "CML_Device.h"
#include "CML_Settings.h"
#include "CML_Node.h"
#include "CML_PDO.h"
#include "CML_CiA401Def.h"
#include "CML_CiA401Stc.h"
CML_NAMESPACE_START()


class CiA401: public Device
{

public:
	CiA401( void );
	CiA401( MDNode &node, uint8 devID );
	CiA401( MDNode &node, uint8 devID, CiA401Settings &settings );
	virtual ~CiA401();

	virtual const Error *Init( MDNode &node, uint8 devID );
	virtual const Error *Init( MDNode &node, uint8 devID, CiA401Settings &settings );

	virtual const Error *WaitIOEvent( CIA401_EVENTS event, Timeout timeout=-1 );
	virtual const Error *WaitIOEvent( Event &e, Timeout timeout, CIA401_EVENTS &match );

	/***************************************************************************/
	/** \name Digital input control

     If the module contains digital inputs, these methods may be used to configure
     and read those inputs.  The inputs may be read and controlled individually,
     or in groups of 8, 16 or 32 inputs.

     All I/O modules should support access to digital inputs in groups of 8.
     Support for individual access or different groupings is optional under the
     spec.  If a particular device does not support such groupings, an attempt
     to use them should return an error code.

     Each input pin or group of pins is assigned an ID number used to access it.
     When single inputs are accessed, these ID numbers range from 0 (the first input)
     to N-1 (the last input), where N is the total number of input pins available
     on the module.

     When groups of inputs are accessed as a unit, the group is assigned a number.
     The first group of inputs will be assigned ID number 0, the second will be ID 1,
     etc.  The number of groups of a particular size will be the total number of inputs
     divided by the group size.

     For example, to access the fifty third input pin individually you would use id
     number 52.  To access it as part of a group of 8 inputs, you would access group
     number 6 (52/8).  Input 52 would be bit 4 (52%8) of that group.
	 */
	/***************************************************************************/
	//@{

	/// Get the current setting of the global interrupt enable for digital inputs.
	/// A return value of true indicates that interrupts are enabled, false disabled.
	/// @param value The current interrupt enable setting is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DinGetIntEna( bool &value ){
		uint8 v;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		const Error *err = node->sdo.Upld8( CIA401_DIN_INTENA, 0, v );
		value = (v!=0);
		return err;
	}

	/// Set the current setting of the global interrupt enable for digital inputs.
	/// Setting this parameter to true enables interrupts, false disables.
	/// @param value The interrupt enable setting.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DinSetIntEna( bool value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		const Error *err = node->sdo.Dnld8( CIA401_DIN_INTENA, 0, (uint8)value );
		if( !err ) dinIntEna = value;
		return err;
	}

	/// Return the number of individual inputs available on this device.
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DinGetCt( uint16 &ct ){
		ct = 0;
		return BitCount( CIA401_DIN_1_VALUE, ct );
	}

	virtual const Error *DinRead( uint16 id, bool &value, bool viaSDO=false );

	/// Get the current polarity settings for a digital input.
	/// Polarity inversion is enabled if true, disabled if false.
	/// @param id Identifies the digital input.
	/// @param value The current polarity setting.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DinGetPol( uint16 id, bool &value ){
		return BitUpld( CIA401_DIN_1_POL, id, value );
	}

	/// Set the current polarity setting for a digital input.
	/// Polarity inversion is enabled if true, disabled if false.
	/// @param id Identifies the digital input.
	/// @param value The new polarity setting.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DinSetPol( uint16 id, bool value ){
		return BitDnld( CIA401_DIN_1_POL, id, value );
	}

	/// Get the current filter constant setting for a digital input.
	/// The filter constant is enabled if true, disabled if false.
	/// @param id Identifies the digital input.
	/// @param value The current filter setting.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DinGetFilt( uint16 id, bool &value ){
		return BitUpld( CIA401_DIN_1_FILT, id, value );
	}

	/// Set the current filter constant setting for a digital input.
	/// The filter constant is enabled if true, disabled if false.
	/// @param id Identifies the digital input.
	/// @param value The new filter setting.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DinSetFilt( uint16 id, bool value ){
		return BitDnld( CIA401_DIN_1_FILT, id, value );
	}

	/// Get the 'any transition' interrupt mask settings for a digital input.
	/// If true, any transition on the input will generate an interrupt.
	/// @param id Identifies the digital input.
	/// @param value The current interrupt mask setting
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DinGetMaskAny( uint16 id, bool &value ){
		return BitUpld( CIA401_DIN_1_MASK_ANY, id, value );
	}

	/// Set the 'any transition' interrupt mask settings for a digital input.
	/// If true, any transition on the input will generate an interrupt.
	/// @param id Identifies the digital input.
	/// @param value The new interrupt mask setting.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DinSetMaskAny( uint16 id, bool value ){
		return BitDnld( CIA401_DIN_1_MASK_ANY, id, value );
	}

	/// Get the 'low to high' interrupt mask settings for a digital input.
	/// If true, a low to high transition on the input will generate an interrupt.
	/// @param id Identifies the digital input.
	/// @param value The current interrupt mask setting
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DinGetMaskLow2High( uint16 id, bool &value ){
		return BitUpld( CIA401_DIN_1_MASK_L2H, id, value );
	}

	/// Set the 'low to high' interrupt mask settings for a digital input.
	/// If true, a low to high transition on the input will generate an interrupt.
	/// @param id Identifies the digital input.
	/// @param value The new interrupt mask setting.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DinSetMaskLow2High( uint16 id, bool value ){
		return BitDnld( CIA401_DIN_1_MASK_L2H, id, value );
	}

	/// Get the 'high to low' interrupt mask settings for a digital input.
	/// If true, a high to low transition on the input will generate an interrupt.
	/// @param id Identifies the digital input.
	/// @param value The current interrupt mask setting
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DinGetMaskHigh2Low( uint16 id, bool &value ){
		return BitUpld( CIA401_DIN_1_MASK_H2L, id, value );
	}

	/// Set the 'high to low' interrupt mask settings for a digital input.
	/// If true, a high to low transition on the input will generate an interrupt.
	/// @param id Identifies the digital input.
	/// @param value The new interrupt mask setting.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DinSetMaskHigh2Low( uint16 id, bool value ){
		return BitDnld( CIA401_DIN_1_MASK_H2L, id, value );
	}

	/// Return the number of 8-bit groups of inputs available on this device.
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din8GetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DIN_8_VALUE, 0, ct );
	}

	virtual const Error *Din8Read( uint8 id, uint8 &value, bool viaSDO=false );

	/// Get the current polarity settings for a group of 8 digital inputs.
	/// For each input in the group, a value of 1 enables inversion and 0 disables.
	/// @param id Identifies which group of 8 inputs to read.
	/// @param value The current polarity setting of the 8 input lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din8GetPol( uint8 id, uint8 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DIN_8_POL, id+1, value );
	}

	/// Set the current polarity setting for a group of 8 digital inputs.
	/// For each input in the group, a value of 1 enables inversion and 0 disables.
	/// @param id Identifies which group of 8 inputs to effect.
	/// @param value The new polarity setting of the 8 input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din8SetPol( uint8 id, uint8 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld8( CIA401_DIN_8_POL, id+1, value );
	}

	/// Get the current filter constant settings for a group of 8 digital inputs.
	/// For each input in the group, a value of 1 enables the filter, 0 disables.
	/// @param id Identifies which group of 8 inputs to read.
	/// @param value The current filter setting of the 8 input lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din8GetFilt( uint8 id, uint8 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DIN_8_FILT, id+1, value );
	}

	/// Set the current filter constant setting for a group of 8 digital inputs.
	/// For each input in the group, a value of 1 enables the filter, 0 disables.
	/// @param id Identifies which group of 8 inputs to effect.
	/// @param value The new filter setting of the 8 input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din8SetFilt( uint8 id, uint8 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld8( CIA401_DIN_8_FILT, id+1, value );
	}

	/// Get the 'any transition' interrupt mask settings for a group of 8 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on any change,
	/// and a value of 0 disables the interrupt.
	/// @param id Identifies which group of 8 inputs to read.
	/// @param value The current interrupt mask setting of the 8 input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din8GetMaskAny( uint8 id, uint8 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DIN_8_MASK_ANY, id+1, value );
	}

	/// Set the 'any transition' interrupt mask settings for a group of 8 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on any transition,
	/// and a value of 0 disables.
	/// @param id Identifies which group of 8 inputs to effect.
	/// @param value The new interrupt mask value.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din8SetMaskAny( uint8 id, uint8 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld8( CIA401_DIN_8_MASK_ANY, id+1, value );
	}

	/// Get the 'low to high' interrupt mask settings for a group of 8 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on a low to high
	/// transition, and a value of 0 disables the interrupt.
	/// @param id Identifies which group of 8 inputs to read.
	/// @param value The current interrupt mask setting of the 8 input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din8GetMaskLow2High( uint8 id, uint8 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DIN_8_MASK_L2H, id+1, value );
	}

	/// Set the 'low to high' interrupt mask settings for a group of 8 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on low to high
	/// transitions, and a value of 0 disables.
	/// @param id Identifies which group of 8 inputs to effect.
	/// @param value The new interrupt mask value.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din8SetMaskLow2High( uint8 id, uint8 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld8( CIA401_DIN_8_MASK_L2H, id+1, value );
	}

	/// Get the 'high to low' interrupt mask settings for a group of 8 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on a high to low
	/// transition, and a value of 0 disables the interrupt.
	/// @param id Identifies which group of 8 inputs to read.
	/// @param value The current interrupt mask setting of the 8 input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din8GetMaskHigh2Low( uint8 id, uint8 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DIN_8_MASK_H2L, id+1, value );
	}

	/// Set the 'high to low' interrupt mask settings for a group of 8 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on high to low
	/// transitions, and a value of 0 disables.
	/// @param id Identifies which group of 8 inputs to effect.
	/// @param value The new interrupt mask value.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din8SetMaskHigh2Low( uint8 id, uint8 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld8( CIA401_DIN_8_MASK_H2L, id+1, value );
	}

	/// Return the number of 16-bit groups of inputs available on this device.
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din16GetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DIN_16_VALUE, 0, ct );
	}

	virtual const Error *Din16Read( uint8 id, uint16 &value, bool viaSDO=false );

	/// Get the current polarity settings for a group of 16 digital inputs.
	/// For each input in the group, a value of 1 enables inversion and 0 disables.
	/// @param id Identifies which group of inputs to read.
	/// @param value The current polarity setting of the input lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din16GetPol( uint8 id, uint16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_DIN_16_POL, id+1, value );
	}

	/// Set the current polarity setting for a group of 16 digital inputs.
	/// For each input in the group, a value of 1 enables inversion and 0 disables.
	/// @param id Identifies which group of inputs to effect.
	/// @param value The new polarity setting of the input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din16SetPol( uint8 id, uint16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_DIN_16_POL, id+1, value );
	}

	/// Get the current filter constant settings for a group of 16 digital inputs.
	/// For each input in the group, a value of 1 enables the filter, 0 disables.
	/// @param id Identifies which group of inputs to read.
	/// @param value The current filter setting of the input lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din16GetFilt( uint8 id, uint16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_DIN_16_FILT, id+1, value );
	}

	/// Set the current filter constant setting for a group of 16 digital inputs.
	/// For each input in the group, a value of 1 enables the filter, 0 disables.
	/// @param id Identifies which group of inputs to effect.
	/// @param value The new filter setting of the input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din16SetFilt( uint8 id, uint16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_DIN_16_FILT, id+1, value );
	}

	/// Get the 'any transition' interrupt mask settings for a group of 16 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on any change,
	/// and a value of 0 disables the interrupt.
	/// @param id Identifies which group of inputs to read.
	/// @param value The current interrupt mask setting of the input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din16GetMaskAny( uint8 id, uint16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_DIN_16_MASK_ANY, id+1, value );
	}

	/// Set the 'any transition' interrupt mask settings for a group of 16 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on any transition,
	/// and a value of 0 disables.
	/// @param id Identifies which group of inputs to effect.
	/// @param value The new interrupt mask value.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din16SetMaskAny( uint8 id, uint16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_DIN_16_MASK_ANY, id+1, value );
	}

	/// Get the 'low to high' interrupt mask settings for a group of 16 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on a low to high
	/// transition, and a value of 0 disables the interrupt.
	/// @param id Identifies which group of inputs to read.
	/// @param value The current interrupt mask setting of the input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din16GetMaskLow2High( uint8 id, uint16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_DIN_16_MASK_L2H, id+1, value );
	}

	/// Set the 'low to high' interrupt mask settings for a group of 16 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on low to high
	/// transitions, and a value of 0 disables.
	/// @param id Identifies which group of inputs to effect.
	/// @param value The new interrupt mask value.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din16SetMaskLow2High( uint8 id, uint16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_DIN_16_MASK_L2H, id+1, value );
	}

	/// Get the 'high to low' interrupt mask settings for a group of 16 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on a high to low
	/// transition, and a value of 0 disables the interrupt.
	/// @param id Identifies which group of inputs to read.
	/// @param value The current interrupt mask setting of the input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din16GetMaskHigh2Low( uint8 id, uint16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_DIN_16_MASK_H2L, id+1, value );
	}

	/// Set the 'high to low' interrupt mask settings for a group of 16 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on high to low
	/// transitions, and a value of 0 disables.
	/// @param id Identifies which group of inputs to effect.
	/// @param value The new interrupt mask value.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din16SetMaskHigh2Low( uint8 id, uint16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_DIN_16_MASK_H2L, id+1, value );
	}

	/// Return the number of 32-bit groups of inputs available on this device.
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din32GetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DIN_32_VALUE, 0, ct );
	}

	virtual const Error *Din32Read( uint8 id, uint32 &value, bool viaSDO=false );

	/// Get the current polarity settings for a group of 32 digital inputs.
	/// For each input in the group, a value of 1 enables inversion and 0 disables.
	/// @param id Identifies which group of inputs to read.
	/// @param value The current polarity setting of the input lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din32GetPol( uint8 id, uint32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_DIN_32_POL, id+1, value );
	}

	/// Set the current polarity setting for a group of 32 digital inputs.
	/// For each input in the group, a value of 1 enables inversion and 0 disables.
	/// @param id Identifies which group of inputs to effect.
	/// @param value The new polarity setting of the input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din32SetPol( uint8 id, uint32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_DIN_32_POL, id+1, value );
	}

	/// Get the current filter constant settings for a group of 32 digital inputs.
	/// For each input in the group, a value of 1 enables the filter, 0 disables.
	/// @param id Identifies which group of inputs to read.
	/// @param value The current filter setting of the input lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din32GetFilt( uint8 id, uint32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_DIN_32_FILT, id+1, value );
	}

	/// Set the current filter constant setting for a group of 32 digital inputs.
	/// For each input in the group, a value of 1 enables the filter, 0 disables.
	/// @param id Identifies which group of inputs to effect.
	/// @param value The new filter setting of the input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din32SetFilt( uint8 id, uint32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_DIN_32_FILT, id+1, value );
	}

	/// Get the 'any transition' interrupt mask settings for a group of 32 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on any change,
	/// and a value of 0 disables the interrupt.
	/// @param id Identifies which group of inputs to read.
	/// @param value The current interrupt mask setting of the input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din32GetMaskAny( uint8 id, uint32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_DIN_32_MASK_ANY, id+1, value );
	}

	/// Set the 'any transition' interrupt mask settings for a group of 32 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on any transition,
	/// and a value of 0 disables.
	/// @param id Identifies which group of inputs to effect.
	/// @param value The new interrupt mask value.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din32SetMaskAny( uint8 id, uint32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_DIN_32_MASK_ANY, id+1, value );
	}

	/// Get the 'low to high' interrupt mask settings for a group of 32 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on a low to high
	/// transition, and a value of 0 disables the interrupt.
	/// @param id Identifies which group of inputs to read.
	/// @param value The current interrupt mask setting of the input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din32GetMaskLow2High( uint8 id, uint32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_DIN_32_MASK_L2H, id+1, value );
	}

	/// Set the 'low to high' interrupt mask settings for a group of 32 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on low to high
	/// transitions, and a value of 0 disables.
	/// @param id Identifies which group of inputs to effect.
	/// @param value The new interrupt mask value.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din32SetMaskLow2High( uint8 id, uint32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_DIN_32_MASK_L2H, id+1, value );
	}

	/// Get the 'high to low' interrupt mask settings for a group of 32 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on a high to low
	/// transition, and a value of 0 disables the interrupt.
	/// @param id Identifies which group of inputs to read.
	/// @param value The current interrupt mask setting of the input lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din32GetMaskHigh2Low( uint8 id, uint32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_DIN_32_MASK_H2L, id+1, value );
	}

	/// Set the 'high to low' interrupt mask settings for a group of 32 digital inputs.
	/// For each input in the group, a value of 1 enables interrupts on high to low
	/// transitions, and a value of 0 disables.
	/// @param id Identifies which group of inputs to effect.
	/// @param value The new interrupt mask value.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Din32SetMaskHigh2Low( uint8 id, uint32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_DIN_32_MASK_H2L, id+1, value );
	}

	//@}

	/***************************************************************************/
	/** \name Digital output control

     If the module contains digital outputs, these methods may be used to configure
     and set those outputs.  The outputs may be set and controlled individually,
     or in groups of 8, 16 or 32 outputs.

     All I/O modules should support access to digital outputs in groups of 8.
     Support for individual access or different groupings is optional under the
     spec.  If a particular device does not support such groupings, an attempt
     to use them should return an error code.

     Each output pin or group of pins is assigned an ID number used to access it.
     When single outputs are accessed, these ID numbers range from 0 (the first output)
     to N-1 (the last output), where N is the total number of output pins available
     on the module.

     When groups of outputs are accessed as a unit, the group is assigned a number.
     The first group of outputs will be assigned ID number 0, the second will be ID 1,
     etc.  The number of groups of a particular size will be the total number of outputs
     divided by the group size.

     For example, to access the twenty seventy output pin individually you would use id
     number 26.  To access it as part of a group of 8 outputs, you would access group
     number 3 (26/8).  Output 26 would be bit 2 (26%8) of that group.

	 */
	/***************************************************************************/
	//@{

	/// Return the number of individual outputs available on this device.
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DoutGetCt( uint16 &ct ){
		ct = 0;
		return BitCount( CIA401_DOUT_1_VALUE, ct );
	}

	virtual const Error *DoutWrite( uint16 id, bool value, bool viaSDO=false );

	/// Get the current polarity setting for an individual digital output.
	/// A value of true enables inversion and false disables.
	/// @param id Identifies the output to read.
	/// @param value The current polarity setting of the output line is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DoutGetPol( uint16 id, bool &value ){
		return BitUpld( CIA401_DOUT_1_POL, id, value );
	}

	/// Set the current polarity setting for an individual digital output.
	/// A value of true enables inversion and false disables.
	/// @param id Identifies which digital output to effect.
	/// @param value The new polarity setting of the output line.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DoutSetPol( uint16 id, bool value ){
		return BitDnld( CIA401_DOUT_1_POL, id, value );
	}

	/// Get the current filter constant setting for an individual digital output.
	/// A value of true enables the filter, false disables.
	/// @param id Identifies the output to read.
	/// @param value The current filter setting of the output line is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DoutGetFilt( uint16 id, bool &value ){
		return BitUpld( CIA401_DOUT_1_FILT, id, value );
	}

	/// Set the current filter constant setting for an individual digital output.
	/// A value of true enables the filter, false disables.
	/// @param id Identifies which digital output to effect.
	/// @param value The new filter setting of the output line.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DoutSetFilt( uint16 id, bool value ){
		return BitDnld( CIA401_DOUT_1_FILT, id, value );
	}

	/// Get the current error mode setting for an individual digital output.
	/// A value of true will cause the output to take it's programmed error value
	/// on a device failure.  Setting the mode to false will cause the output to
	/// hold it's programmed value on failure.
	/// @param id Identifies the output to read.
	/// @param value The current error mode setting of the output line is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DoutGetErrMode( uint16 id, bool &value ){
		return BitUpld( CIA401_DOUT_1_ERRMODE, id, value );
	}

	/// Set the current error mode setting for an individual digital output.
	/// A value of true will cause the output to take it's programmed error value
	/// on a device failure.  Setting the mode to false will cause the output to
	/// hold it's programmed value on failure.
	/// @param id Identifies which digital output to effect.
	/// @param value The new error mode setting of the output line.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DoutSetErrMode( uint16 id, bool value ){
		return BitDnld( CIA401_DOUT_1_ERRMODE, id, value );
	}


	/// Get the current error value setting for an individual digital output.
	/// Error values define the state of the output if a device failure occurs.
	/// The error value will only be set for those output pins which have an error
	/// mode set to true.  Those with error mode set to false will not be changed by
	/// a device failure.
	/// @param id Identifies the output to read.
	/// @param value The current error value setting of the output line is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DoutGetErrValue( uint16 id, bool &value ){
		return BitUpld( CIA401_DOUT_1_ERRVAL, id, value );
	}

	/// Set the current error value setting for an individual digital output.
	/// Error values define the state of the output if a device failure occurs.
	/// The error value will only be set for those output pins which have an error
	/// mode set to true.  Those with error mode set to false will not be changed by
	/// a device failure.
	/// @param id Identifies which digital output to effect.
	/// @param value The new error value setting of the output line.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *DoutSetErrValue( uint16 id, bool value ){
		return BitDnld( CIA401_DOUT_1_ERRVAL, id, value );
	}

	/// Return the number of 8-bit groups of outputs available on this device.
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout8GetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DOUT_8_VALUE, 0, ct );
	}

	const Error *Dout8Write( uint8 id, uint8 value, bool viaSDO=false );

	/// Read back the last value written to this bank of 8 digital outputs.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current state of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	const Error *Dout8Read( uint8 id, uint8 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DOUT_8_VALUE, id+1, value );
	}

	/// Get the current polarity settings for a group of 8 digital outputs.
	/// For each output in the group, a value of 1 enables inversion and 0 disables.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current polarity setting of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout8GetPol( uint8 id, uint8 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DOUT_8_POL, id+1, value );
	}

	/// Set the current polarity setting for a group of 8 digital outputs.
	/// For each output in the group, a value of 1 enables inversion and 0 disables.
	/// @param id Identifies which group of outputs to effect.
	/// @param value The new polarity setting of the output lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout8SetPol( uint8 id, uint8 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld8( CIA401_DOUT_8_POL, id+1, value );
	}

	/// Get the current filter constant settings for a group of 8 digital outputs.
	/// For each output in the group, a value of 1 enables the filter, 0 disables.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current filter setting of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout8GetFilt( uint8 id, uint8 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DOUT_8_FILT, id+1, value );
	}

	/// Set the current filter constant setting for a group of 8 digital outputs.
	/// For each output in the group, a value of 1 enables the filter, 0 disables.
	/// @param id Identifies which group of outputs to effect.
	/// @param value The new filter setting of the output lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout8SetFilt( uint8 id, uint8 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld8( CIA401_DOUT_8_FILT, id+1, value );
	}

	/// Get the current error mode settings for a group of 8 digital outputs.
	/// For each output in the group, a value of 1 will cause the output to take it's
	/// programmed error value on a device failure.  Setting the mode to 0 will cause
	/// the output to hold it's programmed value on failure.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current error mode setting of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout8GetErrMode( uint8 id, uint8 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DOUT_8_ERRMODE, id+1, value );
	}

	/// Set the current error mode settings for a group of 8 digital outputs.
	/// For each output in the group, a value of 1 will cause the output to take it's
	/// programmed error value on a device failure.  Setting the mode to 0 will cause
	/// the output to hold it's programmed value on failure.
	/// @param id Identifies which group of outputs to effect.
	/// @param value The new error mode setting of the output lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout8SetErrMode( uint8 id, uint8 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld8( CIA401_DOUT_8_ERRMODE, id+1, value );
	}


	/// Get the current error value settings for a group of 8 digital outputs.
	/// Error values define the state of the output if a device failure occurs.
	/// The error value will only be set for those output pins which have an error
	/// mode set to 1.  Those with error mode set to zero will not be changed by
	/// a device failure.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current error value setting of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout8GetErrValue( uint8 id, uint8 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DOUT_8_ERRVAL, id+1, value );
	}

	/// Set the current error value settings for a group of 8 digital outputs.
	/// Error values define the state of the output if a device failure occurs.
	/// The error value will only be set for those output pins which have an error
	/// mode set to 1.  Those with error mode set to zero will not be changed by
	/// a device failure.
	/// @param id Identifies which group of outputs to effect.
	/// @param value The new error value setting of the output lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout8SetErrValue( uint8 id, uint8 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld8( CIA401_DOUT_8_ERRVAL, id+1, value );
	}

	/// Return the number of 16-bit groups of outputs available on this device.
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout16GetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DOUT_16_VALUE, 0, ct );
	}

	virtual const Error *Dout16Write( uint8 id, uint16 value, bool viaSDO=false );

	/// Read back the last value written to this bank of 16 digital outputs.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current state of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	const Error *Dout16Read( uint8 id, uint16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_DOUT_16_VALUE, id+1, value );
	}

	/// Get the current polarity settings for a group of 16 digital outputs.
	/// For each output in the group, a value of 1 enables inversion and 0 disables.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current polarity setting of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout16GetPol( uint8 id, uint16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_DOUT_16_POL, id+1, value );
	}

	/// Set the current polarity setting for a group of 16 digital outputs.
	/// For each output in the group, a value of 1 enables inversion and 0 disables.
	/// @param id Identifies which group of outputs to effect.
	/// @param value The new polarity setting of the output lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout16SetPol( uint8 id, uint16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_DOUT_16_POL, id+1, value );
	}

	/// Get the current filter constant settings for a group of 16 digital outputs.
	/// For each output in the group, a value of 1 enables the filter, 0 disables.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current filter setting of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout16GetFilt( uint8 id, uint16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_DOUT_16_FILT, id+1, value );
	}

	/// Set the current filter constant setting for a group of 16 digital outputs.
	/// For each output in the group, a value of 1 enables the filter, 0 disables.
	/// @param id Identifies which group of outputs to effect.
	/// @param value The new filter setting of the output lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout16SetFilt( uint8 id, uint16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_DOUT_16_FILT, id+1, value );
	}

	/// Get the current error mode settings for a group of 16 digital outputs.
	/// For each output in the group, a value of 1 will cause the output to take it's
	/// programmed error value on a device failure.  Setting the mode to 0 will cause
	/// the output to hold it's programmed value on failure.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current error mode setting of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout16GetErrMode( uint8 id, uint16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_DOUT_16_ERRMODE, id+1, value );
	}

	/// Set the current error mode settings for a group of 16 digital outputs.
	/// For each output in the group, a value of 1 will cause the output to take it's
	/// programmed error value on a device failure.  Setting the mode to 0 will cause
	/// the output to hold it's programmed value on failure.
	/// @param id Identifies which group of outputs to effect.
	/// @param value The new error mode setting of the output lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout16SetErrMode( uint8 id, uint16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_DOUT_16_ERRMODE, id+1, value );
	}


	/// Get the current error value settings for a group of 16 digital outputs.
	/// Error values define the state of the output if a device failure occurs.
	/// The error value will only be set for those output pins which have an error
	/// mode set to 1.  Those with error mode set to zero will not be changed by
	/// a device failure.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current error value setting of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout16GetErrValue( uint8 id, uint16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_DOUT_16_ERRVAL, id+1, value );
	}

	/// Set the current error value settings for a group of 16 digital outputs.
	/// Error values define the state of the output if a device failure occurs.
	/// The error value will only be set for those output pins which have an error
	/// mode set to 1.  Those with error mode set to zero will not be changed by
	/// a device failure.
	/// @param id Identifies which group of outputs to effect.
	/// @param value The new error value setting of the output lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout16SetErrValue( uint8 id, uint16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_DOUT_16_ERRVAL, id+1, value );
	}

	/// Return the number of 32-bit groups of outputs available on this device.
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout32GetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_DOUT_32_VALUE, 0, ct );
	}

	virtual const Error *Dout32Write( uint8 id, uint32 value, bool viaSDO=false );

	/// Read back the last value written to this bank of 32 digital outputs.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current state of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	const Error *Dout32Read( uint8 id, uint32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_DOUT_32_VALUE, id+1, value );
	}

	/// Get the current polarity settings for a group of 32 digital outputs.
	/// For each output in the group, a value of 1 enables inversion and 0 disables.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current polarity setting of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout32GetPol( uint8 id, uint32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_DOUT_32_POL, id+1, value );
	}

	/// Set the current polarity setting for a group of 32 digital outputs.
	/// For each output in the group, a value of 1 enables inversion and 0 disables.
	/// @param id Identifies which group of outputs to effect.
	/// @param value The new polarity setting of the output lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout32SetPol( uint8 id, uint32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_DOUT_32_POL, id+1, value );
	}

	/// Get the current filter constant settings for a group of 32 digital outputs.
	/// For each output in the group, a value of 1 enables the filter, 0 disables.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current filter setting of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout32GetFilt( uint8 id, uint32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_DOUT_32_FILT, id+1, value );
	}

	/// Set the current filter constant setting for a group of 32 digital outputs.
	/// For each output in the group, a value of 1 enables the filter, 0 disables.
	/// @param id Identifies which group of outputs to effect.
	/// @param value The new filter setting of the output lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout32SetFilt( uint8 id, uint32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_DOUT_32_FILT, id+1, value );
	}

	/// Get the current error mode settings for a group of 32 digital outputs.
	/// For each output in the group, a value of 1 will cause the output to take it's
	/// programmed error value on a device failure.  Setting the mode to 0 will cause
	/// the output to hold it's programmed value on failure.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current error mode setting of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout32GetErrMode( uint8 id, uint32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_DOUT_32_ERRMODE, id+1, value );
	}

	/// Set the current error mode settings for a group of 32 digital outputs.
	/// For each output in the group, a value of 1 will cause the output to take it's
	/// programmed error value on a device failure.  Setting the mode to 0 will cause
	/// the output to hold it's programmed value on failure.
	/// @param id Identifies which group of outputs to effect.
	/// @param value The new error mode setting of the output lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout32SetErrMode( uint8 id, uint32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_DOUT_32_ERRMODE, id+1, value );
	}


	/// Get the current error value settings for a group of 32 digital outputs.
	/// Error values define the state of the output if a device failure occurs.
	/// The error value will only be set for those output pins which have an error
	/// mode set to 1.  Those with error mode set to zero will not be changed by
	/// a device failure.
	/// @param id Identifies which group of outputs to read.
	/// @param value The current error value setting of the output lines is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout32GetErrValue( uint8 id, uint32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_DOUT_32_ERRVAL, id+1, value );
	}

	/// Set the current error value settings for a group of 32 digital outputs.
	/// Error values define the state of the output if a device failure occurs.
	/// The error value will only be set for those output pins which have an error
	/// mode set to 1.  Those with error mode set to zero will not be changed by
	/// a device failure.
	/// @param id Identifies which group of outputs to effect.
	/// @param value The new error value setting of the output lines.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Dout32SetErrValue( uint8 id, uint32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_DOUT_32_ERRVAL, id+1, value );
	}
	//@}

	/***************************************************************************/
	/** \name Analog input control

     If the module contains analog inputs, these methods may be used to configure
     and read those inputs.

     Most manufacturers support 16-bit access to analog inputs.  Other input sizes
     are optional in the spec. and may or may not be available.
	 */
	/***************************************************************************/
	//@{
	/// Return the number of 8-bit analog inputs available on this device
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain8GetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_AIN_8_VALUE, 0, ct );
	}

	/// Read an 8-bit analog input.
	/// @param id The analog input channel ID
	/// @param value The analog input value
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain8Read( uint8 id, int8 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_AIN_8_VALUE, id+1, value );
	}

	/// Return the number of 16-bit analog inputs available on this device
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain16GetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_AIN_16_VALUE, 0, ct );
	}

	virtual const Error *Ain16Read( uint8 id, int16 &value, bool viaSDO=false );

	/// Return the number of 32-bit analog inputs available on this device
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32GetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_AIN_32_VALUE, 0, ct );
	}

	/// Read a 32-bit analog input.
	/// @param id The analog input channel ID
	/// @param value The analog input value
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32Read( uint8 id, int32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_AIN_32_VALUE, id+1, value );
	}

#ifdef CML_ALLOW_FLOATING_POINT
	/// Return the number of floating point analog inputs available on this device
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltGetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_AIN_FLT_VALUE, 0, ct );
	}

	/// Read a floating point analog input.
	/// @param id The analog input channel ID
	/// @param value The analog input value
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltRead( uint8 id, float &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.UpldFlt( CIA401_AIN_FLT_VALUE, id+1, value );
	}
#endif

	/// Get the analog input offset value as a 32-bit integer.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32GetOffset( uint8 id, int32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_AIN_32_OFFSET, id+1, value );
	}

	/// Set the analog input offset value as a 32-bit integer.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32SetOffset( uint8 id, int32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_AIN_32_OFFSET, id+1, value );
	}

	/// Get the analog input scaling factor as a 32-bit integer.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32GetScaling( uint8 id, int32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_AIN_32_SCALE, id+1, value );
	}

	/// Set the analog input scaling factor as a 32-bit integer.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32SetScaling( uint8 id, int32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_AIN_32_SCALE, id+1, value );
	}

#ifdef CML_ALLOW_FLOATING_POINT
	/// Get the analog input offset value as a floating point value.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltGetOffset( uint8 id, float &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.UpldFlt( CIA401_AIN_FLT_OFFSET, id+1, value );
	}

	/// Set the analog input offset value as a floating point value.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltSetOffset( uint8 id, float value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.DnldFlt( CIA401_AIN_FLT_OFFSET, id+1, value );
	}

	/// Get the analog input scaling factor as a floating point value.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltGetScaling( uint8 id, float &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.UpldFlt( CIA401_AIN_FLT_SCALE, id+1, value );
	}

	/// Set the analog input scaling factor as a floating point value.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltSetScaling( uint8 id, float value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.DnldFlt( CIA401_AIN_FLT_SCALE, id+1, value );
	}
#endif

	/// Get the current setting of the global interrupt enable for analog inputs.
	/// A return value of true indicates that interrupts are enabled, false disabled.
	/// @param value The current interrupt enable setting is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinGetIntEna( bool &value ){
		uint8 v;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		const Error *err = node->sdo.Upld8( CIA401_AIN_INTENA, 0, v );
		value = (v!=0);
		return err;
	}

	/// Set the current setting of the global interrupt enable for analog inputs.
	/// Setting this parameter to true enables interrupts, false disables.
	/// @param value The interrupt enable setting.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinSetIntEna( bool value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		const Error *err = node->sdo.Dnld8( CIA401_AIN_INTENA, 0, (uint8)value );
		if( !err ) ainIntEna = value;
		return err;
	}

	/// Get the analog input trigger type associated with the input channel.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinGetTrigType( uint8 id, CIA401_AIN_TRIG_TYPE &value ){
		uint8 v;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		const Error *err = node->sdo.Upld8( CIA401_AIN_TRIG, id+1, v );
		value = (CIA401_AIN_TRIG_TYPE)v;
		return err;
	}

	/// Set the analog input trigger type associated with the input channel.
	/// @param id The analog input channel ID
	/// @param value The value to set
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinSetTrigType( uint8 id, CIA401_AIN_TRIG_TYPE value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld8( CIA401_AIN_TRIG, id+1, (uint8)value );
	}

	/// Get the analog input interrupt source.  This variable may be used to
	/// determine which analog input has produced an interrupt.  There are
	/// eight banks of interrupt source registers, each of which covers 32
	/// analog inputs in it's 32 bits.  Bank 0 identifies analog inputs 0 to 31,
	/// Bank 1 identifies analog inputs 32 to 63, etc.
	/// The bit associated with the analog input generating the latest interrupt
	/// will be set in the value returned by this read.  Reading this variable
	/// causes all it's bit to be automatically reset.
	/// @param id The bank number to read (0 to 7)
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinGetIntSource( uint8 id, uint32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_AIN_INTSRC, id+1, value );
	}

	/// Get the analog input upper limit value as a 16-bit integer.
	/// The upper limit defines the value at which an interrupt will be
	/// generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain16GetUpperLimit( uint8 id, int16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_AIN_32_UPLIM, id+1, value );
	}

	/// Set the analog input upper limit value as a 16-bit integer.
	/// The upper limit defines the value at which an interrupt will be
	/// generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain16SetUpperLimit( uint8 id, int16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_AIN_32_UPLIM, id+1, value );
	}

	/// Get the analog input lower limit value as a 16-bit integer.
	/// The lower limit defines the value at which an interrupt will be
	/// generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain16GetLowerLimit( uint8 id, int16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_AIN_32_LWLIM, id+1, value );
	}

	/// Set the analog input lower limit value as a 16-bit integer.
	/// The lower limit defines the value at which an interrupt will be
	/// generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain16SetLowerLimit( uint8 id, int16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_AIN_32_LWLIM, id+1, value );
	}

	/// Get the analog input unsigned delta value as a 16-bit integer.
	/// The unsigned delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain16GetUnsignedDelta( uint8 id, int16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_AIN_32_UDELTA, id+1, value );
	}

	/// Set the analog input unsigned delta value as a 16-bit integer.
	/// The unsigned delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain16SetUnsignedDelta( uint8 id, int16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_AIN_32_UDELTA, id+1, value );
	}

	/// Get the analog input negative delta value as a 16-bit integer.
	/// The negative delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain16GetNegativeDelta( uint8 id, int16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_AIN_32_NDELTA, id+1, value );
	}

	/// Set the analog input negative delta value as a 16-bit integer.
	/// The negative delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain16SetNegativeDelta( uint8 id, int16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_AIN_32_NDELTA, id+1, value );
	}

	/// Get the analog input positive delta value as a 16-bit integer.
	/// The positive delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain16GetPositiveDelta( uint8 id, int16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_AIN_32_PDELTA, id+1, value );
	}

	/// Set the analog input positive delta value as a 16-bit integer.
	/// The positive delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain16SetPositiveDelta( uint8 id, int16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_AIN_32_PDELTA, id+1, value );
	}

	/// Get the analog input upper limit value as a 32-bit integer.
	/// The upper limit defines the value at which an interrupt will be
	/// generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32GetUpperLimit( uint8 id, int32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_AIN_32_UPLIM, id+1, value );
	}

	/// Set the analog input upper limit value as a 32-bit integer.
	/// The upper limit defines the value at which an interrupt will be
	/// generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32SetUpperLimit( uint8 id, int32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_AIN_32_UPLIM, id+1, value );
	}

	/// Get the analog input lower limit value as a 32-bit integer.
	/// The lower limit defines the value at which an interrupt will be
	/// generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32GetLowerLimit( uint8 id, int32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_AIN_32_LWLIM, id+1, value );
	}

	/// Set the analog input lower limit value as a 32-bit integer.
	/// The lower limit defines the value at which an interrupt will be
	/// generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32SetLowerLimit( uint8 id, int32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_AIN_32_LWLIM, id+1, value );
	}

	/// Get the analog input unsigned delta value as a 32-bit integer.
	/// The unsigned delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32GetUnsignedDelta( uint8 id, int32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_AIN_32_UDELTA, id+1, value );
	}

	/// Set the analog input unsigned delta value as a 32-bit integer.
	/// The unsigned delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32SetUnsignedDelta( uint8 id, int32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_AIN_32_UDELTA, id+1, value );
	}

	/// Get the analog input negative delta value as a 32-bit integer.
	/// The negative delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32GetNegativeDelta( uint8 id, int32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_AIN_32_NDELTA, id+1, value );
	}

	/// Set the analog input negative delta value as a 32-bit integer.
	/// The negative delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32SetNegativeDelta( uint8 id, int32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_AIN_32_NDELTA, id+1, value );
	}

	/// Get the analog input positive delta value as a 32-bit integer.
	/// The positive delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32GetPositiveDelta( uint8 id, int32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_AIN_32_PDELTA, id+1, value );
	}

	/// Set the analog input positive delta value as a 32-bit integer.
	/// The positive delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Ain32SetPositiveDelta( uint8 id, int32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_AIN_32_PDELTA, id+1, value );
	}

#ifdef CML_ALLOW_FLOATING_POINT
	/// Get the analog input upper limit value as a floating point value.
	/// The upper limit defines the value at which an interrupt will be
	/// generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltGetUpperLimit( uint8 id, float &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.UpldFlt( CIA401_AIN_FLT_UPLIM, id+1, value );
	}

	/// Set the analog input upper limit value as a floating point value.
	/// The upper limit defines the value at which an interrupt will be
	/// generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltSetUpperLimit( uint8 id, float value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.DnldFlt( CIA401_AIN_FLT_UPLIM, id+1, value );
	}

	/// Get the analog input lower limit value as a floating point value.
	/// The lower limit defines the value at which an interrupt will be
	/// generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltGetLowerLimit( uint8 id, float &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.UpldFlt( CIA401_AIN_FLT_LWLIM, id+1, value );
	}

	/// Set the analog input lower limit value as a floating point value.
	/// The lower limit defines the value at which an interrupt will be
	/// generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltSetLowerLimit( uint8 id, float value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.DnldFlt( CIA401_AIN_FLT_LWLIM, id+1, value );
	}

	/// Get the analog input unsigned delta value as a floating point value.
	/// The unsigned delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltGetUnsignedDelta( uint8 id, float &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.UpldFlt( CIA401_AIN_FLT_UDELTA, id+1, value );
	}

	/// Set the analog input unsigned delta value as a floating point value.
	/// The unsigned delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltSetUnsignedDelta( uint8 id, float value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.DnldFlt( CIA401_AIN_FLT_UDELTA, id+1, value );
	}

	/// Get the analog input negative delta value as a floating point value.
	/// The negative delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltGetNegativeDelta( uint8 id, float &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.UpldFlt( CIA401_AIN_FLT_NDELTA, id+1, value );
	}

	/// Set the analog input negative delta value as a floating point value.
	/// The negative delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltSetNegativeDelta( uint8 id, float value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.DnldFlt( CIA401_AIN_FLT_NDELTA, id+1, value );
	}

	/// Get the analog input positive delta value as a floating point value.
	/// The positive delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltGetPositiveDelta( uint8 id, float &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.UpldFlt( CIA401_AIN_FLT_PDELTA, id+1, value );
	}

	/// Set the analog input positive delta value as a floating point value.
	/// The positive delta defines the amount of change at which an
	/// interrupt will be generated if it is enabled.
	/// @param id The analog input channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AinFltSetPositiveDelta( uint8 id, float value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.DnldFlt( CIA401_AIN_FLT_PDELTA, id+1, value );
	}
#endif
	//@}

	/***************************************************************************/
	/** \name Analog output control

     If the module contains analog outputs, these methods may be used to configure
     and write to those outputs.

     Most manufacturers support 16-bit access to analog inputs.  Other input sizes
     are optional in the spec. and may or may not be available.
	 */
	/***************************************************************************/
	//@{

	/// Return the number of 8-bit analog outputs available on this device
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Aout8GetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_AOUT_8_VALUE, 0, ct );
	}

	/// Write to an 8-bit analog output.
	/// @param id The analog input channel ID
	/// @param value The value to write.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Aout8Write( uint8 id, int8 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld8( CIA401_AOUT_8_VALUE, id+1, value );
	}

	/// Return the number of 16-bit analog outputs available on this device
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Aout16GetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_AOUT_16_VALUE, 0, ct );
	}

	virtual const Error *Aout16Write( uint8 id, int16 value, bool viaSDO=false );

	/// Return the number of 32-bit analog outputs available on this device
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Aout32GetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_AOUT_32_VALUE, 0, ct );
	}

	/// Write to a 32-bit analog output.
	/// @param id The analog input channel ID
	/// @param value The value to write.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Aout32Write( uint8 id, int32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_AOUT_32_VALUE, id+1, value );
	}

#ifdef CML_ALLOW_FLOATING_POINT
	/// Return the number of floating point analog outputs available on this device
	/// @param ct The count is returned here.  Zero is returned on error.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AoutFltGetCt( uint8 &ct ){
		ct = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld8( CIA401_AOUT_FLT_VALUE, 0, ct );
	}

	/// Write to a floating point analog output.
	/// @param id The analog input channel ID
	/// @param value The value to write.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AoutFltWrite( uint8 id, float value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.DnldFlt( CIA401_AOUT_FLT_VALUE, id+1, value );
	}
#endif

	/// Get the analog output offset value as a 32-bit integer.
	/// @param id The analog output channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Aout32GetOffset( uint8 id, int32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_AOUT_32_OFFSET, id+1, value );
	}

	/// Set the analog output offset value as a 32-bit integer.
	/// @param id The analog output channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Aout32SetOffset( uint8 id, int32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_AOUT_32_OFFSET, id+1, value );
	}

	/// Get the analog output scaling factor as a 32-bit integer.
	/// @param id The analog output channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Aout32GetScaling( uint8 id, int32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_AOUT_32_SCALE, id+1, value );
	}

	/// Set the analog output scaling factor as a 32-bit integer.
	/// @param id The analog output channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Aout32SetScaling( uint8 id, int32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_AOUT_32_SCALE, id+1, value );
	}

#ifdef CML_ALLOW_FLOATING_POINT
	/// Get the analog output offset value as a floating point value.
	/// @param id The analog output channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AoutFltGetOffset( uint8 id, float &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.UpldFlt( CIA401_AOUT_FLT_OFFSET, id+1, value );
	}

	/// Set the analog output offset value as a floating point value.
	/// @param id The analog output channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AoutFltSetOffset( uint8 id, float value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.DnldFlt( CIA401_AOUT_FLT_OFFSET, id+1, value );
	}

	/// Get the analog output scaling factor as a floating point value.
	/// @param id The analog output channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AoutFltGetScaling( uint8 id, float &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.UpldFlt( CIA401_AOUT_FLT_SCALE, id+1, value );
	}

	/// Set the analog output scaling factor as a floating point value.
	/// @param id The analog output channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AoutFltSetScaling( uint8 id, float value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.DnldFlt( CIA401_AOUT_FLT_SCALE, id+1, value );
	}
#endif

	/// Get the analog output error mode.
	/// If the error mode is true, then the analog output will change it's value
	/// to the programmed error value in the case of a device failure.  If false,
	/// a device failure will not cause a change in the analog output value.
	/// @param id The analog output channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AoutGetErrMode( uint8 id, bool &value ){
		uint8 v;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		const Error *err = node->sdo.Upld8( CIA401_AOUT_ERRMODE, id+1, v );
		value = (v!=0);
		return err;
	}

	/// Set the analog output error mode.
	/// If the error mode is true, then the analog output will change it's value
	/// to the programmed error value in the case of a device failure.  If false,
	/// a device failure will not cause a change in the analog output value.
	/// @param id The analog output channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AoutSetErrMode( uint8 id, bool value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld8( CIA401_AOUT_ERRMODE, id+1, (uint8)value );
	}

	/// Get the analog output error value as a 32-bit integer.
	/// The error value is the value that the analog output will assume
	/// on device error, if it's error mode is set to true.
	/// @param id The analog output channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Aout32GetErrValue( uint8 id, int32 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld32( CIA401_AOUT_32_ERRVAL, id+1, value );
	}

	/// Set the analog output error value as a 32-bit integer.
	/// The error value is the value that the analog output will assume
	/// on device error, if it's error mode is set to true.
	/// @param id The analog output channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Aout32SetErrValue( uint8 id, int32 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld32( CIA401_AOUT_32_ERRVAL, id+1, value );
	}

	/// Get the analog output error value as a 16-bit integer.
	/// The error value is the value that the analog output will assume
	/// on device error, if it's error mode is set to true.
	/// @param id The analog output channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Aout16GetErrValue( uint8 id, int16 &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Upld16( CIA401_AOUT_32_ERRVAL, id+1, value );
	}

	/// Set the analog output error value as a 32-bit integer.
	/// The error value is the value that the analog output will assume
	/// on device error, if it's error mode is set to true.
	/// @param id The analog output channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *Aout16SetErrValue( uint8 id, int16 value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld16( CIA401_AOUT_32_ERRVAL, id+1, value );
	}

#ifdef CML_ALLOW_FLOATING_POINT
	/// Get the analog output error value as a floating point value.
	/// The error value is the value that the analog output will assume
	/// on device error, if it's error mode is set to true.
	/// @param id The analog output channel ID
	/// @param value The value is returned here.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AoutFltGetErrValue( uint8 id, float &value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.UpldFlt( CIA401_AOUT_FLT_ERRVAL, id+1, value );
	}

	/// Set the analog output error value as a floating point value.
	/// The error value is the value that the analog output will assume
	/// on device error, if it's error mode is set to true.
	/// @param id The analog output channel ID
	/// @param value The value to be set.
	/// @return A pointer to an error object, or NULL on success
	virtual const Error *AoutFltSetErrValue( uint8 id, float value ){
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.DnldFlt( CIA401_AOUT_FLT_ERRVAL, id+1, value );
	}
#endif
	//@}

//	virtual void PostIOEvent( CIA401_EVENTS event )
//	{
//		eventMap.setBits( (uint32)event );
//		eventMap.clrBits( (uint32)event );
//	}

protected:

	EventMap  eventMap;

	bool dinIntEna;
	bool ainIntEna;

	/// Upload a setting for a single digital I/O pin.  The object dictionary
	/// entry is calculated based on the pin ID and the base object dictionary
	/// ID passed.
	/// @param base The object dictionary base index for this parameter
	/// @param id The I/O pin ID.  Must range from 0 to 1024
	/// @param value The boolean value is returned here
	/// @return A pointer to an error object, or NULL on success
	const Error *BitUpld( uint16 base, uint16 id, bool &value )
	{
		if( id >= 1024 ) return &CiA401Error::BadID;
		base += id>>7;

		uint8 v;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		const Error *err = node->sdo.Upld8( base, (uint8)( (id&0x7F)+1 ), v );
		value = (v!=0);
		return err;
	}

	/// Download a setting for a single digital I/O pin.  The object dictionary
	/// index/sub-index is calculated from the pin ID and a passed base address.
	/// @param base The object dictionary base index for this parameter
	/// @param id The I/O pin ID.  Must range from 0 to 1024
	/// @param value The boolean value is passed here.
	/// @return A pointer to an error object, or NULL on success
	const Error *BitDnld( uint16 base, uint16 id, bool value )
	{
		if( id >= 1024 ) return &CiA401Error::BadID;
		base += id>>7;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		return node->sdo.Dnld8( base, (uint8)( (id&0x7F)+1 ), (uint8)value );
	}

	/// Count the number of individual I/O pins available on the device.
	/// @param base The base index for the object dictionary
	/// @param ct The count is returned here
	/// @return A pointer to an error object, or NULL on success
	const Error *BitCount( uint16 base, uint16 &ct )
	{
		const Error *err = 0;
		RefObjLocker<MDNode> node( GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;

		ct = 0;
		for( int i=0; i<8; i++ )
		{
			uint8 x;
			err = node->sdo.Upld8( base++, 0, x );

			if( err )
				break;
			ct += x;
		}

		if( !ct )
			return err;

		return 0;
	}



	/// Default PDO used to transmit digital output info
	RPDO_Dout doutPDO;
	TPDO_Din  dinPDO;
	RPDO_Aout aoutPDO[3];
	TPDO_Ain  ainPDO[3];

	/// Post an event condition to the I/O module's event map.
	/// This method is used internally by the various standard
	/// transmit PDOs when a new PDO message is received.
	/// @param event The event bit(s) to post
	virtual void PostIOEvent( CIA401_EVENTS event )
	{
		eventMap.setBits( (uint32)event );
		eventMap.clrBits( (uint32)event );
	}

	friend class RPDO_Dout;
	friend class TPDO_Din;
	friend class RPDO_Aout;
	friend class TPDO_Ain;

private:
	/// Private copy constructor (not supported)
	CiA401( const CiA401& );

	/// Private assignment operator (not supported)
	CiA401& operator=( const CiA401& );
};

CML_NAMESPACE_END()
#endif //_CIA401_
