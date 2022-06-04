#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QLayout>
#include <QTimer>
#include <QSlider>
#include <QLabel>
#include <QKeyEvent>
#include "glwidget.h"
#include "world.h"
#include "rainbow.h"

class window : public QWidget
{
    Q_OBJECT

    glWidget *GLWidget;
    QHBoxLayout *MainLayout;
    QSlider *PotentialDifference;
    QLabel *PotentialDifferenceLabel;
    QSlider *Temperature;
    QLabel *TemperatureLabel;
    QLabel *CurrentTemperatureLabel;
    QLabel *IntensityLabel;
    QSlider *Intensity;
    QLabel *AccuracyLabel;
    QSlider *Accuracy;
    QLabel *FrequencyLabel;
    QSlider *Frequency;
    rainbow *FrequencyRainbow;
    QTimer *TimerReplot;
    world *World;
    QTimer *FPScountTimer;    
    float Tfilter;

    void initControls();
    void initFPS();

signals:


public:
    window(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *event);
    ~window();

public slots:
    void updatePlots();
    void FPShandle();
    void updateControls();
    void setAccuracy(int value);
    void updateCurrentTemperature(float value);
};

#endif // WINDOW_H
