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
	//������� ����� � ���������� � ���������
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-400, -400, 800, 800); //���������� ��������� ���� � ������ �����
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);

	//������ ��������� ���������
	const int numberOfHex = 7; //���������� ����� �� �����������
	const int numberOfStr = 7; //���������� ����� �� ��������� (��������)

	const int sizeLength = 2 * numberOfHex + 1; //�����������
	const int sizeWight = numberOfStr + 1; //���������
	Node *arrayNode[sizeWight][sizeLength];
	for(int i = 0; i < sizeWight; i++)
	{
		for(int j = 0; j < sizeLength; j++) //������� ����
		{
			arrayNode[i][j] = new Node(this); //������� ��� � �������� ��� ��������� � ������
			scene->addItem( arrayNode[i][j] ); //�������� �� ����� ���
			arrayNode[i][j]->setPos(0,0); //���������� ������� ������
		}
		for(int j = 0; j < sizeLength - 1; j++) //��������� ���� � �����
		{
			scene->addItem(new Edge(arrayNode[i][j], arrayNode[i][j+1]));
		}
	}

	for(int i = 0; i < sizeWight - 1; i++)
	{
		if(0 == i%2) //���� ������� ������
		{
			for(int j = 0; j < sizeLength; j++) 
				if(0 == j%2) //��������� ��� ������ ���� ����� �����
					scene->addItem(new Edge(arrayNode[i][j], arrayNode[i+1][j]));
		}
		if(1 == i%2) //���� ������� ��������
		{
			for(int j = 0; j < sizeLength; j++) 
				if(1 == j%2) //��������� ��� �������� ���� ����� �����
					scene->addItem(new Edge(arrayNode[i][j], arrayNode[i+1][j]));
		}
	}

	//���������� ��������� ����
    scale(qreal(0.8), qreal(0.8)); //�������
    setMinimumSize(800, 800); //����������� ������ ����
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

    // ����
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
	painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
	painter->fillRect(bottomShadow, Qt::darkGray);

    // ������� �������� ������������
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::lightGray);
    gradient.setColorAt(1, Qt::black);
    painter->fillRect(rect.intersect(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);
}
