//#include "komodo.h"  //defines all the uintx_t types
//Note: For GNU C-Compiler use compiler option  CC=gcc -m32 -fshort-enums     (-fshort-enums as default system will use 32bit enum variables!!!)

#pragma pack(push, 4)

#define ALIGNED_(x)	 __attribute__ ((aligned (x)))


#define  MAX_THERMISTORS_PER_CELL 	2
#define  MAX_CELL_NUM_LTC 			12
#define  FAST_STORAGE_STACK_SIZE 	10
#define  TOTAL_24V_ONBOARD_TEMP_SENSORS	6
#define  MAX_BATTERY_STRINGS_NUMBER 1
#define  MAX_RACKS_FOR_FORKLIFT    	4
#define CHARGER_NAME_LENGTH			8 //length of the string received from the charger
#define LEAK_VOLT_MEAS_SIZE							3
#define LTC3300_CONFIG_NUM_ICS_PER_ADDRESS					(2)


#ifndef _MSC_VER
/* C99-compliant compilers (GCC) */
#include <stdint.h>
typedef uint8_t   u08;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef int8_t    s08;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

#else
/* Microsoft compilers (Visual C++) */
typedef unsigned __int8   u08;
typedef unsigned __int16  u16;
typedef unsigned __int32  u32;
typedef unsigned __int64  u64;
typedef signed   __int8   s08;
typedef signed   __int16  s16;
typedef signed   __int32  s32;
typedef signed   __int64  s64;

#endif /* __MSC_VER */

typedef float   f32;
typedef double  f64;



typedef enum
{
	masterOpStateBoot,										//0
	masterOpStateWaitCommunStart,							//1
	masterOpStateEntryToAllStringsDisconnected,				//2
	masterOpStateAllStringsDisconnected,					//3
	masterOpStateConnectStringsForDisch,  					//4
	masterOpStateDischarge,									//5
	masterOpStatePreparationToCharging,						//6
	masterOpStateCharging,									//7
	masterOpStateCurrentTransducerOffsetCalibrationDelay,	//8
	masterOpStateCurrentTransducerOffsetCalibration,		//9
	masterOpStateLeakMeasurement,							//10
	masterOpStateChargePreparationNoDCR,                    //11
	masterOpStateExitFromChargePreparationNoDCR,			//12
	masterOpStateEntryToPowerOffState,						//13
	masterOpStatePowerOff,									//14
	masterOpStateHardwareFailure,							//15
	masterOpStateDisableWDog,								//16
	masterOpStateStopping,									//17
	masterOpStateStopped,									//18
	masterOpStateEntryToEWS,								//19
	masterOpStateWaitEWSResponse,							//20
	masterOpStatePoweredOff,								//21
	masterOpStatePowerOffProblem,							//22
	masterOpStateJmpToBootLoader							//23
}MasterOperationState;


//niloo
static const char * const MasterOperationState_names[] = {
	[masterOpStateBoot] = "Boot",
	[masterOpStateWaitCommunStart] = "Awaiting To Start Communication",
	[masterOpStateEntryToAllStringsDisconnected] = "Entry To All Cabinets Disconnected",
	[masterOpStateAllStringsDisconnected] = "All Strings Disconnected", 		  /*hy niloo you missed this  enum masterOpStateAllStringsDisconnected causing crashes*/
	[masterOpStateConnectStringsForDisch] = "Connect Cabinets For Discharge",
	[masterOpStateDischarge] = "Discharge",
	[masterOpStatePreparationToCharging] = "Preparation To Charging",
	[masterOpStateCharging] = "Charging",
	[masterOpStateCurrentTransducerOffsetCalibrationDelay] = "Current Transducer Offset Calibration Delay",
	[masterOpStateCurrentTransducerOffsetCalibration] = "Current Transducer Offset Calibration",
	[masterOpStateLeakMeasurement] = "Leak Measurement",
	[masterOpStateChargePreparationNoDCR] = "Charge Preparation. No DCR.",
	[masterOpStateExitFromChargePreparationNoDCR] = "Exit Charge Preparation. No DCR.",
	[masterOpStateEntryToPowerOffState] = "Power Off",
	[masterOpStatePowerOff] = "Power Off",
	[masterOpStateHardwareFailure] = "Hardware Failure",
	[masterOpStateDisableWDog] = "Disable Watch Dog",
	[masterOpStateStopping] = "Stopping",
	[masterOpStateStopped] = "Stopped",
	[masterOpStateEntryToEWS] = "Entry To EWS",
	[masterOpStateWaitEWSResponse] = "Awaiting EWS Response",
	[masterOpStatePoweredOff] = "Powered Off",
	[masterOpStatePowerOffProblem] = "PowerOffProblem",
	[masterOpStateJmpToBootLoader] = "JumpToBootLoader"
};



typedef enum
{
	PackCurrentInRange,
	PackCurrentOutOfRange
} PackCurrentStatus;

typedef enum
{
	ChargeNotStarted, //ChargeNotStoped
	ChargeStopedStringCanBeDischargedOnly,
	ChargeStopedStringCannotBeConnected,
	ChargeStopedOverCurrent,
	ChargeStarting, //ChargerRemoved
	ChargerRemoved
}ChargeStopReason;

typedef enum
{
	NoNewCalibrationData,
	NewCalibrationDataAvailable
} CalibrationDataRenewal;

typedef enum
{
	CAN_M_TO_RACK_FW_VERSION_REQUEST,			//0, start of order important area
	CAN_M_TO_RACK_BOARD_REV_REQUEST,			//1
	CAN_M_TO_RACK_LENGTH_STATUS_STR_REQUEST,	//2, at the beginning
	CAN_M_TO_RACK_STATUS_STR_REQUEST,			//3, at the beginning
	CAN_M_TO_RACK_LENGTH_DIAGN_STR_REQUEST,		//4
	CAN_M_TO_RACK_DIAGN_STR_REQUEST,			//5
	CAN_M_TO_RACK_START_BOOTLOADER,				//6 Currently not used
	CAN_M_TO_RACK_START_DCR_CALIBR,				//7 Currently not used
	CAN_M_TO_RACK_START_CURRENT_TRANSD_CALIBR,  //8
	CAN_M_TO_RACK_START_CHARGING,				//9
	CAN_M_TO_RACK_STOP_CHARGING,				//10
	CAN_M_TO_RACK_RESET_FAILURES,				//11
	CAN_M_TO_RACK_ENTER_STR_DISCHARGE_STATE,    //12
	CAN_M_TO_RACK_ENTER_STR_DISCONNECTED_STATE, //13
	CAN_M_TO_RACK_CP_LENGTH,					//14
	CAN_M_TO_RACK_CP_REQ,						//15
	CAN_M_TO_RACK_LENGTH_SHORT_STATUS_REQUEST,	//16
	CAN_M_TO_RACK_SHORT_STATUS_REQUEST,			//17,
	CAN_M_TO_RACK_POWER_OFF,                    //18 end of order important area
	CAN_M_TO_RACK_WAIT_ACK_STR_DISCHARGE_STATE, //19
	CAN_M_TO_RACK_WAIT_ACK_STR_DISCONNECTED_STATE, //20
	CAN_M_TO_RACK_WAIT_ACK_START_DCR_CALIBR,	//21 Currently not used
	CAN_M_TO_RACK_WAIT_ACK_START_CURRENT_TRANSD_CALIBR, //22
	CAN_M_TO_RACK_WAIT_ACK_START_CHARGING,		//23
	CAN_M_TO_RACK_WAIT_ACK_STOP_CHARGING,		//24
	CAN_M_TO_RACK_START_LENGTH_SHORT_ST_REQ,	//25
	CAN_M_TO_RACK_WAIT_ACK_RESET_FAILURES,		//26
	CAN_M_TO_RACK_NOT_STARTED,  				//27, after initialization
	CAN_M_TO_RACK_INITIAL,						//28
	CAN_M_TO_RACK_FINISHED,						//29, after transaction completed
	CAN_M_TO_RACK_WAIT_ACK_POWER_OFF,			//30
    CAN_M_TO_RACK_START_INIT_WITH_WDOG_ENABLE,  //31
	CAN_M_TO_RACK_START_LENGTH_LONG_ST_REQ,     //32
	CAN_M_TO_RACK_LENGTH_LONG_STATUS_REQUEST,	//33
	CAN_M_TO_RACK_LONG_STATUS_REQUEST,			//34
	CAN_M_TO_RACK_SEND_CP_LENGTH,				//35
	CAN_M_TO_RACK_SEND_CP_LENGTH_WAIT_ACK,		//36
	CAN_M_TO_RACK_SEND_CP_DATA,					//37
	CAN_M_TO_RACK_START_CALIBR_REQ,				//38
	CAN_M_TO_RACK_LENGTH_CALIBR_REQUEST,		//39
	CAN_M_TO_RACK_CALIBR_REQUEST,				//40
	CAN_M_TO_RACK_DCR_PERIOD_START,				//41
	CAN_M_TO_RACK_WAIT_ACK_DCR_PERIOD_START,	//42
	CAN_M_TO_RACK_PERFORM_DCR_PERIOD,			//43
	CAN_M_TO_RACK_WAIT_ACK_PERFORM_DCR_PERIOD,  //44
	CAN_M_TO_RACK_STOP_WDOG,					//45
	CAN_M_TO_RACK_WAIT_ACK_STOP_WDOG,			//46
	CAN_M_TO_RACK_START_WDOG,					//47
	CAN_M_TO_RACK_WAIT_ACK_START_WDOG,			//48
	CAN_M_TO_RACK_ASK_BOARD_REVISION,			//49
	CAN_M_TO_RACK_START_LENGTH_SERIALZ_STR_REQUEST,     //50
	CAN_M_TO_RACK_LENGTH_SERIALZ_STR_REQUEST,	//51
	CAN_M_TO_RACK_SERIALZ_STR_REQUEST,			//52
	CAN_M_TO_RACK_FAILURE,						//53
	CAN_M_TO_RACK_START_LENGTH_DIAGS_REQ,     	//54
	CAN_M_TO_RACK_LENGTH_DIAGNOSTICS_REQUEST,	//55
	CAN_M_TO_RACK_DIAGNOSTICS_REQUEST,			//56
	CAN_M_TO_RACK_START_LEAK_MEASUREMENT,		//57
	CAN_M_TO_RACK_WAIT_ACK_LEAK_MEASUREMENT,    //58
	CAN_M_TO_RACK_DISCONNECT_NEG_CONTACTOR,		//59
	CAN_M_TO_RACK_WAIT_ACK_DISCONNECT_NEG_CONTACTOR,   //60
	CAN_M_TO_RACK_CONNECT_NEG_CONTACTOR,		//61
	CAN_M_TO_RACK_WAIT_ACK_CONNECT_NEG_CONTACTOR,       //62
	CAN_M_TO_RACK_SEND_SAFETY_CP_LENGTH,		//63
	CAN_M_TO_RACK_SEND_SAFETY_CP_LENGTH_WAIT_ACK, 		//64
	CAN_M_TO_RACK_SEND_SAFETY_CP_DATA					//65
} CANMasterToRackCommunication;

typedef enum
{
	ChargerNotPresent,
	ChargerPresent,
	ChargerPilotDetected         /*hy Apr24 Needed Interim State for Bassi as PilotSignal can be there(Cable connected) but FrontPanel Switched OFF so until CAN0 ping response occurs Don't start charging*/
}ChargerPresence;

//niloo
static const char * const ChargerPresence_names[] = {
	[ChargerNotPresent] = "Not Present",
	[ChargerPresent] = "Present",
	[ChargerPilotDetected] = "Pilot Detected"
};

typedef enum
{
	ChargerNotActivated,
	ChargerActivated,
	ChargerActivationUnknown
}ChargerActivation;


typedef enum
{
    ChargerHmiNotPresent,           //0 display fork lift sign instead of charger
	ChargerHmiPresentNotCharging,   //1 Yellow frame
	ChargerHmiConnected,			//2 green frame, not flashing. Communication with charger established
	ChargerHmiCharging,   //3 Green bolt, flashing
	ChargerHmiChargeCompleted, //4 Green solid
	ChargerHmiNoChargeDueToConditions, //5 most likely over current. Can be over voltage.
	ChargerHmiStoppedByOperator,   //6 Orange
	ChargerHmiNoChargeDueToLowTemperature, //7 Thermometer Blue
	ChargerHmiNoChargeDueToHighTemperature //8 Thermometer Red
}ChargerHmiStatus;

typedef enum
{
	ContactorNotActivated,  //default for contactorActivationParameter
	ContactorActivated,
	ContactorUnknown
}ContactorActivation;

typedef enum
{
	OutputZero,
	OutputOne,
	OutputUnknown
}OutputState;

typedef enum
{
	opStateUndefined,           						//0
	opStateBoot,										//1
	opStateCharging,									//2
	opStateDischarge,									//3
	opStateStringDisconnected,   						//4
	opStateCurrentTransducerOffsetCalibrationDelay,		//5
	opStateCurrentTransducerOffsetCalibration,			//6
	opStateLeakMeasurement,                             //7
	opStatePowerOff,									//8
	opStateJumpToBootLoader,							//9
	opStateDelayOnStartDCRPeriodic,						//10
	opStateStartDCRPeriodic,							//11
	opStateDCRPeriodic,									//12
	opStatePowerOffProblem,								//13
	opStateHardwareFailure			  					//14
}OperationState;

//niloo
static const char * const OperationState_names[] = {
	[opStateUndefined] = "Undefined",
	[opStateBoot] = "Boot",
	[opStateCharging] = "Charging",
	[opStateDischarge] = "Discharge",
	[opStateStringDisconnected] = "Disconnected",
	[opStateCurrentTransducerOffsetCalibrationDelay] = "CurrentTransducerOffsetCalibrationDelay",
	[opStateCurrentTransducerOffsetCalibration] = "CurrentTransducerOffsetCalibration",
	[opStateLeakMeasurement] = "LeakMeasurement",
	[opStatePowerOff] = "PowerOff",
	[opStateJumpToBootLoader] = "JumpToBootLoader",
	[opStateDelayOnStartDCRPeriodic] = "DelayOnStartDCRPeriodic",
	[opStateStartDCRPeriodic] = "StartDCRPeriodic",
	[opStateDCRPeriodic] = "DCRPeriodic",
	[opStatePowerOffProblem] = "PowerOffProblem",
	[opStateHardwareFailure] = "HardwareFailure"
};


typedef enum
{
	bstringCommunNotActive, //can start communication in this state
	bstringCommunActive,
	bstringCommunRestart
}BstringCommunication;

typedef enum
{
	AutoBalancingNotStarted,
	//AutoBalancingReadyToStart,
	AutoBalancingInProgress,
	AutoBalancingSuspended,
	AutoBalancingToSuspend,
	AutoBalancingLongSuspention
} AutoBalancingStateMachine;

typedef enum
{
	StringVoltageCriticalHighNotRegistered,
	StringVoltageCriticalHighRegistered
}StringVoltageCriticalHigh;

typedef enum
{
	LTC6804_WRITE_CONFIG,									//0
	LTC6804_SCAN_WITHOUT_BALANCING,							//1
	LTC6804_SCAN_WITH_BALANCING,							//2
	LTC6804_SPI_ERROR_STATE,								//3
	LTC6804_WAIT_RESTART,									//4
	LTC6804_WAIT_POWER_ON									//5
}CELL_SCAN_TASK_ID;





typedef uint32_t CommonDeviceID_t;
typedef uint16_t FirmwareError_t;

typedef struct
{
    uint8_t ALIGNED_(1) spare1[4];//4 complement to 4 bytes
    /// Indicates which device caused seriousErrorOccurred flag to
    /// be set to MB_SERIOUS_ERR_DEVICE_SHUTDOWN.  For all other
    /// values of seriousErrorOccurred, this member is cleared.
    /// The device IDs are bitwise-exclusive to make identifying the
    /// error-causing device simpler.
    CommonDeviceID_t ALIGNED_(4) masterSeriousDeviceErrorBitmask;//8
    /// Indicates whether or not the firmware watchdog or host communication
    // watchdog caused a system restart
    FirmwareError_t ALIGNED_(2) masterSeriousErrorOccurred;//10

    uint8_t ALIGNED_(1) spare[2];//complement to 4 bytes
} MasterMakerGeneralStatus_t;//12

typedef enum
{
	BootLoaderNotActive,
	BootLoaderActive
} BootLoaderActiveInfo;

typedef enum
{
    CAN_HOST_NOT_STARTED,
	CAN_HOST_RECEIVING,
    CAN_HOST_TRANSMITTING,
	CAN_HOST_ERROR
} CANHostState;

typedef enum
{
	ChargerOpNotInitialised,
	ChargerOpInitialised
}ChargerOperationState;

typedef enum
{
	ChargerCurrent00,   //hy 23-jan-17 added to identify 0 current required ie disconnect and 100%SOC Av mode doesn't have setting for 0 current
	ChargerCurrent16,
	ChargerCurrent22,  //added for one rack configuration
	ChargerCurrent26,
	ChargerCurrent49_1,
	ChargerCurrent63,
	ChargerCurrent75, //ChargerCurrent80_6
	ChargerCurrent95,
	ChargerCurrent100
}ChargerCurrentControl;

typedef enum
{
	EWSLow, //Activated
	EWSHigh,	  //NotActivated
	EWSUnknown
}EWSActivation;
//niloo
static const char * const EWSActivation_names[] = {
	[EWSLow] = "Low(Activated)",
	[EWSHigh] = "High(NotActivated)",
	[EWSUnknown] = "Unknown"
};


typedef enum
{
	EWSReturnLow,	 	  //NotActivated
	EWSReturnHigh		  //Activated
}EWSReturn;
//niloo
static const char * const EWSReturn_names[] = {
	[EWSReturnLow] = "Low(Truck Not Connected)",
	[EWSReturnHigh] = "High(Truck Connected)"
};

typedef enum
{
	ContactOpen,
	ContactClosed,
	ContactUnknown
}ContactorConnectionIndication;

//niloo
static const char * const ContactorConnectionIndication_names[] = {
	[ContactOpen] = "Off",
	[ContactClosed] = "On",
	[ContactUnknown] = "Unknown"
};


typedef enum
{
	ContactorStateOpen,
	ContactorStateWaitClosingCompletion,
	ContactorStateClosed,
	ContactorStateWaitOpeningCompletion
}ContactorStates;

//niloo
static const char * const ContactorStates_names[] = {
	[ContactorStateOpen] = "Open",
	[ContactorStateWaitClosingCompletion] = "Awaiting Closing Completion",
	[ContactorStateClosed] = "Closed",
	[ContactorStateWaitOpeningCompletion] = "Awaiting Opening Completion"
};


typedef enum
{
	VOLTAGE_IN_LIMITS,
	VOLTAGE_NOT_IN_LIMITS,
	VOLTAGE_UNKNOWN
}VoltageStatus;

typedef enum
{
	FuseClosed,
	FuseOpen,
	FuseUnknown
} FuseStatus ;

typedef enum
{
	ChargeCurrentOnStringInLimit,
	ChargeCurrentOnStringOutLimit
}ChargeCurrentOnString;

typedef enum
{
	LTC6804_INIT_WAKE_FROM_SLEEP,                  				//0
	LTC6804_INIT_WAKE_FROM_IDLE,								//1
	LTC6804_INIT_WRITE_CONFIG,									//2
	LTC6804_INIT_I2C_RG_CONFIG,									//3
	LTC6804_INIT_STCOMM_SHIFT_FOR_INIT,							//4
	LTC6804_INIT_WAIT_STCOMM_COMPLETION,						//5
	LTC6804_INIT_COMPLETED										//6
} LTC6804InitStateMachine;

typedef enum
{
	LTC6804_NO_BALANCE_SET_THERMISTOR_INPUT,						//0
	LTC6804_NO_BALANCE_STCOMM_SHIFT,								//1
	LTC6804_NO_BALANCE_CLR_CELL_VOLTAGE_RG,							//2
	LTC6804_NO_BALANCE_CLR_AUX_RG_GROUP,							//3
	LTC6804_NO_BALANCE_START_CELL_VOLT_AND_TEMPER_ADC_CONVERSION,	//4
	LTC6804_NO_BALANCE_WAKE_FROM_IDLE_AFTER_ADCVAX,					//5
	LTC6804_NO_BALANCE_READ_CELL_VOLT_AND_TEMPER_RG_GROUP,			//6
	LTC6804_NO_BALANCE_PROCESS_CELL_VOLT_AND_TEMPER_RG_GROUP_RESULTS,  //7
	LTC6804_NO_BALANCE_COMMUNICATION_COMPLETED						//8
} LTC6804NoBalanceStateMachine;

typedef enum
{
	BALANCE_IDLE,  //no balance in progress			//0
	BALANCE_IN_PROGRESS, //balance in progress		//1
	BALANCE_STCOMM_SHIFT_STAGE1,					//2
	BALANCE_WAIT_STCOMM_STAGE1,						//3
	BALANCE_STCOMM_SHIFT_STAGE2,					//4
	BALANCE_WAIT_STCOMM_STAGE2,						//5
	BALANCE_STCOMM_SHIFT_EXECUTE_COMMAND,			//6
	BALANCE_WAIT_STCOMM_EXECUTE_COMMAND,			//7
	BALANCE_RDCOMM_SHIFT_STAGE1,					//8
	BALANCE_RDCOMM_WAIT_STCOMM_STAGE1,				//9
	BALANCE_WAIT_RDCOMM_COMPLETION1,				//A
	BALANCE_RDCOMM_SHIFT_STAGE2,					//b
	BALANCE_RDCOMM_WAIT_STCOMM_STAGE2,				//c
	BALANCE_WAIT_RDCOMM_COMPLETION2					//d
} LTC6804BalanceStateMachine;

typedef enum
{
	StringNotConnected,
	StringConnected
} StringConnectionStatus;

typedef enum
{
	NoLeakageAlarm,
	LeakageAlarmHappened
}LeakageAlarm;

typedef enum
{
	LeakageMeasNotStarted,
	LeakageWaitBeforeBatPlusMeasurement,
	LeakageSSRBatPlusActive,
	LeakageWaitBeforeSSRCommonActive,
	LeakageSSRCommonActive,
	LeakageWaitBeforeBatPlusAndCommonActive,
	LeakageSSRBatPlusAndCommonActive,
	LeakageSSRDelay,
	LeakageSSRMeasCompleted
}LeakageStateMachine;

typedef enum
{
	POWER_STATE_BOOT,
	POWER_STATE_WAIT_POWER_SUPPLY_RELIABLE,
	POWER_STATE_WAIT_ADC_RELIABLE_RESULT, //find rack and master revisions
	POWER_STATE_ACTIVE,
	POWER_STATE_WAIT_AFTER_5V_DEACTIVATION,
	POWER_STATE_WAIT_AFTER_3V3_DEACTIVATION,
	POWER_STATE_OFF
}PowerStateMachine;

typedef enum
{
	BalancerOff,                 //in accordance with data sheet Table 5
	BalancerDischNonsync,
	BalancerDischSync,
	BalancerCharging
} BalancerChargingState;

typedef enum
{
	CellDCRCalculationNotPerformed,
	CellDCRCalculationStarted,
	CellDCRCalculationPerformed
}CellDCRCalculationMachine;

typedef enum
{
	StringCannotBeConnected,
	StringCanBeChargedOnly,
	StringCanBeChargedAndDischarged,
	StringCanBeDischargedOnly
}StringFunctionalState;

typedef enum
{
	StringVoltageDifferentialInRangeDisch,
	StringVoltageDifferentialNotInRangeDisch
}StringVoltageDifferentialDisch;

typedef enum
{
	StringVoltageDifferentialInRangeCharge,
	StringVoltageDifferentialNotInRangeCharge
}StringVoltageDifferentialCharge;

typedef enum
{
	ChargeCurrent0Allowed,
	ChargeCurrent25Allowed,
	ChargeCurrent50Allowed,
	ChargeCurrent75Allowed,
	ChargeCurrent100Allowed
} ChargeCurrentAllowedLevel;

//set if application requires to suspend passive balancing (leakage
//measurement in progress or other reason)
typedef enum 
{
	PASSIVE_BAL_NOT_SUSPENDED = 0,
	PASSIVE_BAL_SUSPENDED = 1
} PassiveBalTemporarySuspention;

typedef enum
{
	PowerOffReasonNotDefined,							//0
	PowerOffReasonUnknown,								//1
	PowerOffReasonButtonPressed,						//2
	PowerOffReasonEWSNotPresentTimeOut,					//3
	PowerOffReasonNoOperationTimeOut,					//4

	PowerOffReasonVoltageBelowDischargeThrRack1,		//5
	PowerOffReasonVoltageBelowDischargeThrRack2,		//6
	PowerOffReasonVoltageBelowDischargeThrRack3,		//7
	PowerOffReasonVoltageBelowDischargeThrRack4,		//8

	PowerOffReasonTemperatureBelowOperationThrRack1,	//9
	PowerOffReasonTemperatureBelowOperationThrRack2,	//10
	PowerOffReasonTemperatureBelowOperationThrRack3,	//11
	PowerOffReasonTemperatureBelowOperationThrRack4,	//12

	PowerOffReasonTemperatureAboveOperationThrRack1,	//13
	PowerOffReasonTemperatureAboveOperationThrRack2,	//14
	PowerOffReasonTemperatureAboveOperationThrRack3,	//15
	PowerOffReasonTemperatureAboveOperationThrRack4,	//16

	PowerOffReasonVoltageAboveOperationThrRack1,		//17
	PowerOffReasonVoltageAboveOperationThrRack2,		//18
	PowerOffReasonVoltageAboveOperationThrRack3,		//19
	PowerOffReasonVoltageAboveOperationThrRack4,		//20

	PowerOffReasonVoltageBelowOperationThrRack1,		//21
	PowerOffReasonVoltageBelowOperationThrRack2,		//22
	PowerOffReasonVoltageBelowOperationThrRack3,		//23
	PowerOffReasonVoltageBelowOperationThrRack4,		//24

	PowerOffReasonVoltageInvalidRack1,					//25
	PowerOffReasonVoltageInvalidRack2,					//26
	PowerOffReasonVoltageInvalidRack3,					//27
	PowerOffReasonVoltageInvalidRack4,					//28

	PowerOffReasonCurrentShortTimeRack1,				//29
	PowerOffReasonCurrentShortTimeRack2,				//30
	PowerOffReasonCurrentShortTimeRack3,				//31
	PowerOffReasonCurrentShortTimeRack4,				//32

	PowerOffReasonCurrentLongTimeRack1,					//33
	PowerOffReasonCurrentLongTimeRack2,					//34
	PowerOffReasonCurrentLongTimeRack3,					//35
	PowerOffReasonCurrentLongTimeRack4,					//36

	PowerOffReasonCurrentShortTimeUnit,					//37
	PowerOffReasonCurrentLongTimeUnit,					//38

	PowerOffReasonADCTimeOutRack1,						//39
	PowerOffReasonADCTimeOutRack2,						//40
	PowerOffReasonADCTimeOutRack3,						//41
	PowerOffReasonADCTimeOutRack4,						//42

	PowerOffReasonFuseRack1,							//43
	PowerOffReasonFuseRack2,							//44
	PowerOffReasonFuseRack3,							//45
	PowerOffReasonFuseRack4,							//46

	PowerOffReason3VRack1,								//47
	PowerOffReason3VRack2,								//48
	PowerOffReason3VRack3,								//49
	PowerOffReason3VRack4,								//50

	PowerOffReason5VRack1,								//51
	PowerOffReason5VRack2,								//52
	PowerOffReason5VRack3,								//53
	PowerOffReason5VRack4,								//54

	PowerOffReason24VRack1,								//55
	PowerOffReason24VRack2,								//56
	PowerOffReason24VRack3,								//57
	PowerOffReason24VRack4,								//58

	PowerOffReasonVrefRack1,							//59
	PowerOffReasonVrefRack2,							//60
	PowerOffReasonVrefRack3,							//61
	PowerOffReasonVrefRack4,							//62

	PowerOffReasonBoardIDRack1,							//63
	PowerOffReasonBoardIDRack2,							//64
	PowerOffReasonBoardIDRack3,							//65
	PowerOffReasonBoardIDRack4,							//66

	PowerOffReasonMMaker,								//67

	PowerOffReasonCellMonitoringRack1,					//68
	PowerOffReasonCellMonitoringRack2,					//69
	PowerOffReasonCellMonitoringRack3,					//70
	PowerOffReasonCellMonitoringRack4,					//71

	PowerOffReasonContactorNotOpenRack1,				//72
	PowerOffReasonContactorNotOpenRack2,				//73
	PowerOffReasonContactorNotOpenRack3,				//74
	PowerOffReasonContactorNotOpenRack4,				//75

	PowerOffReasonContactorNotClosedRack1,				//76
	PowerOffReasonContactorNotClosedRack2,				//77
	PowerOffReasonContactorNotClosedRack3,				//78
	PowerOffReasonContactorNotClosedRack4,				//79

	PowerOffReasonCommunBreakRack,						//80

	PowerOffReasonSFlashMaster,							//81

	PowerOffReasonRegenCurrentNegTemperatureRack1,		//82
	PowerOffReasonRegenCurrentNegTemperatureRack2,		//83
	PowerOffReasonRegenCurrentNegTemperatureRack3,		//84
	PowerOffReasonRegenCurrentNegTemperatureRack4,		//85

	PowerOffReasonRegenCurrentPosTemperatureRack1,		//86
	PowerOffReasonRegenCurrentPosTemperatureRack2,		//87
	PowerOffReasonRegenCurrentPosTemperatureRack3,		//88
	PowerOffReasonRegenCurrentPosTemperatureRack4,		//89

	PowerOffReasonUnrecognisedCharger,					//90

	PowerOffReasonNegContactorNotOpen,					//91
	PowerOffReasonNegContactorNotClosed,				//92
	LimitedOperationReasonVoltageDifferenceBetwStr,		//93
	PowerOffHWFailure,									//94
	PowerOffReasonContinuousCurrentUnit,				//95

	PowerOffReasonContinuousCurrentRack1,				//96
	PowerOffReasonContinuousCurrentRack2,				//97
	PowerOffReasonContinuousCurrentRack3,				//98
	PowerOffReasonContinuousCurrentRack4,				//99

	PowerOffReasonTransducerOffsetUnreliableRack1,		//100
	PowerOffReasonTransducerOffsetUnreliableRack2,		//101
	PowerOffReasonTransducerOffsetUnreliableRack3,		//102
	PowerOffReasonTransducerOffsetUnreliableRack4,		//103

	PowerOffReasonSafetyParamReceiveTimeOutRack1,		//104
	PowerOffReasonSafetyParamReceiveTimeOutRack2,		//105
	PowerOffReasonSafetyParamReceiveTimeOutRack3,		//106
	PowerOffReasonSafetyParamReceiveTimeOutRack4,		//107

	PowerOffReasonSafetyCurrentReceiveTimeOutRack1,		//108
	PowerOffReasonSafetyCurrentReceiveTimeOutRack2,		//109
	PowerOffReasonSafetyCurrentReceiveTimeOutRack3,		//110
	PowerOffReasonSafetyCurrentReceiveTimeOutRack4,		//111

	PowerOffReasonUnitContinuousOvercurrentRack1,		//112 - continuous over current for the unit
	PowerOffReasonUnitContinuousOvercurrentRack2,		//113 registered by the rack. Introduced as part
	PowerOffReasonUnitContinuousOvercurrentRack3,		//114 of safety related routines
	PowerOffReasonUnitContinuousOvercurrentRack4,		//115

	PowerOffReasonUnitShortOvercurrentRack1,			//116 - short time over current for the unit
	PowerOffReasonUnitShortOvercurrentRack2,			//117 registered by the rack. Introduced as part
	PowerOffReasonUnitShortOvercurrentRack3,			//118 of safety related routines
	PowerOffReasonUnitShortOvercurrentRack4,			//119

	PowerOffReasonAnalogModuleMuxFailureRack1,			//120
	PowerOffReasonAnalogModuleMuxFailureRack2,			//121
	PowerOffReasonAnalogModuleMuxFailureRack3,			//122
	PowerOffReasonAnalogModuleMuxFailureRack4,			//123

	PowerOffReasonCurrentTransFailureRack1,				//124
	PowerOffReasonCurrentTransFailureRack2,				//125
	PowerOffReasonCurrentTransFailureRack3,				//126
	PowerOffReasonCurrentTransFailureRack4,				//127

	PowerOffReasonStringVoltageDiffRack1,				//128
	PowerOffReasonStringVoltageDiffRack2,				//129
	PowerOffReasonStringVoltageDiffRack3,				//130
	PowerOffReasonStringVoltageDiffRack4,				//131

	PowerOffReasonBattModuleVregOneThirdFailureRack1,	//132
	PowerOffReasonBattModuleVregOneThirdFailureRack2,	//133
	PowerOffReasonBattModuleVregOneThirdFailureRack3,	//134
	PowerOffReasonBattModuleVregOneThirdFailureRack4,	//135

	PowerOffReasonBattModuleVregTwoThirdFailureRack1,	//136
	PowerOffReasonBattModuleVregTwoThirdFailureRack2,	//137
	PowerOffReasonBattModuleVregTwoThirdFailureRack3,	//138
	PowerOffReasonBattModuleVregTwoThirdFailureRack4,	//139

	PowerOffReasonBattModuleGndMeasureFailureRack1,		//140
	PowerOffReasonBattModuleGndMeasureFailureRack2,		//141
	PowerOffReasonBattModuleGndMeasureFailureRack3,		//142
	PowerOffReasonBattModuleGndMeasureFailureRack4,		//143

	PowerOffReasonChargeCurrentUnit,						//144
    PowerOffReasonExitFromMaintenance                   //145
} PowerOffReason;



typedef struct
{
	uint8_t ALIGNED_(1) buzzer[4];//4 complement to 4 bytes
	int16_t ALIGNED_(2) maxCellTemp;//6, for the pack
	int16_t ALIGNED_(2) globalPackCurrent;//8
	uint16_t ALIGNED_(2) voltEWSIntegrity;//10
	MasterOperationState ALIGNED_(1) masterOperationState;//11
	uint8_t ALIGNED_(1) numberOfStringsCannotBeConnected;//12
	uint8_t ALIGNED_(1) numberOfStringsCanBeChargedOnly;//13
	uint8_t ALIGNED_(1) numberOfStringsCanBeChargedAndDischarged;//14
	uint8_t ALIGNED_(1) numberOfStringsCanBeDischargedOnly;//15
	uint8_t ALIGNED_(1) numberOfStringsWithinDischargeThreshold;//16
	uint8_t ALIGNED_(1) numberOfStringsWithinChargeThreshold;//17
	uint8_t ALIGNED_(1) numberOfStringsCurrentlyConnected;//18

	uint8_t ALIGNED_(1) stateOfChargePack;//19, SOC for the pack, %
	PackCurrentStatus ALIGNED_(1) packCurrentStatusShortTime;//20
	PackCurrentStatus ALIGNED_(1) packCurrentStatusLongTime;//21
	ChargeStopReason ALIGNED_(1) chargeStopReason;//22
	CalibrationDataRenewal ALIGNED_(1) calibrationDataRenewal;//23
	BootLoaderActiveInfo ALIGNED_(1) bootLoaderActiveInfo;//24
} MasterStateHandlerStatus_t; //24


typedef struct
{
	CANHostState ALIGNED_(1) cAN1HostState;//1
	CANMasterToRackCommunication ALIGNED_(1) cANMasterToRackCommunication;//2
	uint8_t ALIGNED_(1) spare[2];//3, complement to 4 bytes
}CAN1HostCommunStatus_t;   //4

typedef struct
{
	uint16_t ALIGNED_(2) voltage_x10;//2
	uint16_t ALIGNED_(2) current_x10;//4, 5000 -> 500A
	uint16_t ALIGNED_(2) consumption_Ah;//6
	uint16_t ALIGNED_(2) chargeTime;//8
	uint16_t ALIGNED_(2) chargerCurrentCapabilityx10;//10, in 1/10A
	uint8_t ALIGNED_(1) chargerTemperature;//11
	uint8_t ALIGNED_(1) terminationCode;//12
	uint8_t ALIGNED_(1) exceptionCode;//13
	ChargerOperationState ALIGNED_(1) chargerOperationState;//14
	ChargerPresence ALIGNED_(1) chargerPresence;//15 show presence of the charger
	ChargerActivation ALIGNED_(1) chargerActivation;//16 show if charger is activated
	ChargerCurrentControl ALIGNED_(1) chargerCurrentControl;//17
	uint8_t ALIGNED_(1) numberOfStringsUnderCharge;//18
	uint8_t ALIGNED_(1) chargerType;//19
	uint8_t ALIGNED_(1) chargerName[CHARGER_NAME_LENGTH];//length 8, 27
	ChargerHmiStatus ALIGNED_(1) chargerHmiStatus;//28
}ChargerStatus_t; //28

typedef struct
{
	EWSActivation ALIGNED_(1) eWSActivation;//shows if EWS activated
	EWSReturn ALIGNED_(1) eWSReturn;//2 shows presence of the Forklift if EWSReturnHigh
	ContactorActivation miniContactorActivation;
	uint8_t ALIGNED_(1) spare1[1];//4 complement to 4 bytes
}EWSStatus_t;  //4


typedef struct
{
    uint8_t ALIGNED_(1) spare1[4];//4 complement to 4 bytes
    /// Indicates which device caused seriousErrorOccurred flag to
    /// be set to MB_SERIOUS_ERR_DEVICE_SHUTDOWN.  For all other
    /// values of seriousErrorOccurred, this member is cleared.
    /// The device IDs are bitwise-exclusive to make identifying the
    /// error-causing device simpler.
    CommonDeviceID_t ALIGNED_(4) seriousDeviceErrorBitmask;//8
    /// Indicates whether or not the firmware watchdog or host communication
    // watchdog caused a system restart
    FirmwareError_t ALIGNED_(2) seriousErrorOccurred;//10
    uint8_t ALIGNED_(1) isControlParametersRequestedFlag;//11
    uint8_t ALIGNED_(1) spare;
} BMSCtrlGeneralStatus_t; //12


typedef struct
{
    uint8_t ALIGNED_(1) spare1[4];//complement to 4 bytes
	ContactorActivation ALIGNED_(1) contactorActivationStatus; //5
	ContactorConnectionIndication ALIGNED_(1) contactorConnectionIndication;//6
	ContactorStates ALIGNED_(1) contactorState;//7
	uint8_t ALIGNED_(1) spare[1];//8, complement to 4 bytes
}ContactorStatus_t; //8

typedef struct
{
    uint8_t ALIGNED_(1) spare1[2];//complement to 4 bytes
    uint16_t ALIGNED_(2) regenCurrentAtLowTemperature;
    uint16_t ALIGNED_(2) stringCurrentDu;//6
    int16_t ALIGNED_(2) stringCurrentA;//8 String current during charge is positive
    int16_t ALIGNED_(2) stringCurrentAStorage[FAST_STORAGE_STACK_SIZE];//28, 20
       									//String current during charge is positive

    uint16_t ALIGNED_(2) voltBeforeFuse;//30, VBATTERY_LEAKAGE/45
    uint16_t ALIGNED_(2) voltAfterFuse;//32, VBAT_FUSE/45
    uint16_t ALIGNED_(2) voltAfterContactor;//34, V+/45
    uint16_t ALIGNED_(2) voltVREF;//36, ADC_CHK_LOW: VREF/3
    uint16_t ALIGNED_(2) volt3V3;//38
    uint16_t ALIGNED_(2) volt5V;//40
    uint16_t ALIGNED_(2) volt24V;//42
    int16_t ALIGNED_(2) TemperatureInternalTherm;//44
    uint16_t ALIGNED_(2) voltExtTherm1;//46
    uint16_t ALIGNED_(2) voltExtTherm2;//48
    uint16_t ALIGNED_(2) voltExtTherm3;//50
    uint16_t ALIGNED_(2) temperatureMicro;//52
    VoltageStatus ALIGNED_(1) voltageStatus3V3;//53
    VoltageStatus ALIGNED_(1) voltageStatus5V;//54
    VoltageStatus ALIGNED_(1) voltageStatus24V;//55
    VoltageStatus ALIGNED_(1) voltageStatusVREF;//56
    OutputState ALIGNED_(1) enableLeakageDetection0;//57
    OutputState ALIGNED_(1) enableLeakageDetection1;//58
    FuseStatus ALIGNED_(1) fuseStatus; //59, no separate device monitoring for fuse
    ChargeCurrentOnString ALIGNED_(1) chargeCurrentOnString; //60
    //uint8_t ALIGNED_(1) spare[1];//44, complement to 4 bytes
}AnalogInpStatus_t; //60

typedef struct
{
    int32_t ALIGNED_(4) leakRlm; //4
    int32_t ALIGNED_(4) leakRlp; //8
    int32_t ALIGNED_(4) rLeakage; //12
    int32_t ALIGNED_(2) leakageVoltMeasurementResults[LEAK_VOLT_MEAS_SIZE + 1];//28

    uint8_t ALIGNED_(1) leakWarning;//29
    uint8_t ALIGNED_(1) dspiInitStatus;//30
	//cell monitoring and balancing
    LTC6804InitStateMachine ALIGNED_(1) lTC6804InitStateMachine;//31
    LTC6804NoBalanceStateMachine ALIGNED_(1) lTC6804NoBalanceStateMachine;//32
    LTC6804BalanceStateMachine ALIGNED_(1) lTC6804BalanceStateMachine;//33
    StringConnectionStatus ALIGNED_(1) stringConnectionStatus;//34
    OperationState ALIGNED_(1) operationState;//35
    CELL_SCAN_TASK_ID ALIGNED_(1) cellPollTaskState;//36
    uint8_t ALIGNED_(1) analogChannelIndex;//37

    BstringCommunication ALIGNED_(1) bStringCommunication;//38communication active, not active or failure 46
    uint8_t ALIGNED_(1) numberOfStringsCannotBeConnected; //39
    uint8_t ALIGNED_(1) numberOfStringsCanBeChargedOnly; //40
    uint8_t ALIGNED_(1) numberOfStringsCanBeChargedAndDischarged; //41
    uint8_t ALIGNED_(1) numberOfStringsCanBeDischargedOnly; //42

    LeakageAlarm ALIGNED_(1) leakageAlarm; //43
    LeakageStateMachine ALIGNED_(1) leakageStateMachine; //44
    PowerStateMachine powerStateMachine;//45
    uint8_t ALIGNED_(1) bStringNumber; //46 number of battery string to be monitored. Required for communication
    AutoBalancingStateMachine ALIGNED_(1) autoBalancingStateMachine;//47

    uint8_t ALIGNED_(1) tempSensorIssueFlag;  //48
	//index 0 - index of string with highest voltage
} StateHandlerStatus_t; //48

typedef struct
{
    uint32_t ALIGNED_(4) balancingTime;//0+4=4 in 200ms
    BalancerChargingState ALIGNED_(1) balancerChargingState; //4+1=5
    uint8_t ALIGNED_(1) filler[3]; //5+3=8
} BalancerActiveState_t; //8 corresponds to BALANCER_ACTIVE_STATE_TYPE

typedef struct
{
    uint8_t ALIGNED_(1) spare1[4];//complement to 4 bytes
	//for LTC3300, start
    BalancerActiveState_t ALIGNED_(4) balancerActiveState[MAX_CELL_NUM_LTC];//100, 8*12 = 96
    int32_t ALIGNED_(4) stringVoltageStorage[FAST_STORAGE_STACK_SIZE];//140, 4*10 = 40
    int32_t ALIGNED_(4) stringVoltage;//144

    uint16_t ALIGNED_(2) cellVolt[MAX_CELL_NUM_LTC];//168 144+24=168

    int16_t ALIGNED_(2) cellVoltCompensation[MAX_CELL_NUM_LTC];//192, 168+24=192 cellDCR * stringChargeCurrent

    int16_t ALIGNED_(2) cellVoltBalancingDelta[MAX_CELL_NUM_LTC];//216, 192+24=216

    int16_t ALIGNED_(2) cellTemp[MAX_THERMISTORS_PER_CELL][MAX_CELL_NUM_LTC];//264 216+48=264

	//for LTC3300, start
    uint16_t ALIGNED_(2) balancerICstatus[LTC3300_CONFIG_NUM_ICS_PER_ADDRESS];//268, 264+4=268 0->cells0...5
    int16_t ALIGNED_(2) stringChargeCurrent;//270, string Charge Current used for cellVoltCompensation calculation

    uint16_t ALIGNED_(2) voltCritLowAlarm;//272, in place of voltageCriticalLowAlarm
    uint16_t ALIGNED_(2) voltDischargeLevelFlag;//274, in place of voltageDischargeLevelFlag
    uint16_t ALIGNED_(2) voltUndervWarningFlag;//276, in place of voltageUndervWarningFlag
    uint16_t ALIGNED_(2) voltHighWarningFlag;//278, in place of voltageHighWarningFlag
    uint16_t ALIGNED_(2) voltCriticalHighAlarm;//280, in place of voltageCriticalHighAlarm
    uint16_t ALIGNED_(2) chCurrentLimitation;//282, in place of chargeCurrentLimitation
    uint16_t ALIGNED_(2) voltHighAlarm;//284, in place of voltageHighAlarm
    uint16_t ALIGNED_(2) voltInvalidAlarm;//286, in place of voltageInvalidAlarm
    uint16_t ALIGNED_(2) voltDiffAlarm;//288, in place of voltageDiffAlarm
    uint16_t ALIGNED_(2) voltStopChargeFlag;//290
    uint16_t ALIGNED_(2) voltDisallowCalibrationFlag;//292. If flag not set, calibration allowed

    uint16_t ALIGNED_(2) temperCriticalHighAlarm[MAX_THERMISTORS_PER_CELL];//296, in place of temperatureCriticalHighAlarm
    uint16_t ALIGNED_(2) temperZeroAlarm[MAX_THERMISTORS_PER_CELL];//300, in place of temperatureZeroAlarm
    uint16_t ALIGNED_(2) temperCriticalLowAlarm[MAX_THERMISTORS_PER_CELL];//304, in place of temperatureCriticalLowAlarm
    uint16_t ALIGNED_(2) temperBelowMinus5[MAX_THERMISTORS_PER_CELL];//308, temperLowWarning
    uint16_t ALIGNED_(2) temperAbove40[MAX_THERMISTORS_PER_CELL];//312 temperHighChargeAlarm
    uint16_t ALIGNED_(2) vRegLTC6804;//314, VReg voltage on LTC6804. CH31 on Analog mux

    CellDCRCalculationMachine ALIGNED_(1) cellDCRCalculationMachine;//315

    StringFunctionalState ALIGNED_(1) stringFunctionalState;//316
    StringVoltageDifferentialDisch ALIGNED_(1) stringVoltageDifferentialDisch;//317
    StringVoltageDifferentialCharge ALIGNED_(1) stringVoltageDifferentialCharge;//318
    uint8_t ALIGNED_(1) pECErrorCounter;//319
    uint8_t ALIGNED_(1) ADCovertId;//320, index of the temperature channels, 0 to 7

    StringVoltageCriticalHigh ALIGNED_(1) stringVoltageCriticalHigh;//321
    ChargeCurrentAllowedLevel ALIGNED_(1) chargeCurrentAllowedLevel;   //322
	uint8_t ALIGNED_(1) Spare[2];     //324   added 2 dummy bytes for identifying structure alignment to 4 byte boundary
} BatteryStringStatus_t; //324


typedef struct _PassiveBalancingStatus_t
{
	int16_t ALIGNED_(2) onBoardTemperature[TOTAL_24V_ONBOARD_TEMP_SENSORS];//0+2*6=12
	uint16_t ALIGNED_(2) balancingBitMask;//12+2=14
	uint16_t ALIGNED_(2) voltForBalancingTooLow;//14+2=16
	uint8_t ALIGNED_(1) isPassiveBalancingConditionsMet; //16+1=17
	uint8_t ALIGNED_(1) balancingThrottlingValue; //17+1=18
	PassiveBalTemporarySuspention ALIGNED_(1) passiveBalTemporarySuspention;//18+1=19
	uint8_t ALIGNED_(1) spare[1];//19+1=20
} PassiveBalancingStatus_t; //20

typedef struct
{
    BMSCtrlGeneralStatus_t ALIGNED_(4) generalStatus;//12
    ContactorStatus_t ALIGNED_(4) contactorStatus; //12+8=20
    AnalogInpStatus_t ALIGNED_(4) analogInpStatus;//20+60=80
    StateHandlerStatus_t ALIGNED_(4) stateHandlerStatus;////48+80=128
    BatteryStringStatus_t ALIGNED_(4) batteryStringStatus[MAX_BATTERY_STRINGS_NUMBER];////128+324=452
    PassiveBalancingStatus_t ALIGNED_(4) passiveBalancingStatus;//468, 452+20=472
} BMSCtrlStatus_t;//472

typedef struct
{
	MasterMakerGeneralStatus_t masterMakerGeneralStatus; //12
	MasterStateHandlerStatus_t masterStateHandlerStatus; //12 +24=36
	CAN1HostCommunStatus_t cAN1HostCommunStatus;//36+4=40
	ContactorStatus_t negativeContactorStatus;//40+8=48
	ChargerStatus_t chargerStatus;//48+28=76
	EWSStatus_t eWSStatus;//76+4=80
	BMSCtrlStatus_t bMSCtrlStatus[MAX_RACKS_FOR_FORKLIFT];//80+(472*4)=1968
} MasterMakerStatus_t;//1968

#pragma pack(pop)

