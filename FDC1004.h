#ifndef __FDC1004_H__

#define __FDC1004_H__

#endif

#include <avr/io.h>

/*
*
*		Mux Enable lines
*
*/
#define ENABLE_1		PD3
#define ENABLE_2		PD4
#define ENABLE_3		PD5

/*
*
*		Register Pointers
*
*/
#define FDC1004_DEVICE_REG_ADDR	UINT8_C(0xFF)
#define FDC1004_FDC_CONF		UINT8_C(0x0C)
#define FDC1004_CONF_MEAS1		UINT8_C(0x08)
#define FDC1004_CONF_MEAS2		UINT8_C(0x09)
#define FDC1004_CONF_MEAS3		UINT8_C(0x0A)
#define FDC1004_CONF_MEAS4		UINT8_C(0x0B)


/*
*
*		Macros
*
*/

// I2C Address
#define I2C_READ_ADDR	0b10100001
#define I2C_Write_ADDR	0b10100000

// Measurement Type
#define SINGLE_ENDED	UINT8_C(0)
#define DIFFERENTIAL	UINT8_C(1)

//Measurement Data Rates
#define	FDC1004_R_100S			UINT8_C(0)
#define FDC1004_R_200S			UINT8_C(1)
#define FDC1004_R_400S			UINT8_C(2)

// Manufacture device ID
#define FDC1004_MAN_ID			UINT8_C(0x1004)


//Position and Mask Macros
#define FDC1004_CHA_POS			UINT16_C(13)
#define FDC1004_CHA_MASK		UINT16_C(0xE000)
#define FDC1004_CHB_POS			UINT16_C(10)
#define FDC1004_CHB_MASK		UINT16_C(0x1C00)
#define FDC1004_CAPDAC_POS		UINT16_C(5)
#define FDC1004_CAPDAC_MASK		UINT16_C(0x03E0)

#define FDC1004_RST_POS			UINT16_C(15)
#define FDC1004_RST_MASK		UINT16_C(0x8000)
#define FDC1004_RATE_POS		UINT16_C(10)
#define FDC1004_RATE_MASK		UINT16_C(0x0C00)
#define FDC1004_REPEAT_POS		UINT16_C(8)
#define FDC1004_REPEAT_MASK		UINT16_C(0x0100)
#define FDC1004_INIT1_POS		UINT16_C(7)
#define FDC1004_INIT1_MASK		UINT16_C(0x0080)
#define FDC1004_INIT2_POS		UINT16_C(6)
#define FDC1004_INIT2_MASK		UINT16_C(0x0040)
#define FDC1004_INIT3_POS		UINT16_C(5)
#define FDC1004_INIT3_MASK		UINT16_C(0x0020)
#define FDC1004_INIT4_POS		UINT16_C(4)
#define FDC1004_INIT4_MASK		UINT16_C(0x0010)
#define FDC1004_DONE1_POS		UINT16_C(3)
#define FDC1004_DONE1_MASK		UINT16_C(0x0008)
#define FDC1004_DONE2_POS		UINT16_C(2)	
#define FDC1004_DONE2_MASK		UINT16_C(0x0004)
#define FDC1004_DONE3_POS		UINT16_C(1)
#define FDC1004_DONE3_MASK		UINT16_C(0x0002)
#define FDC1004_DONE4_POS		UINT16_C(0)
#define FDC1004_DONE4_MASK		UINT16_C(0x0001)


//measurement configuration macros
//table 4

#define CHA_CIN1		UINT16_C(0x0000)
#define CHA_CIN2		UINT16_C(0x2000)
#define CHA_CIN3		UINT16_C(0x4000)
#define CHA_CIN4		UINT16_C(0x6000)

#define CHB_CIN1		UINT16_C(0x0000)
#define CHB_CIN2		UINT16_C(0x0400)
#define CHB_CIN3		UINT16_C(0x0800)
#define CHB_CIN4		UINT16_C(0x0C00)
#define CHB_CAPDAC		UINT16_C(0x1000)
#define CHB_DISABLED	UINT16_C(0x1C00)





/*
*
*		Function Prototypes
*	
*/

// Create an instance of a capacitive chip and tests communications with that chip
// assumes you have created an instance of a struct and pass the ID you want the chip to be known by
uint8_t FDC1004_Chip_Init(struct FDC1004_Dev *dev, uint8_t instanceID);


// Configures the measurement type for a given measurement id (1-4) for a given capacitive chip instance
// 
// Inputs - chip instance, measurement id, Capacitance offset value
uint8_t FDC1004_Measurement_Type_CAPDAC(struct FDC1004_Dev *dev, uint8_t MeasID, uint8_t CapOffset);

// Configures the measurement type for a given measurement id (1-4) for a given capacitive chip instance
// This function assumes CAPDAC is 0
//
// Inputs - chip instance, measurement id, (SINGLE_ENDED/DIFFERENTIAL),
uint8_t FDC1004_Measurement_Type(struct FDC1004_Dev *dev, uint8_t MeasID, uint8_t InputConfig);

// Creates a Measurement instance, creates an ID containing a Capacitive Chip instance and measurement ID
//
// Inputs - chip instance, measurement struct instance, ID to set to (plate number),
uint8_t FDC1004_Create_Measurement_Instance(struct FDC1004_Dev *dev, struct Measurement *inst, uint8_t Plate); //not sure about this, might not be necessary

// Writes to the FDC Register to begin a measurement
//
// Inputs - chip instance, measurement id
uint8_t FDC1004_Start_Measurement(struct FDC1004_Dev *dev, uint8_t MeasID);

// Writes to the FDC Register to set the data rate for the Chip
//
// Inputs - chip instance, (R_100S/R_200S/R_400S)
uint8_t FDC1004_Datarate_Set(struct FDC1004_Dev *dev, uint8_t DataRate);

// Reads from the FDC Register to get the Chips current Data Rate
//
// Inputs - chip instance, Pointer to temp storage
uint8_t FDC1004_Datarate_Get(struct FDC1004_Dev *dev, uint8_t *DataRate);

// Reads from the FDC Register to get the Chips measurement complete status
// for a given measurement register
//
// Inputs - chip instance, measurement ID
// Output - 0(not ready),1(ready), negative(error)
uint8_t FDC1004_Measurement_Complete(struct FDC1004_Dev *dev, uint8_t MeasID);

// Writes to the Offset Calibration Register to set the capacitive offset for the
// given application of the Capacitance Chip
//
// Inputs - chip instance, Capacitance Offset value
uint8_t FDC1004_Capacitance_Offset(struct FDC1004_Dev *dev, uint16_t CapOff);

// Reads from the Device ID Register to get the Chips Device ID
//
// Inputs - chip instance
uint8_t FDC1004_MaufactureID_Get(struct FDC1004_Dev *dev);



#define FDC1004_GET_BITS(bitname, x)				((x & bitname##_MASK) >> bitname##_POS)

#define FDC1004_SET_BITS(regvar, bitname, val)		((regvar & ~bitname##_MASK) | ((val << bitname##_POS) & bitname##_MASK))


// function pointer type definition
typedef uint8_t (*FDC1004_com_fptr_t)(uint8_t instance_id, uint8_t reg_addr, uint16_t *data);

struct  FDC1004_Dev
{
	uint8_t						chip_id;				//programmer created identification
	uint16_t					device_id;				//manufacturer defined ID
	FDC1004_com_fptr_t			read;
	FDC1004_com_fptr_t			write;
	struct Measurment_Plate_U				plates;
	struct FDC_Register			cap;
	struct Measurement_Configs_U	configs;
};

union Measurement_Configs_U
{
	struct Measurement_Configs
	{
		struct Measurement_Config	meas1;
		struct Measurement_Config	meas2;
		struct Measurement_Config	meas3;
		struct Measurement_Config	meas4;
	};
	struct Measurement_Config MeasArray[4];

};


//Capacitance to digital configuration
struct FDC_Register
{
	uint16_t	measure;
	uint16_t	init_4;
	uint16_t	init_3;
	uint16_t	init_2;
	uint16_t	init_1;
	uint16_t	repeat;
	uint16_t	rate;
	uint16_t	rst;
};

struct Measurement_Config 
{
	uint8_t		CHA;
	uint8_t		CHB;
	uint8_t		CAPDAC;
};

union Measurment_Plate_U
{
	struct Plates
	{
		struct Measurement plate1;
		struct Measurement plate2;
		struct Measurement plate3;
		struct Measurement plate4;
	};
	
	struct Measurement PlateArray[4];	
};


struct Measurement 
{
	uint8_t		plate_id;		
	uint8_t		channel_id;
	uint8_t		chip_id;
	uint16_t	result;
};