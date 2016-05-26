///////////////////////////////////////////////////////////////////////////
// Author: Martial TOLA
// Year: 2016
// CNRS-Lyon, LIRIS UMR 5205
///////////////////////////////////////////////////////////////////////////
#ifndef HEADER_QtCRNMain
#define HEADER_QtCRNMain

#include <QtGlobal> // important, for QT_VERSION

#include <QObject>
#include <QAction>

class QtCRNMain : public QObject
{
	Q_OBJECT

	public:
		QtCRNMain()
		{
			actionExample = new QAction(tr("Action example"), this);
			if (actionExample)
				connect(actionExample, SIGNAL(triggered()), this, SLOT(example()));
		}

		~QtCRNMain()
		{
			delete actionExample;
		}

	private:

	public slots:
		void example();

	private:
		QAction *actionExample;
};

#endif
