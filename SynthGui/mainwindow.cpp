#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::getOsc1(){
    return osc1;
}

int MainWindow::getOsc2(){
    return osc2;
}

int MainWindow::getOct(){
    return octave;
}

double MainWindow::getAtt(){
    return att;
}

double MainWindow::getDec(){
    return decayVal;
}

double MainWindow::getSus(){
    return sus;
}

double MainWindow::getRel(){
    return rel;
}

double MainWindow::getLFOAmp(){
    return lfoAmp;
}

double MainWindow::getLFOHertz(){
    return lfoHertz;
}

double MainWindow::getBias(){
    return bias;
}

double MainWindow::getVol(){
    return vol;
}

void MainWindow::on_Osc2Select_valueChanged(int value)
{
    osc2 = value;
}

void MainWindow::on_OscBias_valueChanged(int value)
{
    bias = value/50;
}

void MainWindow::on_LFOAmplitude_valueChanged(int value)
{
    lfoAmp = value/10;
}

void MainWindow::on_LFOHertz_valueChanged(int value)
{
    lfoHertz = value;
}

void MainWindow::on_OctaveKnob_valueChanged(int value)
{
    octave = value;
}

void MainWindow::on_OSC1Select_valueChanged(int value)
{
    osc1 = value;
}


void MainWindow::on_Attack_valueChanged(int value)
{
    att = value/10;
}

void MainWindow::on_Decay_valueChanged(int value)
{
    decayVal = value/100;
}


void MainWindow::on_Sustain_valueChanged(int value)
{
    sus = value/10;
}

void MainWindow::on_Release_valueChanged(int value)
{
    rel = value/10;
}

void MainWindow::on_Volume_valueChanged(int value)
{
    vol = value/100;
}
