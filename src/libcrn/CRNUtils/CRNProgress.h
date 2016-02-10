/* Copyright 2007-2015 Yann LEYDIER, CoReNum, INSA-Lyon, Université Paris Descartes
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
 * file: CRNProgress.h
 * \author Yann LEYDIER
 */

#ifndef CRNPROGRESS_HEADER
#define CRNPROGRESS_HEADER

#include <CRNComplexObject.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief Base class for a progress display
	 *
	 * Progress display base class
	 *
	 * \author 	Yann LEYDIER
	 * \date		August 2009
	 * \version 0.1
	 * \ingroup utils
	 */
	class Progress: public ComplexObject
	{
		public:
			enum class Type { PERCENT = 0, ABSOLUTE = 1 };
			/*! \brief Constructor */
			Progress(const String &name, size_t maxcount = 100):ComplexObject(name),end(maxcount),current(0),type(Type::PERCENT) { }
			Progress(const Progress &) = delete;
			Progress(Progress&&) = default;
			Progress& operator=(const Progress &) = delete;
			Progress& operator=(Progress&&) = default;

			/*! \brief Destructor */
			virtual ~Progress() override;

			/*! \brief It is a ComplexObject */
			virtual Protocol GetClassProtocols() const noexcept override { return Protocol::ComplexObject; }

			/*! \brief Progresses of one step */
			void Advance();
			/*! \brief Sets the total number of steps */
			void SetMaxCount(size_t maxcount, bool reset = true) { end = maxcount; if (reset) Reset(); }
			/*! \brief Resets the progress to 0 */
			void Reset();

			/*! \brief Sets the type of progress bar */
			void SetType(Type typ) noexcept { type = typ; }

		protected:
			/*! \brief Displays the progress */
			virtual void display(const String &msg) = 0;

			size_t current; /*!< The current step */
			size_t end; /*!< The last step */
			String disp; /*!< The text displayed */
			Type type; /*!< The type of progress bar (Progress::PERCENT or Progress::ABSOLUTE */
	};

	CRN_ALIAS_SMART_PTR(Progress)

	/****************************************************************************/
	/*! \brief Progress display in console
	 *
	 * Progress display in console
	 *
	 * \author 	Yann LEYDIER
	 * \date		August 2009
	 * \version 0.1
	 * \ingroup utils
	 */
	class ConsoleProgress: public Progress
	{
		public:
			/*! \brief Constructor */
			ConsoleProgress(const String &name, size_t maxcount = 100):Progress(name, maxcount) {}
			/*! \brief Destructor */
			virtual ~ConsoleProgress() override {}
			virtual const String& GetClassName() const override { static const String cn(U"ConsoleProgress"); return cn; }

		protected:
			/*! \brief Displays the progress */
			virtual void display(const String &msg) override;
	};

	CRN_ALIAS_SMART_PTR(ConsoleProgress)
}

#endif
