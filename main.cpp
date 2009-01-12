#include <QApplication>
#include <QTranslator>

#include "FinWindow.h"


int main(int ac, char *av[])
{
    QApplication app(ac, av);

    QTranslator translator;
    translator.load("financier_ru");
    app.installTranslator(&translator);

    FinWindow main;
    main.show();
    
    return app.exec();
}

