/* Copyright 2016 CNRS
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
 * \file mainwindow.hxx
 * \brief mainwindow file.
 * \author Martial TOLA, CNRS-Lyon, LIRIS UMR 5205
 * \date 2016
 */
#ifndef HEADER_mainwindow
#define HEADER_mainwindow

#include <QMainWindow>

/*!
 * \class mainwindow
 * \brief mainwindow class.
 */
class mainwindow : public QMainWindow
{
	Q_OBJECT

	public:
		/*!
		 * \brief Constructor.
		 *
		 * \param parent : parent window.
		 */
		mainwindow(QMainWindow *parent = 0);
		/*!
		 * \brief Destructor.
		 */
		~mainwindow();

	protected:

		private slots:

	private:
};

#endif
