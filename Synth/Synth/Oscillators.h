#pragma once
#include "audioOut.h"

double w(double dHertz) //converts herts to angular velocity
{
	return dHertz * 2.0 * PI;
}

#define OSC_SINE 0
#define OSC_SQUARE 1
#define OSC_TRIANGLE 2
#define OSC_SAW_ANA 3
#define OSC_SAW_DIG 4
#define OSC_NOISE 5

double osc(double hertz, double time, int type = OSC_SINE)
{
	switch (type)
	{
	case OSC_SINE: // Sine wave
		return sin(w(hertz) * time);

	case OSC_SQUARE: // Square wave
		return sin(w(hertz) * time) > 0 ? 1.0 : -1.0;

	case OSC_TRIANGLE: // Triangle wave
		return asin(sin(w(hertz) * time)) * (2.0 / PI);

	case OSC_SAW_ANA: // Saw wave analogue style
	{
		double dOutput = 0.0;

		for (double n = 1.0; n < 40.0; n++)
			dOutput += (sin(n * w(hertz) * time)) / n;

		return dOutput * (2.0 / PI);
	}

	case OSC_SAW_DIG: // saw wave digitalized
		return (2.0 / PI) * (hertz * PI * fmod(time, 1.0 / hertz) - (PI / 2.0));


	case OSC_NOISE: //noise generator
		return 2.0 * ((double)rand() / (double)RAND_MAX) - 1.0;

	default:
		return 0.0;
	}
}