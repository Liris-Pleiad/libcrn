/* Copyright 2016 CoReNum
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
 * \file QtCRNImage.hxx
 * \brief image display
 * \author Yann Leydier
 * \date 2016
 */
#ifndef HEADER_QtCRNImage
#define HEADER_QtCRNImage

#include <QtGlobal> // important, for QT_VERSION

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPixmap>
#include <QImage>
#include <CRNImage/CRNImage.h>

namespace QtCRN
{
	QImage QImageFromCRN(const crn::ImageBase &img);
	crn::UImage CRNImageFromQt(const QImage &img);

	class Image: public QGraphicsView
	{
		Q_OBJECT

		public:
			Image(QWidget *parent = nullptr);
			void setImage(const QImage &img);
			void setImage(QImage &&img);

		protected:
			virtual void mouseReleaseEvent(QMouseEvent *event) override;
			virtual void wheelEvent(QWheelEvent *event) override;

		private:
			void redraw();

			QGraphicsScene scene;
			QPixmap pm;
			qreal zoom;
	};
}

#endif
