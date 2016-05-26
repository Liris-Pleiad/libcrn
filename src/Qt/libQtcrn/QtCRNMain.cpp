///////////////////////////////////////////////////////////////////////////
// Author: Martial TOLA
// Year: 2016
// CNRS-Lyon, LIRIS UMR 5205
/////////////////////////////////////////////////////////////////////////// 
#include "QtCRNMain.hxx"

#include <QApplication>

void QtCRNMain::example()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	QApplication::restoreOverrideCursor();
}
