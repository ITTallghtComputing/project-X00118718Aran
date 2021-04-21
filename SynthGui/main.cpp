#include <QApplication>
#include <iostream>
#include "audioOut.h"
#include "ADSR.h"
#include "Oscillators.h"
#include "mainwindow.h"
using namespace std;

atomic<double> frequencyOutput{0.0};
double octaveBaseFrequency = 440.0;
const double twelfthRootOfTwo = 1.05946309436;
int osc1 = 2;
int osc2 = 0;
double att = 0.1;
double decayVal = 0.1;
double sus = 0.8;
double rel = 0.8;
ADSREnvelope env(att, decayVal, sus, rel);
double lfoHertz = 5.0;
double lfoAmp = 0.5;
double bias1 = 1.0;
double bias2 = 1.0;
double volume = 0.4;

double MakeNoise(double dTime)
{
    // Mixed sine and square waves
    double dOutput = env.GetAmplitude(dTime) *
        (+bias1 * osc(frequencyOutput * 0.5, dTime, osc1, lfoHertz, lfoAmp) + bias2 * osc(frequencyOutput, dTime, osc2, lfoHertz, lfoAmp));
    return dOutput * volume; // Master Volume
}


int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    vector<wstring> devices = audioOut<short>::Enumerate(); //find output device

    for (auto d : devices) wcout << "found output device: " << d << endl;

    audioOut<short> sound(devices[0], 44100, 1, 8, 512); //creates an object of the AudioOut Class

    cout << "_____________________________________________________________\n"
            "|    | |    |    | |   | |    |    | |   | |   | |    |     |\n"
            "|    | |    |    | |   | |    |    | |   | |   | |    |     |\n"
            "|    |S|    |    |F|   |G|    |    |J|   |K|   |L|    |     |\n"
            "|     -     |     -     -     |     -     -     -     |     |\n"
            "|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |\n"
            "-------------------------------------------------------------";



    int currentKey = -1;
    bool keyPressed = false;
    while(w.isVisible())
    {
        a.processEvents();
        if(w.getOct() == -2){
            octaveBaseFrequency = 110;
        }else if (w.getOct() == -1){
            octaveBaseFrequency = 220;
        }else if(w.getOct() == 0){
            octaveBaseFrequency = 440;
        }else if(w.getOct() == 1){
            octaveBaseFrequency = 880;
        }else{
            octaveBaseFrequency = 1760;
        }
        bias1 = w.getBias();
        bias2 = 2.0 - w.getBias();
        lfoHertz = w.getLFOHertz();
        lfoAmp = w.getLFOAmp();
        osc1 = w.getOsc1();
        osc2 = w.getOsc2();
        sound.SetUserFunction(MakeNoise);

        keyPressed = false;
        for (int k = 0; k < 16; k++)
        {
            if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe\xbf"[k])) & 0x8000)
            {
                if (currentKey != k)
                {
                    frequencyOutput = octaveBaseFrequency * pow(twelfthRootOfTwo, k);
                    env.KeyPress(sound.GetTime());
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
                env.KeyDepress(sound.GetTime());
                currentKey = -1;
            }
        }
    }

    return 0;
}
