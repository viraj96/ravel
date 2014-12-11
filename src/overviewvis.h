#ifndef OVERVIEWVIS_H
#define OVERVIEWVIS_H

#include "viswidget.h"
#include <QVector>

class VisOptions;
class QResizeEvent;
class QMouseEvent;
class QPainter;
class Trace;

// Full timeline shown by metric
class OverviewVis : public VisWidget
{
    Q_OBJECT
public:
    OverviewVis(QWidget *parent = 0, VisOptions *_options = new VisOptions());
    void setTrace(Trace * t);
    void processVis();
    void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

public slots:
    void setSteps(float start, float stop, bool jump = false);

protected:
    void qtPaint(QPainter *painter);

private:
    int roundeven(float step);

    QString cacheMetric; // So we can tell if metric changes

    // We can do this by time or steps, currently steps
    unsigned long long minTime;
    unsigned long long maxTime;
    int maxStep;
    int startCursor;
    int stopCursor;
    unsigned long long startTime;
    unsigned long long stopTime;
    int startStep;
    int stopStep;
    float stepWidth;
    int height;
    bool mousePressed;
    QVector<float> heights; // bar heights

    // Map between cursor position and steps
    QVector<std::pair<int, int> > stepPositions;
};

#endif // OVERVIEWVIS_H