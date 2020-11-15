#include <iostream>
#include "audioOut.h"
using namespace std;

const double twelfthRootOfTwo = 1.05946309436;

double MakeNoise1(double dTime) {
	return 1.0 * sin(440.00 * 2 * 3.14159 * dTime);
}


int main() {

	vector<wstring> devices = audioOut<short>::Enumerate();

	for (auto d : devices) wcout << "found output device: " << d << endl;

	audioOut<short> sound(devices[0], 44100, 1, 8, 512);

	sound.SetUserFunction(MakeNoise1);
	
	bool check = true;
	while (check == true) {
		cout << "enter 0 to exit" << endl;
		cin >> check;
	}
	return 0;
}