#include "graphwidget.h"
#include "edge.h"
#include "node.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QWheelEvent>

#include <math.h>

GraphWidget::GraphWidget()
    : timerId(0)
{
	//Создали сцену и установили её параметры
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-400, -400, 800, 800); //Установили положение нуля и размер сцены
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);

	//Вводим атомарную структуру
	const int numberOfHex = 7; //Количество ячеек по горизонтали
	const int numberOfStr = 7; //Количество ячеек по вертикали (нечетное)

	const int sizeLength = 2 * numberOfHex + 1; //Горизонталь
	const int sizeWight = numberOfStr + 1; //Вертикаль
	Node *arrayNode[sizeWight][sizeLength];
	for(int i = 0; i < sizeWight; i++)
	{
		for(int j = 0; j < sizeLength; j++) //Создаем шары
		{
			arrayNode[i][j] = new Node(this); //Создали шар и добавили его указатель в массив
			scene->addItem( arrayNode[i][j] ); //Добавили на сцену шар
			arrayNode[i][j]->setPos(0,0); //Установили позиция шарика
		}
		for(int j = 0; j < sizeLength - 1; j++) //Связываем шары в линию
		{
			scene->addItem(new Edge(arrayNode[i][j], arrayNode[i][j+1]));
		}
	}

	for(int i = 0; i < sizeWight - 1; i++)
	{
		if(0 == i%2) //Если полоска четная
		{
			for(int j = 0; j < sizeLength; j++) 
				if(0 == j%2) //Соединяем все четные шары между собой
					scene->addItem(new Edge(arrayNode[i][j], arrayNode[i+1][j]));
		}
		if(1 == i%2) //Если полоска нечетная
		{
			for(int j = 0; j < sizeLength; j++) 
				if(1 == j%2) //Соединяем все нечетные шары между собой
					scene->addItem(new Edge(arrayNode[i][j], arrayNode[i+1][j]));
		}
	}

	//Установили параметры окна
    scale(qreal(0.8), qreal(0.8)); //Масштаб
    setMinimumSize(800, 800); //Минимальный размер окна
    setWindowTitle(tr("Graphene Structure"));
}

void GraphWidget::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}

void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) 
	{
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    foreach (Node *node, nodes)
        node->calculateForces();

    bool itemsMoved = false;
    foreach (Node *node, nodes) 
	{
        if (node->advance())
            itemsMoved = true;
    }

    if (!itemsMoved) 
	{
        killTimer(timerId);
        timerId = 0;
    }
}

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Тень
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
	painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
	painter->fillRect(bottomShadow, Qt::darkGray);

    // Заливка рабочего пространства
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::lightGray);
    gradient.setColorAt(1, Qt::black);
    painter->fillRect(rect.intersect(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);
}
