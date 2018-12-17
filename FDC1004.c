#include "FDC1004.h"
#include "I2C.h"

//functioned defined above "Init" are considered "Private" or "Internal" functions

uint8_t FDC_Read(uint8_t instance_id, uint8_t reg_addr, uint16_t *data)
{
	uint8_t dataH, dataL;
	
	//read the given instance ID and use that to select the appropriate Capacitance chip
	switch (instance_id)
	{
		case 1:
		PORTD &= ~((1<<ENABLE_1) | (1<<ENABLE_3) |(1<<ENABLE_2));
		PORTD |= (1<<ENABLE_1);
		
		break;
		
		case 2:
		PORTD &= ~((1<<ENABLE_1) | (1<<ENABLE_3) |(1<<ENABLE_2));
		PORTD |= (1<<ENABLE_2);
		
		break;
		
		case 3:
		PORTD &= ~((1<<ENABLE_1) | (1<<ENABLE_3) |(1<<ENABLE_2));
		PORTD |= (1<<ENABLE_3);
		
		break;
		
		default:
		return -1;
	}
	
	//assuming the appropriate chip is now selected
	
	
	I2C_Start();
	
	//Send write address, and prepare to write
	I2C_Send(I2C_Write_ADDR);
	
	//Write register to read from
	I2C_Send(reg_addr);
	
	
	I2C_Start();
	I2C_Send(I2C_READ_ADDR);
	
	// Read two 8 bit numbers to empty the FDC's output buffer
	dataH = I2C_ReadAck();
	dataL = I2C_ReadNack();
	
	I2C_Stop();
	
	//Convert to 16 bit number
	data = dataH * 256 + dataL;
	
	return 0;
}



uint8_t FDC_Write(uint8_t instance_id, uint8_t reg_addr, uint16_t *data)
{
	//read the given instance ID and use that to select the appropriate Capacitance chip	
	switch (instance_id)
	{
		case 1:
				PORTD &= ~((1<<ENABLE_1) | (1<<ENABLE_3) |(1<<ENABLE_2));
				PORTD |= (1<<ENABLE_1);
				
				break;
				
		case 2:
				PORTD &= ~((1<<ENABLE_1) | (1<<ENABLE_3) |(1<<ENABLE_2));
				PORTD |= (1<<ENABLE_2);
				
				break;
				
		case 3:
				PORTD &= ~((1<<ENABLE_1) | (1<<ENABLE_3) |(1<<ENABLE_2));
				PORTD |= (1<<ENABLE_3);
				
				break;
				
		default:
				return -1;
	}
	
	//assuming the appropriate chip is now selected
	
	I2C_Start();
	I2C_Send(I2C_Write_ADDR);
	I2C_Send(reg_addr);
	I2C_Send(((data >> 8) & 0xFF));
	I2C_Send(((data >> 0) & 0xFF));
	I2C_Stop();
}



// Reads from the Device ID Register to get the Chips Device ID
//
// Inputs - chip instance
uint8_t FDC1004_MaufactureID_Get(struct FDC1004_Dev *dev)
{
	
	//trying to pass in pointer to device id
	dev->read(dev->chip_id, FDC1004_DEVICE_REG_ADDR, &dev->device_id);
	
	return 0;
}


uint8_t FDC1004_Compile_Reg(static struct FDC1004_Dev *dev, uint8_t reg, uint16_t *C_data)
{
	switch(reg)
	{
		case FDC1004_FDC_CONF:
				C_data = dev->cap.init_1 | dev->cap.init_2 | dev->cap.init_3 | dev->cap.init_4 | dev->cap.rate | dev->cap.repeat | dev->cap.rst;
				break;
		
		case FDC1004_CONF_MEAS1:
				C_data = dev->configs.Measurement_Configs.meas1.CHA | dev->configs.Measurement_Configs.meas1.CHB | dev->configs.Measurement_Configs.meas1.CAPDAC;
				break;	
				
		case FDC1004_CONF_MEAS2:
				C_data = dev->configs.Measurement_Configs.meas2.CHA | dev->configs.Measurement_Configs.meas2.CHB | dev->configs.Measurement_Configs.meas2.CAPDAC;
				break;
		
		case FDC1004_CONF_MEAS3:
				C_data = dev->configs.Measurement_Configs.meas3.CHA | dev->configs.Measurement_Configs.meas3.CHB | dev->configs.Measurement_Configs.meas3.CAPDAC;
				break;
		
		case FDC1004_CONF_MEAS4:
				C_data = dev->configs.Measurement_Configs.meas4.CHA | dev->configs.Measurement_Configs.meas4.CHB | dev->configs.Measurement_Configs.meas4.CAPDAC;
				break;
		
	}
}

// Create an instance of a capacitive chip and tests communications with that chip
// assumes you have created an instance of a struct and pass the ID you want the chip to be known by
uint8_t FDC1004_Chip_Init(struct FDC1004_Dev *dev, uint8_t instanceID)
{
	int8_t rslt;
	
	
	dev->chip_id = instanceID;
	dev->read = FDC_Read;
	dev->write = FDC_Write;
	
	uint8_t try_count = 5;
	
	while(try_count)
	{
		rslt = FDC1004_MaufactureID_Get(&dev);
		if (rslt == 0)
		{
			if (dev->device_id == FDC1004_MAN_ID)
			{
				return 0;
			}
		}
		
		try_count--;
	}
	// Capacitance chip was unreachable or returned garbage send error
	return -1;
}


// Configures the measurement type for a given Channel input (1-4) for a given capacitive chip instance
//
// Inputs - chip instance, measurement id, Capacitance offset value
uint8_t FDC1004_Measurement_Type_CAPDAC(struct FDC1004_Dev *dev, uint8_t Channel, uint8_t CapOffset)
{
	if (CapOffset > 0)
	{
		//MeasArray is a memory Union with the configs struct
		//theoretically this allows incrementing though the struct like an array 
		dev->configs.MeasArray[(Channel - 1)]->CHB |= CHB_CAPDAC;
	}
	else
	{
		dev->configs.MeasArray[(Channel - 1)]->CHB |= CHB_DISABLED;
	}
	
	//
	//	add code to convert CapOffset to valid CAPDAC value
	//
	
	switch(Channel)
	{
		case 1:	
				dev->configs.MeasArray[(Channel - 1)]->CHA = CHA_CIN1;
				break;
				
		case 2:
				dev->configs.MeasArray[(Channel - 1)]->CHA = CHA_CIN2;
				break;
				
		case 3:
				dev->configs.MeasArray[(Channel - 1)]->CHA = CHA_CIN3;
				break;
				
		case 4:
				dev->configs.MeasArray[(Channel - 1)]->CHA = CHA_CIN4;
				break;
		default:
				return -1;
	}
	
	//CHA CHB and CAPDAC now configured return clear
	return 0;
}

//this function reads the data from a given register and stores it in the pointer passed in

uint8_t	FDC1004_Get_Reg(struct FDC1004_Dev *dev, uint8_t addr, uint16_t *data)
{
	uint8_t rslt;
	
	rslt = dev->read(dev->chip_id, addr, &data);
	
	return rslt;
}

//this function takes the data passed in via pointer and writes it to the given register
uint8_t FDC1004_Set_Reg(struct FDC1004_Dev *dev, uint8_t addr, uint16_t *data)
{
	uint8_t rslt;
	
	rslt = dev->write(dev->chip_id, addr, &data);
	
	return rslt;
}

uint8_t FDC1004_Create_Measurement_Instance(struct FDC1004_Dev *dev, uint8_t channel, uint8_t Plate)
{
	dev->plates.PlateArray[(channel - 1)]->channel_id = channel;
	dev->plates.PlateArray[(channel - 1)]->chip_id = dev->chip_id;
	dev->plates.PlateArray[(channel - 1)]->plate_id = Plate;
	
	return 0;
}

uint8_t FDC1004_Start_Measurement(struct FDC1004_Dev *dev, uint8_t MeasID)
{
	uint8_t temp;
	uint16_t val;
	
	temp = dev->plates.PlateArray[(MeasID - 1)]->channel_id;
	switch(temp)
	{
		case 1;
				dev->cap.init_1=FDC1004_SET_BITS(val,FDC1004_INI1,1);
				break;
				
		case 2:
				dev->cap.init_2=FDC1004_SET_BITS(val,FDC1004_INI2,1);
				break;
				
		case 3:
				dev->cap.init_3=FDC1004_SET_BITS(val,FDC1004_INI3,1);
				break;
				
		case 4:
				dev->cap.init_4=FDC1004_SET_BITS(val,FDC1004_INI4,1);
				break;
			
		temp = FDC1004_Compile_Reg(dev, FDC1004_FDC_CONF, &val);
		
		if (temp == 0)
		{
			temp = dev->write(dev->chip_id, FDC1004_FDC_CONF, val);
		}
			
		return temp;
				
	}			
	
}



uint8_t FDC1004_Datarate_Set(struct FDC1004_Dev *dev, uint8_t DataRate)
{
	uint16_t data;
	
	dev->cap.rate = DataRate;
	
	data = dev->cap.rate;
	
	FDC1004_Set_Reg(dev->chip_id, FDC1004_FDC_CONF, &data);
	
}