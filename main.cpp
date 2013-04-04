#include <QtGui>
#include "graphwidget.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    GraphWidget widget;
    widget.show();
	return a.exec();
}
