#include "window.h"

#include <string.h>
#include <math.h>
//#include <QDebug>

window::window(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowTitle("Моделирование вакуумного фотодиода");
    Tfilter = 0.0;

    //Graphics
    GLWidget = new glWidget;
    GLWidget->setMinimumSize(500,500);
    GLWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    initControls();
    initPlotWidget();

    MainLayout = new QHBoxLayout;
    MainLayout->addWidget(GLWidget);

    QLayout *controlsLayout = new QVBoxLayout;
    controlsLayout->addWidget(PlotWidget);
    controlsLayout->addWidget(PotentialDifferenceLabel);
    controlsLayout->addWidget(PotentialDifference);
    //controlsLayout->addWidget(TemperatureLabel);
    //controlsLayout->addWidget(Temperature);
    //controlsLayout->addWidget(CurrentTemperatureLabel);
    controlsLayout->addWidget(IntensityLabel);
    controlsLayout->addWidget(Intensity);
    controlsLayout->addWidget(FrequencyLabel);
    controlsLayout->addWidget(Frequency);
    controlsLayout->addWidget(FrequencyRainbow);
    controlsLayout->addWidget(AccuracyLabel);
    controlsLayout->addWidget(Accuracy);
    MainLayout->addLayout(controlsLayout);

    updateControls();

    //Applying Layout
    this->setLayout(MainLayout);

    World = new world(96, 300, 5.0, 0.9);
    GLWidget->setWorld(World);

    connect(World, SIGNAL(newTemperature(float)), this, SLOT(updateCurrentTemperature(float)));
    connect(Temperature, SIGNAL(valueChanged(int)), World, SLOT(setT(int)));
    connect(PotentialDifference, SIGNAL(valueChanged(int)), World, SLOT(setU(int)));
    connect(Temperature, SIGNAL(valueChanged(int)), this, SLOT(updateControls()));
    connect(PotentialDifference, SIGNAL(valueChanged(int)), this, SLOT(updateControls()));
    connect(Temperature, SIGNAL(valueChanged(int)), this, SLOT(refreshPlot()));
    connect(Temperature, SIGNAL(valueChanged(int)), this, SLOT(temperatureChangeHandle()));
    connect(PotentialDifference, SIGNAL(valueChanged(int)), this, SLOT(refreshPlot()));
    connect(Intensity, SIGNAL(valueChanged(int)), World, SLOT(setIntensity(int)));
    connect(Intensity, SIGNAL(valueChanged(int)), this, SLOT(updateControls()));
    connect(Frequency, SIGNAL(valueChanged(int)), World, SLOT(setFrequency(int)));
    connect(Frequency, SIGNAL(valueChanged(int)), this, SLOT(updateControls()));
    connect(Frequency, SIGNAL(valueChanged(int)), World, SLOT(setColor(int)));
    connect(Accuracy, SIGNAL(valueChanged(int)), this, SLOT(setAccuracy(int)));

    //Temperature->setValue(161);
    World->setIntensity(Intensity->value());
    World->setFrequency(Frequency->value());
    World->setColor(Frequency->value());
    World->setT(Temperature->value());
    World->setU(PotentialDifference->value());
    setAccuracy(Accuracy->value());

    initFPS();

    World->startCalc();

    TimerReplot = new QTimer();
    TimerReplot->setInterval(1000/60);
    connect(TimerReplot, SIGNAL(timeout()), this, SLOT(updatePlots()));
    TimerReplot->start();
}

window::~window()
{
}

void window::temperatureChangeHandle()
{
    PlotWidget->graph(currentGraph)->setPen(QPen(QColor(124, 124, 124), 2));
    currentGraph = 1 - currentGraph;
    PlotWidget->graph(currentGraph)->setPen(QPen(QColor(0, 0, 0), 2));
    PlotWidget->graph(currentGraph)->data()->clear();
}

void window::updateControls()
{
    char tmp[20];
    float val = PotentialDifference->value();
    sprintf(tmp, "%d", int(val/50.0));
    PotentialDifferenceLabel->setText("Напряжение (" + QString(tmp) + " В) : ");

    IntensityLabel->setText("Интенсивность излучения (" + QString::number(1+Intensity->value()) + " ед.) : ");

    FrequencyLabel->setText("Длина волны излучения (" + QString::number(int(6.0*140000.0/(1200.0+Frequency->value()))) + " нм) : ");
    //FrequencyRainbow->repaint();
}

void window::initPlotWidget()
{
    PlotWidget = new plotWidget;
    //PlotWidget->setMinimumHeight(200);
    //PlotWidget->setMinimumWidth(500);
    PlotWidget->xAxis->setLabel("Напряжение Катод-Анод U");
    PlotWidget->yAxis->setLabel("Сила тока I(U)");
    PlotWidget->xAxis->setRange(-0.5, 1.0);
    //PlotWidget->yAxis->setScaleType(QCPAxis::stLogarithmic);
    PlotWidget->yAxis->setRange(-0.1, 1.0);
    PlotWidget->addGraph(PlotWidget->xAxis, PlotWidget->yAxis);
    PlotWidget->addGraph(PlotWidget->xAxis, PlotWidget->yAxis);
    PlotWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    PlotWidget->graph(1)->setPen(QPen(QColor(255, 110, 40), 2));

    refreshPlot();
}

void window::initControls()
{
    currentGraph = 1;
    TemperatureLabel = new QLabel();
    Temperature = new QSlider(Qt::Horizontal);
    Temperature->setMinimum(0);
    Temperature->setMaximum(1500);
    Temperature->setValue(10);
    Temperature->setMaximumWidth(500);

    CurrentTemperatureLabel = new QLabel();

    PotentialDifferenceLabel = new QLabel();
    PotentialDifference = new QSlider(Qt::Horizontal);
    PotentialDifference->setMinimum(-500);
    PotentialDifference->setMaximum(750);
    PotentialDifference->setValue(150);
    PotentialDifference->setMaximumWidth(500);

    IntensityLabel = new QLabel();
    Intensity = new QSlider(Qt::Horizontal);
    Intensity->setMinimum(0);
    Intensity->setMaximum(10);
    Intensity->setValue(0);
    Intensity->setMaximumWidth(500);

    FrequencyLabel = new QLabel();
    Frequency = new QSlider(Qt::Horizontal);
    Frequency->setMinimum(0);
    Frequency->setMaximum(900);
    Frequency->setValue(500);
    Frequency->setMaximumWidth(500);
    FrequencyRainbow = new rainbow;
    //FrequencyRainbow->setMaximumWidth(500);
    FrequencyRainbow->setMinimumHeight(20);


    Accuracy = new QSlider(Qt::Horizontal);
    Accuracy->setMinimum(1);
    Accuracy->setMaximum(10);
    Accuracy->setValue(2);
    Accuracy->setMaximumWidth(500);
    AccuracyLabel = new QLabel("\nШаг интегрирования по времени:");
}

void window::setAccuracy(int value)
{
    float dt=0.20*1e-3;
    for (int i=0; i<value; ++i)
        dt /= 1.25;
    World->set_dt(dt);
}

void window::updateCurrentTemperature(float value)
{
    char tmp[20];
    float alpha = 0.05;
    Tfilter = (1.0-alpha)*Tfilter + alpha*value;
    GLWidget->Tpercent = Tfilter/float(63.0);
    //sprintf(tmp, "%5.0f", Tfilter*15.878);
    sprintf(tmp, "%4d", int(Tfilter*15.878));
    CurrentTemperatureLabel->setText("Текущая температура подогрева катода = " + QString(tmp));
}

void window::initFPS()
{    
    FPScountTimer = new QTimer();
    FPScountTimer->setInterval(250);
    connect(FPScountTimer, SIGNAL(timeout()), this, SLOT(FPShandle()));
    FPScountTimer->start();
}

void window::updatePlots()
{        
    GLWidget->update();
    //PlotWidget->update();

    this->update();
}

void window::refreshPlot()
{
    //QPlot
    // Particle Density
    /*
    PlotWidget->graph(0)->data()->clear();
    int density[24];
    for (int i=0; i<24; ++i)
        density[i]=0;
    for (int i=0; i<World->Nparticles; ++i)
    {
        int ind = 4+int(World->aDraw[i].R.y*2.0);
        if (ind < 0)
            ind = 0;
        if (ind > 23)
            ind = 23;
        density[ind]++;
    }
    for (int i=0; i<24; ++i)
        PlotWidget->graph(0)->addData(i/2.0 - 2.0, density[i]);
    */

    // DATA line
    //int Npoints=300;
    float T = float(this->Temperature->value())/float(this->Temperature->maximum());    
    T *= T; //square T, coefficient Deshmanna-Richardson'a

    /*for (int i=0; i<Npoints; ++i)
        PlotWidget->graph(0)->addData(-1.0+float(i)/float(Npoints), 0.0);

    for (int i=0; i<Npoints; ++i)
    {
        float ii = float(i)/float(Npoints);
        if (i < Npoints/2)
            PlotWidget->graph(0)->addData(ii, T*ii*sqrt(ii));
        else
            PlotWidget->graph(0)->addData(ii, T*ii*sqrt(ii));
    }
    */


    // cross
    float x = float(this->PotentialDifference->value())/float(this->PotentialDifference->maximum());
    PlotWidget->xCross = x;
    float y=0;
    if (x>0)
        y = sqrt(x+0.1)-0.315;

    PlotWidget->yCross = y;

    //DATA line islandic
    PlotWidget->graph(currentGraph)->addData(x, y);

    PlotWidget->replot();
}

void window::keyPressEvent(QKeyEvent *event)
{    
    if (Qt::Key_Escape == event->key())
        this->close();
}

void window::FPShandle()
{    
    setWindowTitle("Научные Развлечения - вакуумный фотодиод (" + QString::number(World->getFrames()*4) + " итераций/сек)");
}
