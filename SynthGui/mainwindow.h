#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setupAudio();
    int getOsc1();
    int getOsc2();
    int getOct();
    double getAtt();
    double getDec();
    double getSus();
    double getRel();
    double getLFOHertz();
    double getLFOAmp();
    double getBias();
    double getVol();


private slots:


    void on_OSC1Select_valueChanged(int value);

    void on_Osc2Select_valueChanged(int value);

    void on_OscBias_valueChanged(int value);

    void on_LFOAmplitude_valueChanged(int value);

    void on_LFOHertz_valueChanged(int value);

    void on_OctaveKnob_valueChanged(int value);

    void on_Attack_valueChanged(int value);

    void on_Decay_valueChanged(int value);

    void on_Sustain_valueChanged(int value);

    void on_Release_valueChanged(int value);

    void on_Volume_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    int osc1 = 2;
    int osc2 = 0;
    int octave = 0;
    double att = 0.1;
    double decayVal = 0.1;
    double sus = 0.8;
    double rel = 0.8;
    double lfoHertz = 5.0;
    double lfoAmp = 0.5;
    double bias = 1.0;
    double vol = 0.4;
};
#endif // MAINWINDOW_H
