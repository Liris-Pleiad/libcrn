/* Copyright 2009-2014 INSA Lyon, CoReNum
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
 * file: CRNTimer.cpp
 * \author Yann LEYDIER
 */

#include <CRNUtils/CRNTimer.h>
#include <CRNi18n.h>
#ifdef CRN_PF_WIN32
#	include <windows.h>
#else
#	include <sys/time.h>
#endif
using namespace crn;

std::map<String, Timer::StopWatch> Timer::stopwatches;

/*! 
 * Platform dependant computation of the current time.
 *
 * \return	the current time en seconds
 */
double Timer::getTime()
{
#ifdef CRN_PF_WIN32
	_SYSTEMTIME tim;
	GetSystemTime(&tim);
	return ((tim.wDay * 24.0 + tim.wHour) * 60.0 + tim.wMinute) * 60.0 + 
		tim.wSecond + (tim.wMilliseconds / 1000.0); // XXX does not work if the month changes!
#else
	timeval tim;
	gettimeofday(&tim, nullptr);
	return double(tim.tv_sec) + (double(tim.tv_usec) / 1000000.0);
#endif
}

/*! 
 * Starts a stopwatch. Stores the beginning time.
 *
 * \param[in]	timername	the name of the stopwatch
 */
void Timer::Start(const String &timername)
{
	StopWatch &sw = stopwatches[timername];
	sw.stops.clear();
	sw.t0 = getTime(); // store time as late as possible
}

/*! 
 * Records time in a stopwatch. Stores a time with a name.
 *
 * \param[in]	timername	the name of the stopwatch
 * \param[in]	splitname	the name of the split
 * \return	the time elapsed since the last split (or the start if this is the first split)
 */
double Timer::Split(const String &timername, const String &splitname)
{
	double t = getTime(); // store time as soon as possible
	double pt;
	StopWatch &sw = stopwatches[timername];
	if (sw.stops.size() > 0)
		pt = sw.stops.back().second;
	else
		pt = sw.t0;
	sw.stops.push_back(std::make_pair(splitname, t));
	return t - pt;
}

/*! 
 * Dumps statistics to a string. Can be called at any moment in the process.
 *
 * \param[in]	timername	the name of the stopwatch
 * \return	a string containing statistics
 */
String Timer::Stats(const String &timername)
{
	StopWatch &sw = stopwatches[timername];
	if (sw.stops.size() == 0)
		return _("Unused stopwatch.");
	int prec = String::Precision();
	String::Precision() = 6;
	String s(_("Stopwatch: "));
	s += timername + U"\n";
	s += _("Total time: ");
	double pt = sw.t0;
	double tot = sw.stops.back().second - pt;
	s += tot;
	s += U" s";
	for (StopTime &st : sw.stops)
	{
		s += U"\n" + st.first + U": ";
		double thistime = st.second - pt;
		String::Precision() = 6;
		s += thistime;
		s += U" s\t(";
		String::Precision() = 4;
		s += (thistime * 100.0) / tot;
		s += U"%)";
		pt = st.second;
	}
	String::Precision() = prec;
	return s;
}

/*! 
 * Frees a stopwatch
 *
 * \param[in]	timername	the name of the stopwatch
 */
void Timer::Destroy(const String &timername)
{
	stopwatches.erase(timername);
}

static const String CRN_TIMER_UNIQUE_NAME(U"ceci n'est pas un nom"); /*!< Internal */
/*! 
 * Starts the quick stopwatch. 
 *
 * \warning Calling Start() twice without calling Stop will discard the effects of the first Start().
 */
void Timer::Start()
{
	Start(CRN_TIMER_UNIQUE_NAME);
}

/*! 
 * Stops the quick stopwatch. Do not call without a Start() before!
 *
 * \return	the elapsed time
 */
double Timer::Stop()
{
	double t = Split(CRN_TIMER_UNIQUE_NAME, CRN_TIMER_UNIQUE_NAME);
	Destroy(CRN_TIMER_UNIQUE_NAME);
	return t;
}

