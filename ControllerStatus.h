#pragma once
#include <iostream>

using namespace std;

class ControllerStatus {
public:
	bool connected;
	bool buttonA, buttonB, buttonX, buttonY;
	bool padLeft, padRight, padUp, padDown;
	bool buttonL, buttonR, buttonZ, buttonStart;

	double mainStickHorizontal, mainStickVertical;
	double cStickHorizontal, cStickVertical;

	double triggerL, triggerR;
	ControllerStatus();
};
inline ostream& operator<<(ostream & string, ControllerStatus const &v) {
	string << "{ connected: " << v.connected << ","
		<< "buttonA: " << v.buttonA << ", buttonB: " << v.buttonB << ", buttonX: " << v.buttonX << ", buttonY:" << v.buttonY << ", "
		<< "buttonL: " << v.buttonL << ", buttonR: " << v.buttonR << ", buttonZ: " << v.buttonZ << ", buttonSTART:" << v.buttonStart << ", "
		<< "mainStickHorizontal: " << v.mainStickHorizontal << ", mainStickVertical:" << v.mainStickVertical << ", "
		<< "cStickHorizontal" << v.cStickHorizontal << ", cStickVerticalAxis" << v.cStickVertical << ", "
		<< "triggerL" << v.triggerL << endl << ", triggerR:" << v.triggerR << " }";

	return string;
};
inline ostream& operator<=(ostream & string, ControllerStatus const &v) {
	string << "Connected: " << v.connected << "," << endl
		<< "A: " << v.buttonA << ", B: " << v.buttonB << ", X: " << v.buttonX << ", Y:" << v.buttonY << "," << endl
		<< "L: " << v.buttonL << ", R: " << v.buttonR << ", Z: " << v.buttonZ << ", START:" << v.buttonStart << "," << endl << endl
		<< "Main Stick horizontal axis: " << v.mainStickHorizontal << endl << "Main stick vertical axis:" << v.mainStickVertical << endl << endl
		<< "C-Stick horizontal axis: " << v.cStickHorizontal << endl << "C-Stick vertical axis:" << v.cStickVertical << endl << endl
		<< "L Trigger axis: " << v.triggerL << endl << "R Trigger axis:" << v.triggerR << endl << endl;

	return string;
}