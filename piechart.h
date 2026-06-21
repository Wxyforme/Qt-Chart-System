#ifndef PIECHART_H
#define PIECHART_H

#include "chart.h"

//饼图
class PieChart : public Chart
{
    Q_OBJECT
public:
    explicit PieChart(QWidget *parent = 0);
    PieChart(QString t, QColor c, QWidget *parent = 0);
    virtual void draw(QPainter &painter);
};

#endif
