#pragma once
#pragma comment(lib, "winmm.lib")

#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>

using namespace std;

#include <Windows.h>

const double PI = 2.0 * acos(0.0);

template<class T>
class audioOut {
public:
	audioOut(wstring outDevice, unsigned int sampleRate = 44100, unsigned int channels = 1, unsigned int packets = 8, unsigned int packetSamples = 512) {
		Create(outDevice, sampleRate, channels, packets, packetSamples);
	}

	~audioOut()
	{
		Destroy();
	}

	//auto creation of an audio output object with all relevent information
	bool Create(wstring outDevice, unsigned int sampleRate = 44100, unsigned int channels = 1, unsigned int packets = 8, unsigned int packetSamples = 512) {
		abReady = false;
		iSampleRate = sampleRate;
		iChannels = channels;
		iPackets = packets;
		iPacketSamples = packetSamples;
		aiPacketFree = iPackets;
		iPacketCurrent = 0;
		cPacketMemory = nullptr;
		whWaveHeaders = nullptr;

		dUserFunction = nullptr;

		//Find OutDevice
		vector<wstring> devices = Enumerate();
		auto d = std::find(devices.begin(), devices.end(), outDevice);
		if (d != devices.end()) {
			int DeviceID = distance(devices.begin(), d);
			WAVEFORMATEX waveForm;
			waveForm.wFormatTag = WAVE_FORMAT_PCM;
			waveForm.nSamplesPerSec = iSampleRate;
			waveForm.wBitsPerSample = sizeof(T) * 8;
			waveForm.nChannels = iChannels;
			waveForm.nBlockAlign = (waveForm.wBitsPerSample / 8) * waveForm.nChannels;
			waveForm.nAvgBytesPerSec = waveForm.nSamplesPerSec * waveForm.nBlockAlign;
			waveForm.cbSize = 0;
			// Open Device if valid
			if (waveOutOpen(&hwDevice, DeviceID, &waveForm, (DWORD_PTR)waveOutProcWrap, (DWORD_PTR)this, CALLBACK_FUNCTION) != S_OK)
				return Destroy();
		}

			//allocate memory
			cPacketMemory = new T[iPackets * iPacketSamples];
			if (cPacketMemory == nullptr)
				return Destroy();
			ZeroMemory(cPacketMemory, sizeof(T) * iPackets * iPacketSamples);

			whWaveHeaders = new WAVEHDR[iPackets];
			if (whWaveHeaders == nullptr)
				return Destroy();
			ZeroMemory(whWaveHeaders, sizeof(WAVEHDR) * iPackets);

			for (unsigned int n = 0; n < iPackets; n++) {
				whWaveHeaders[n].dwBufferLength = iPacketSamples * sizeof(T);
				whWaveHeaders[n].lpData = (LPSTR)(cPacketMemory + (n * iPacketSamples));
			}

			abReady = true;
			tThread = thread(&audioOut::MainThread, this);

			unique_lock<mutex> lm(mutexPacketNotZero);
			cvPacketNotZero.notify_one();

			return true;
	}

	bool Destroy() {
		return false;
	}

	void Stop() {
		abReady = false;
		tThread.join();
	}

	virtual double UserProcess(double dTime) {
		return 0.0;
	}

	double GetTime() {
		return adGlobalTime;
	}

	static vector<wstring> Enumerate() {
		int iDeviceCount = waveOutGetNumDevs();
		vector<wstring> sDevices;
		WAVEOUTCAPS woc;
		for (int n = 0; n < iDeviceCount; n++) {
			if (waveOutGetDevCaps(n, &woc, sizeof(WAVEOUTCAPS)) == S_OK) {
				sDevices.push_back(woc.szPname);
			}
		}
		return sDevices;
	}

	void SetUserFunction(double(*func)(double)) {
		dUserFunction = func;
	}

	double clip(double dSample, double dMax) {
		if (dSample >= 0.0) {
			return fmin(dSample, dMax);
		}else{
			return fmax(dSample, -dMax);
		}
	}

private:
	double(*dUserFunction)(double);

	unsigned int iSampleRate; //rate of samples in hz
	unsigned int iChannels; //number of channels
	unsigned int iPackets; //count of packets, packets are multiple samples
	unsigned int iPacketSamples; // amount of samples per packet
	unsigned int iPacketCurrent; // current packet in use

	T* cPacketMemory; //pointer to packets position in memory
	WAVEHDR *whWaveHeaders; // info about wave packet
	HWAVEOUT hwDevice; //wave file output handler

	thread tThread;
	atomic<bool> abReady; 
	atomic<unsigned int> aiPacketFree;
	condition_variable cvPacketNotZero;
	mutex mutexPacketNotZero;

	atomic<double> adGlobalTime;

	//soundcard(DAC) data request handler
	void waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwParam1, DWORD dwParam2) {
		if (uMsg != WOM_DONE) return;

		aiPacketFree++;
		unique_lock<mutex> lm(mutexPacketNotZero);
		cvPacketNotZero.notify_one();
	}

	//wrapper for soundcard(DAC) handler
	static void CALLBACK waveOutProcWrap(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
		((audioOut*)dwInstance)->waveOutProc(hWaveOut, uMsg, dwParam1, dwParam2);
	}

	// handles responses to DAC requests to fill packets with audio data
	void MainThread() {
		adGlobalTime = 0.0;
		double dTimeStep = 1.0 / (double)iSampleRate;

		//getting max int for a template at run-time
		T cMaxSample = (T)pow(2, (sizeof(T) * 8) - 1) - 1;
		double dMaxSample = (double)cMaxSample;
		T cPreviousSample = 0;

		while (abReady) {
			//wait for packet
			if (aiPacketFree == 0) {
				unique_lock<mutex> lm(mutexPacketNotZero);
				cvPacketNotZero.wait(lm);
			}
			//packet is usable
			aiPacketFree--;

			//prepare packet
			if (whWaveHeaders[iPacketCurrent].dwFlags & WHDR_PREPARED)
				waveOutUnprepareHeader(hwDevice, &whWaveHeaders[iPacketCurrent], sizeof(WAVEHDR));

			T cNewSample = 0;
			int iCurrentPacket = iPacketCurrent * iPacketSamples;

			for(unsigned int n = 0; n < iPacketSamples; n++){
				//user process
				if (dUserFunction == nullptr) {
					cNewSample = (T)(clip(UserProcess(adGlobalTime), 1.0) * dMaxSample);
				}else {
					cNewSample = (T)(clip(dUserFunction(adGlobalTime), 1.0) * dMaxSample);
				}

				cPacketMemory[iCurrentPacket + n] = cNewSample;
				cPreviousSample = cNewSample;
				adGlobalTime = adGlobalTime + dTimeStep;
			}

			//send packet to DAC
			waveOutPrepareHeader(hwDevice, &whWaveHeaders[iPacketCurrent], sizeof(WAVEHDR));
			waveOutWrite(hwDevice, &whWaveHeaders[iPacketCurrent], sizeof(WAVEHDR));
			iPacketCurrent++;
			iPacketCurrent %= iPackets;
		}


	}
};