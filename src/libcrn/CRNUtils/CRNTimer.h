/* Copyright 2009-2015 INSA Lyon, CoReNum, Université Paris Descartes
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
 * file: CRNTimer.h
 * \author Yann LEYDIER
 */

#ifndef CRNTimer_HEADER
#define CRNTimer_HEADER

#include <CRNString.h>
#include <map>

namespace crn
{
	/*! \brief A class to measure time.
	 *
	 * This class holds stopwatches that can be used in parallel
	 *
	 * \author Yann LEYDIER
	 * \date	August 2009
	 * \version 0.1
	 * \ingroup utils
	 */
	class Timer
	{
		public:
			/***********************************************************************/
			/* Simple unique stopwatch *********************************************/
			/***********************************************************************/
			/*! \brief Starts the quick stopwatch */
			static void Start();
			/*! \brief Stops the quick stopwatch */
			static double Stop();

			/***********************************************************************/
			/* Full stopwatch that can be used in parallel *************************/
			/***********************************************************************/
			/*! \brief Starts a stopwatch */
			static void Start(const String &timername);
			/*! \brief Records time in a stopwatch */
			static double Split(const String &timername, const String &splitname);
			/*! \brief Dumps statistics to a string */
			static String Stats(const String &timername);
			/*! \brief Frees a stopwatch */
			static void Destroy(const String &timername);

		private:
			/*! \brief Private constructor: this class may not be instantiated */
			Timer() {}
			/*! \brief Platform dependant computation of the current time */
			static double getTime();

			/*! \cond */
			/*! \brief Internal */
			using StopTime = std::pair<String, double>;
			/*! \brief Internal */
			struct StopWatch
			{
				std::vector<StopTime> stops;
				double t0;
			};
			/*! \endcond */
			static std::map<String, StopWatch> stopwatches; /*!< the stopwatches */
	};
}

#endif


