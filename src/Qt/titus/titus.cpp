/*!
 * \file main.cpp
 * \brief main file.
 * \author Martial TOLA, CNRS-Lyon, LIRIS UMR 5205
 * \date 2016
 */
#include <QApplication>

#include "mainwindow.hxx"

#include <QFile>
#include <QTextStream>

/*!
 * \fn void loadStyleSheet()
 * \brief Load and apply StyleSheet (not used).
 */
void loadStyleSheet()
{
    // Let's use QFile and point to a resource...
    QFile data("./titus.qss");
    QString style;

    // ...to open the file
    if (data.open(QFile::ReadOnly))
	{
        // QTextStream...
        QTextStream styleIn(&data);
        // ...read file to a string.
        style = styleIn.readAll();
        data.close();

        // We'll use qApp macro to get the QApplication pointer and set the style sheet application wide.
        qApp->setStyleSheet(style);
    }
}

/*!
 * \fn int main(int argc, char *argv[])
 * \brief main function.
 *
 * \param argc .
 * \param argv .
 * \return 0 if normal.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    mainwindow window;
	//loadStyleSheet();
	window.show();

    return app.exec();
}
