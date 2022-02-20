#include "MonitoringAndTypewiseAlert.h"
#include <stdio.h>
#include <string.h>

bool checkIfValueIsBelowLimit(double value, double limit) {
	return (value < limit) ? true : false;
}

bool checkIfValueIsAboveLimit(double value, double limit) {
	return (value > limit) ? true : false;
}

bool checkIfValueIsWithinLimits(double value, double lowerLimit, double upperLimit) {
	return (value > lowerLimit && value < upperLimit) ? true : false;
}

void checkBreachStatus(bool breachStatus, BreachType breachTypeValidated, BreachType *valueBreachType) {
	if (breachStatus == true)
		*valueBreachType = breachTypeValidated;
}

void inferLowBreach(double value, double lowerLimit, BreachType *valueBreachType) {
	bool breachStatus;
	breachStatus = checkIfValueIsBelowLimit(value, lowerLimit);
	checkBreachStatus(breachStatus, TOO_LOW, valueBreachType);
}

void inferHighBreach(double value, double upperLimit, BreachType *valueBreachType) {
	bool breachStatus;
	breachStatus = checkIfValueIsAboveLimit(value, upperLimit);
	checkBreachStatus(breachStatus, TOO_HIGH, valueBreachType);
}

void inferNoBreach(double value, double lowerLimit, double upperLimit, BreachType *valueBreachType) {
	bool breachStatus;
	breachStatus = checkIfValueIsWithinLimits(value, lowerLimit, upperLimit);
	checkBreachStatus(breachStatus, NORMAL, valueBreachType);
}

BreachType inferBreach(double value, double lowerLimit, double upperLimit) {
	BreachType valueBreachType;
	inferLowBreach(value, lowerLimit, &valueBreachType);
	inferHighBreach(value, upperLimit, &valueBreachType);
	inferNoBreach(value, lowerLimit, upperLimit, &valueBreachType);
	return valueBreachType;
}

CoolingTypeLimitPair CoolingTypeLimitPairs[] = {
{PASSIVE_COOLING,{0,35}},
{HI_ACTIVE_COOLING,{0,45}},
{MED_ACTIVE_COOLING,{0,40}},
};

bool checkIfCoolingTypeMatches(CoolingType coolingType, int index) {
	return (coolingType == CoolingTypeLimitPairs[index].coolingType) ? true : false;
}
	
BreachType classifyTemperatureBreach(
	CoolingType coolingType, double temperatureInC) {
	int lowerLimit = 0;
	int upperLimit = 0;
	int matchingIndex, numberOfCoolingTypes=3;

	for(int i=0;i<numberOfCoolingTypes;i++){
		matchingIndex = checkIfCoolingTypeMatches(coolingType, i);
		if(matchingIndex){
			lowerLimit = CoolingTypeLimitPairs[i].valueLimits.lowerLimit;
			upperLimit = CoolingTypeLimitPairs[i].valueLimits.upperLimit;
		}
	}
	return inferBreach(temperatureInC, lowerLimit, upperLimit);
}

void print(const char* printStatement) {
	printf("%s \n", printStatement);
}

AlertTargetAndAlertPair alertTargetAndAlertPairs[] = {
	{TO_CONTROLLER, sendToController},
	{TO_EMAIL, sendToEmail},
};
 
bool checkIfAlertTargetMatches(AlertTarget alertTarget, int index) {
	return (alertTarget == alertTargetAndAlertPairs[index].alertTarget) ? true : false;
}

void checkAndAlert(
    AlertTarget alertTarget, BatteryCharacter batteryChar, double temperatureInC, void (*fpAlertPrint) (const char*)) {
	int matchingIndex, numberOfAlertTargets=2;
	BreachType breachType = classifyTemperatureBreach(
    	batteryChar.coolingType, temperatureInC);
	for(int i=0; i<numberOfAlertTargets; i++){
		matchingIndex = checkIfAlertTargetMatches(alertTarget, i);
		if(matchingIndex) {
			alertTargetAndAlertPairs[i].fpSendAlert(breachType, fpAlertPrint);
		}
  }
}

void convertBreachTypeToString(BreachType breachType, char* breachTypeString, char* header){
	sprintf(breachTypeString,"%d",breachType);
	strcat(header, breachTypeString);
}
	
void sendToController(BreachType breachType, void (*fpAlertPrint) (const char*)) {
	char breachTypeString[10];
	char header[5] = "feed";
	convertBreachTypeToString(breachType, breachTypeString, header);
  	fpAlertPrint(header);
}


breachTypeAlertStatementPair breachTypeAlertStatementPairs[] = {
	{TOO_LOW, "Hi, the temperature is too low"},
	{TOO_HIGH, "Hi, the temperature is too high"},
	{NORMAL, "h"},
}; 

bool checkIfBreachTypeMatches(BreachType breachType, int index) {
	return (breachType == breachTypeAlertStatementPairs[index].breachType) ? true : false;
}

void sendToEmail(BreachType breachType, void (*fpAlertPrint) (const char*)){
  const char* recepient = "a.b@c.com";
	int matchingIndex, numberOfBreachTypes = 3;
	for(int i=0; i<numberOfBreachTypes; i++){
		matchingIndex = checkIfBreachTypeMatches(breachType, i);
		// Don't alert for NORMAL breach type (i.e value=2)
		if(matchingIndex && (i != 2)){
			fpAlertPrint(recepient);
			fpAlertPrint(breachTypeAlertStatementPairs[i].alertStatement);
			break;
		}
  }
}

