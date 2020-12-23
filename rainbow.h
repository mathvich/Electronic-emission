#ifndef RAINBOW_H
#define RAINBOW_H

#include <QWidget>
#include <QPainter>

class rainbow : public QWidget
{
    Q_OBJECT
public:
    explicit rainbow(QWidget *parent = 0);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent * event);
};

#endif // RAINBOW_H
