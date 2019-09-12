// MAX17048/49 library for Arduino
//
// Created by Luca Dentella (http://www.lucadentella.it)
// Modified by Ali Kianzadeh

#include "MAX17048.h"
#include "Wire.h"

float MAX17048::getVCell() {

	byte MSB = 0;
	byte LSB = 0;
	
	readRegister(VCELL_REGISTER, MSB, LSB);
	int value = (MSB << 4) | (LSB >> 4);
	return map(value, 0x000, 0xFFF, 0, 50000) / 10000.0;
	//return value * 0.00125;
}

float MAX17048::getSoC() {
	
	byte MSB = 0;
	byte LSB = 0;
	
	readRegister(SOC_REGISTER, MSB, LSB);
	float decimal = LSB / 256.0;
	return MSB + decimal;	
}

int MAX17048::getVersion() {

	byte MSB = 0;
	byte LSB = 0;
	
	readRegister(VERSION_REGISTER, MSB, LSB);
	return (MSB << 8) | LSB;
}

byte MAX17048::getCompensateValue() {

	byte MSB = 0;
	byte LSB = 0;
	
	readConfigRegister(MSB, LSB);
	return MSB;
}

byte MAX17048::getAlertThreshold() {

	byte MSB = 0;
	byte LSB = 0;
	
	readConfigRegister(MSB, LSB);	
	return 32 - (LSB & 0x1F);
}

void MAX17048::setAlertThreshold(byte threshold) {

	byte MSB = 0;
	byte LSB = 0;
	
	readConfigRegister(MSB, LSB);	
	if(threshold > 32) threshold = 32;
	threshold = 32 - threshold;
	
	writeRegister(CONFIG_REGISTER, MSB, (LSB & 0xE0) | threshold);
}

boolean MAX17048::inAlert() {

	byte MSB = 0;
	byte LSB = 0;
	
	readConfigRegister(MSB, LSB);	
	return LSB & 0x20;
}

void MAX17048::clearAlert() {

	byte MSB = 0;
	byte LSB = 0;
	
	readConfigRegister(MSB, LSB);	
}

void MAX17048::reset() {
	
	writeRegister(COMMAND_REGISTER, 0x00, 0x54);
}

void MAX17048::quickStart() {
	
	writeRegister(MODE_REGISTER, 0x40, 0x00);
}


void MAX17048::readConfigRegister(byte &MSB, byte &LSB) {

	readRegister(CONFIG_REGISTER, MSB, LSB);
}

void MAX17048::readRegister(byte startAddress, byte &MSB, byte &LSB) {

	Wire.beginTransmission(MAX17048_ADDRESS);
	Wire.write(startAddress);
	Wire.endTransmission();
	
	Wire.requestFrom(MAX17048_ADDRESS, 2);
	MSB = Wire.read();
	LSB = Wire.read();
}

void MAX17048::writeRegister(byte address, byte MSB, byte LSB) {

	Wire.beginTransmission(MAX17048_ADDRESS);
	Wire.write(address);
	Wire.write(MSB);
	Wire.write(LSB);
	Wire.endTransmission();
}
