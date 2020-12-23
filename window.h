#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QLayout>
#include <QTimer>
#include <QSlider>
#include <QLabel>
#include "glwidget.h"
#include "plotwidget.h"
#include "world.h"
#include "rainbow.h"

class window : public QWidget
{
    Q_OBJECT

    glWidget *GLWidget;
    plotWidget *PlotWidget;
    QHBoxLayout *MainLayout;
    QSlider *PotentialDifference;
    QLabel *PotentialDifferenceLabel;
    QSlider *Temperature;
    int currentGraph;
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

    void initPlotWidget();
    void initControls();
    void initFPS();

signals:


public:
    window(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *event);
    ~window();

public slots:
    void temperatureChangeHandle();
    void updatePlots();
    void FPShandle();
    void refreshPlot();
    void updateControls();
    void setAccuracy(int value);
    void updateCurrentTemperature(float value);
};

#endif // WINDOW_H
