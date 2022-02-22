#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "test/catch.hpp"
#include "MonitoringAndTypewiseAlert.h"

int printCallCount = 0;

void fakePrint(const char* printStatement) {
	printf("%s \n", printStatement);
	printCallCount++;	
}

TEST_CASE("Checks if the value is below given limit") {
	REQUIRE(checkIfValueIsBelowLimit(12,30) == true);
	REQUIRE(checkIfValueIsBelowLimit(42,30) == false);
}

TEST_CASE("Checks if the value is above given limit") {
	REQUIRE(checkIfValueIsAboveLimit(12,30) == false);
	REQUIRE(checkIfValueIsAboveLimit(42,30) == true);
}

TEST_CASE("Checks if the values is within given limits") {
	REQUIRE(checkIfValueIsWithinLimits(12,0,30) == true);
	REQUIRE(checkIfValueIsWithinLimits(42,0,30) == false);
}

TEST_CASE("Infers the breach type according to limits") {
	REQUIRE(inferBreach(12, 20, 30) == TOO_LOW);
	REQUIRE(inferBreach(22, 20, 30) == NORMAL);
	REQUIRE(inferBreach(32, 20, 30) == TOO_HIGH);
}

TEST_CASE("Checks if the given index matches with the given cooling type") {
	REQUIRE(checkIfCoolingTypeMatches(PASSIVE_COOLING, 0) == true);
	REQUIRE(checkIfCoolingTypeMatches(HI_ACTIVE_COOLING, 2) == false);
}

TEST_CASE("Classifies the temperature type and infers the breach type") {
  REQUIRE(classifyTemperatureBreach(PASSIVE_COOLING, -10) == TOO_LOW);
}

TEST_CASE("Checks if the given index matches with the given alert type") {
	REQUIRE(checkIfAlertTargetMatches(TO_CONTROLLER, 0) == true);
	REQUIRE(checkIfAlertTargetMatches(TO_CONTROLLER, 1) == false);
}

TEST_CASE("check if the given index matches with given breach type") {
	REQUIRE(checkIfBreachTypeMatches(TOO_HIGH,1) == true);
	REQUIRE(checkIfBreachTypeMatches(TOO_HIGH,2) == false);
}

TEST_CASE("Send alert to controller") {
	printCallCount = 0;
	sendToController(TOO_HIGH, print);
	sendToController(TOO_HIGH, fakePrint);
	REQUIRE(printCallCount == 1);
}

TEST_CASE("Send alert to EMAIL") {
	printCallCount = 0;
	sendToEmail(TOO_LOW, fakePrint);
	REQUIRE(printCallCount == 2);
}

TEST_CASE("Send Email") {
	printCallCount = 0;
	sendAlertEmail(true,1,fakePrint,"a.b@c.com");
	REQUIRE(printCallCount == 2);
}
	
TEST_CASE("Check the value and alert for Passive Cooling") {
	BatteryCharacter batteryCharacter;
	batteryCharacter.coolingType = PASSIVE_COOLING;
	printCallCount = 0;
	checkAndAlert(TO_CONTROLLER, batteryCharacter, -10, print);
	checkAndAlert(TO_EMAIL, batteryCharacter, -10, fakePrint);
	checkAndAlert(TO_EMAIL, batteryCharacter, 10, fakePrint);
	checkAndAlert(TO_CONTROLLER, batteryCharacter, 40, fakePrint);
	REQUIRE(printCallCount == 3);
}

