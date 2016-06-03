/* Copyright 2015 CNRS
 * 
 * This file is part of libcrn.
 * 
 * libcrn is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libcrn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with libcrn.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * \file QtCRNMain.hxx
 * \brief mainwindow file.
 * \author Martial TOLA, CNRS-Lyon, LIRIS UMR 5205
 * \date 2016
 */
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
