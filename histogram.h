#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "chart.h"

//柱状图
class Histogram : public Chart
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = 0);
    Histogram(QString t, QColor c, QWidget *parent = 0);
    virtual void draw(QPainter &painter);
};

#endif
