#include <iostream>
#include "audioOut.h"
#include "ADSR.h"
#include "Oscillators.h"
using namespace std;

atomic<double> frequencyOutput = 0.0;
ADSREnvelope envelope;
double octaveBaseFrequency = 110.0;
const double twelfthRootOfTwo = 1.05946309436;





double MakeNoise(double dTime)
{
	// Mixed sine and square waves
	double dOutput = envelope.GetAmplitude(dTime) *
		(+1.0 * osc(frequencyOutput * 0.5, dTime, OSC_SINE) + 1.0 * osc(frequencyOutput, dTime, OSC_SAW_ANA));
	return dOutput * 0.4; // Master Volume
}


int main() {

	vector<wstring> devices = audioOut<short>::Enumerate(); //find output device

	for (auto d : devices) wcout << "found output device: " << d << endl;

	audioOut<short> sound(devices[0], 44100, 1, 8, 512); //creates an object of the AudioOut Class

	sound.SetUserFunction(MakeNoise);

	cout << "_____________________________________________________________\n"
			"|    | |    |    | |   | |    |    | |   | |   | |    |     |\n"
			"|    | |    |    | |   | |    |    | |   | |   | |    |     |\n"
			"|    |S|    |    |F|   |G|    |    |J|   |K|   |L|    |     |\n"
			"|     -     |     -     -     |     -     -     -     |     |\n"
			"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |\n"
			"-------------------------------------------------------------\n";

		



	
	int currentKey = -1;
	bool keyPressed = false;
	while (1)
	{
		keyPressed = false;
		for (int k = 0; k < 16; k++)
		{
			if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe\xbf"[k])) & 0x8000)
			{
				if (currentKey != k)
				{
					frequencyOutput = octaveBaseFrequency * pow(twelfthRootOfTwo, k);
					envelope.KeyPress(sound.GetTime());
					wcout << "\rNote On : " << sound.GetTime() << "s " << frequencyOutput << "Hz";
					currentKey = k;
				}

				keyPressed = true;
			}
		}

		if (!keyPressed)
		{
			if (currentKey != -1)
			{
				wcout << "\rNote Off: " << sound.GetTime() << "s                        ";
				envelope.KeyDepress(sound.GetTime());
				currentKey = -1;
			}
		}
	}
	return 0;
}