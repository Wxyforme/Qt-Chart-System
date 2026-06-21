#ifndef LINECHART_H
#define LINECHART_H

#include "chart.h"

//折线图
class LineChart : public Chart
{
    Q_OBJECT
public:
    explicit LineChart(QWidget *parent = 0);
    LineChart(QString t, QColor c, QWidget *parent = 0);
    virtual void draw(QPainter &painter);
};

#endif