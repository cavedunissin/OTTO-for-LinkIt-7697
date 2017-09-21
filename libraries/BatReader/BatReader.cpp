/******************************************************************************
* Zowi Battery Reader Library
* 
* @version 20150831
* @author Raul de Pablos Martin
*
******************************************************************************/
// modifed for LinkIt 7697

#include "BatReader.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

BatReader::BatReader() {
}

double BatReader::readBatVoltage(void) {
    // read ADC value and convert to voltage 
    // LinkIt 7697's ADC range is 0~2.5V (0~4095)
    // LinkIt 7697 has a diver circuit
    
	// readed = ((R1+R2)/R2)*adcValue*2.5/4096
	// R1 = 200
	// R2 = 100
	double readed = (double)(analogRead(BAT_PIN)*0.0018315);
 	
	if(readed > BAT_MAX) return BAT_MAX;
	else return readed;
}

double BatReader::readBatPercent(void) {
	double value = (SLOPE*readBatVoltage()) - OFFSET;
	if(value < 0) return 0;
	else return value;
}

