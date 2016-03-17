/* Copyright 2010-2015 CoReNum
 * 
 * This file is part of libgtkcrnmm.
 * 
 * libgtkcrnmm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libgtkcrnmm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with libgtkcrnmm.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * file: GtkCRNProgress.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNProgress.h>

using namespace GtkCRN;

/*! Constructor
 * \param[in]	name	the label displayed on the progress bar
 * \param[in]	maxcount	the number of calls to Advance() to reach 100%
 */
GtkCRN::Progress::Progress(const crn::String &name, size_t maxcount):
	crn::Progress(name, maxcount),
	has_ended(false)
{
	pb.show();
	display(name);
}

/*! Constructor
 * \param[in]	name	the label displayed on the progress bar
 * \param[in]	execute_at_end	the callback to call when the progress reaches 100%
 * \param[in]	maxcount	the number of calls to Advance() to reach 100%
 */
GtkCRN::Progress::Progress(const crn::String &name, sigc::slot<void> execute_at_end, size_t maxcount):
	crn::Progress(name, maxcount),
	has_ended(false)
{
	progress_end.connect(execute_at_end);
	pb.show();
	display(name);
}

/*! Displays current progress on Gtk progress bar (thread safe)
 * \param[in]	msg	the text to display
 */
void GtkCRN::Progress::display(const crn::String &msg)
{
	PM *data(new PM(this, msg));
	g_idle_add(GSourceFunc(Progress::ts_display), data);
}

gboolean Progress::ts_display(PM *data)
{
	data->prog->pb.set_text(data->msg.CStr());
	data->prog->pb.set_fraction(double(data->prog->current) / data->prog->end);
	if ((data->prog->current >= data->prog->end) && !data->prog->has_ended)
	{
		data->prog->has_ended = true;
		data->prog->progress_end.emit();
	}
	delete data;
	return FALSE;
}

