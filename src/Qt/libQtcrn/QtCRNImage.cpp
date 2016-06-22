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
 * \file QtCRNImage.cpp
 * \brief image display
 * \author Yann LEYDIER
 * \date 2016
 */

#include <QtCRNImage.hxx>
#include <QPainter>
#include <QMouseEvent>

#include <iostream>

using namespace QtCRN;

QImage QtCRN::QImageFromCRN(const crn::ImageBase &img)
{
	// TODO
	throw 1;
}

crn::UImage QtCRN::CRNImageFromQt(const QImage &img)
{
	// TODO
	throw 1;
}

Image::Image(QWidget *parent):
	QGraphicsView(parent),
	zoom(1.0)
{
	setScene(&scene);

	// TMP
	setImage(QImage{"/data/images/The_Evil_Snowman.jpg"});

	redraw();
}

void Image::setImage(const QImage &img)
{
	pm = QPixmap::fromImage(img);
}

void Image::setImage(QImage &&img)
{
	pm = QPixmap::fromImage(std::move(img));
}

void Image::setZoom(qreal z)
{
	zoom = z;
	if (zoom > 10) zoom = 10;
	if (zoom < 0.1) zoom = 0.1;
	resetTransform();
	scale(zoom, zoom);
}

void Image::zoomIn()
{
	zoom += 0.1;
	if (zoom > 10) zoom = 10;
	resetTransform();
	scale(zoom, zoom);
}

void Image::zoomOut()
{
	zoom -= 0.1;
	if (zoom < 0.1) zoom = 0.1;
	resetTransform();
	scale(zoom, zoom);
}

void Image::zoom100()
{
	zoom = 1;
	resetTransform();
	scale(zoom, zoom);
}

void Image::zoomFit()
{
	fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
	zoom = transform().m11();
}

void Image::mouseReleaseEvent(QMouseEvent *event)
{
	std::cout << "click @ " << event->x() << "x" << event->y() << '\n';
	auto p = mapToScene(event->x(), event->y());
	std::cout << "click @ " << p.x() << "x" << p.y() << '\n';
	
	QGraphicsView::mouseReleaseEvent(event);
}

void Image::wheelEvent(QWheelEvent *event)
{
	if (event->modifiers() & Qt::ControlModifier)
	{ // zoom
#if QT_VERSION >= 0x050000
		if (event->angleDelta().y() > 0)
#else
		if (event->delta() > 0)
#endif
			zoom += 0.1;
		else
			zoom -= 0.1;
		if (zoom > 10) zoom = 10;
		if (zoom < 0.1) zoom = 0.1;
		resetTransform();
		scale(zoom, zoom);
	}
	else
		QGraphicsView::wheelEvent(event);
}

void Image::redraw()
{
	scene.clear();
	scene.addPixmap(pm);
}

