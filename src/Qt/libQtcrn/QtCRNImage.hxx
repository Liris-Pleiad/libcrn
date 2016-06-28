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

			qreal getZoom() const noexcept { return zoom; }
			void setZoom(qreal z);
			void zoomIn();
			void zoomOut();
			void zoom100();
			void zoomFit();

			// focus_on() --> centerOn()

			// void 	set_user_cursor (const Gdk::Cursor &cur)

			// Glib::RefPtr< Gtk::ActionGroup > 	get_actions ()

			// sigc::signal< void, crn::String, crn::String, MouseMode > 	signal_overlay_changed () --> virtual QVariant QGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
			// sigc::signal< void, guint, guint32, std::vector < std::pair< crn::String, crn::String > >, int, int > 	signal_rmb_clicked ()
			// sigc::signal< void, int, int > 	signal_user_mouse ()
			// sigc::signal< void, Glib::RefPtr< Gdk::Pixbuf > > 	signal_drawing ()
			// sigc::signal< void > 	signal_zoom_changed ()
			// sigc::signal< void, int, int > 	signal_scrolled_event ()

			// Gtk::Adjustment& 	get_hadjustment() --> horizontalScrollBar() or transform().m31() = dx
			// Gtk::Adjustment& 	get_vadjustment() --> verticalScrollBar()  or transform().m32() = dy

			// OverlayConfig& 	get_overlay_config (const crn::String &id)
			// void 	show_overlay (const crn::String &id)
			// void 	hide_overlay (const crn::String &id)
			// void 	set_overlay_visible (const crn::String &id, bool visible)
			// void 	clear_overlays ()
			// void 	clear_overlay (const crn::String &id)
			// void 	add_overlay_item (const crn::String &overlay_id, const crn::String &item_id, const crn::Rect &r, const crn::StringUTF8 &label="")
			// void 	add_overlay_item (const crn::String &overlay_id, const crn::String &item_id, const crn::Point2DInt &point, const crn::StringUTF8&label="")
			// void 	add_overlay_item (const crn::String &overlay_id, const crn::String &item_id, const crn::Point2DInt &p1, const crn::Point2DInt &p2, const crn::StringUTF8 &label="")
			// void 	add_overlay_item (const crn::String &overlay_id, const crn::String &item_id, const std::vector< crn::Point2DInt > &p, const crn::StringUTF8 &label="")
			// void 	add_overlay_item (const crn::String &overlay_id, const crn::String &item_id, std::vector< crn::Point2DInt > &&p, const crn::StringUTF8 &label="")
			// void 	add_overlay_item (const crn::String &overlay_id, const crn::String &item_id, const crn::StringUTF8 &label, const crn::Point2DInt &position)
			// OverlayItem& 	get_overlay_item (const crn::String &overlay_id, const crn::String &item_id)
			// const OverlayItem& 	get_overlay_item (const crn::String &overlay_id, const crn::String &item_id) const
			// void 	remove_overlay_item (const crn::String &overlay_id, const crn::String &item_id)

			// void 	set_selection_type (Overlay typ)
			// Overlay 	get_selection_type () const
			// OverlayConfig& 	get_selection_config ()
			// bool 	has_selection () const
			// void 	clear_selection ()
			// const crn::Rect& 	get_selection_as_rect () const
			// const crn::Point2DInt& 	get_selection_as_point () const
			// std::pair< crn::Point2DInt, crn::Point2DInt > 	get_selection_as_line () const
			// void 	set_selection (const crn::Rect &r)
			// void 	set_selection (const crn::Point2DInt &p)
			// void 	set_selection (const crn::Point2DInt &p1, const crn::Point2DInt &p2)

			// static const crn::String& 	selection_overlay ()

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
