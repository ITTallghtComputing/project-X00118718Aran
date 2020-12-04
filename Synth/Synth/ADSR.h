#pragma once

struct ADSREnvelope {
	double attack;
	double decay;
	double sustain;
	double release;
	double startingAmp;
	double offTime;
	double onTime;
	bool noteOn;

	ADSREnvelope() {
		attack = 0.1;
		decay = 0.01;
		sustain = 0.8;
		release = 0.2;
		noteOn = false;
		offTime = 0.0;
		onTime = 0.0;
	}

	void KeyPress(double timeOn) {
		onTime = timeOn;
		noteOn = true;
	}

	void KeyDepress(double timeOff) {
		offTime = timeOff;
		noteOn = false;
	}

	double GetAmplitude(double dTime)
	{
		double amplitude = 0.0;
		double lifeTime = dTime - onTime;

		if (noteOn)
		{
			if (lifeTime <= attack)
			{
				// In attack Phase - approach max amplitude
				amplitude = (lifeTime / attack) * startingAmp;
			}

			if (lifeTime > attack && lifeTime <= (attack + decay))
			{
				// In decay phase - reduce to sustained amplitude
				amplitude = ((lifeTime - attack) / decay) * (sustain - startingAmp) + startingAmp;
			}

			if (lifeTime > (attack + decay))
			{
				// In sustain phase - dont change until note released
				amplitude = sustain;
			}
		}
		else
		{
			// Note has been released, so in release phase
			amplitude = ((dTime - offTime) / release) * (0.0 - sustain) + sustain;
		}

		// Amplitude should not be negative
		if (amplitude <= 0.0001)
			amplitude = 0.0;

		return amplitude;
	}
};
