#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QtGui/QGraphicsView>

class Node;

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GraphWidget();

    void itemMoved();

protected:
    void timerEvent(QTimerEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect); //Отрисовка заднего плана

private:
    int timerId;
    Node *centerNode;
};

#endif
