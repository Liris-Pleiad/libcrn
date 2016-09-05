/* Copyright 2011-2016 CoReNum
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
 * file: CRNAltoDescription.h
 * \author Yann LEYDIER
 */

/*! \cond */
#ifdef CRNAlto_HEADER
/*! \endcond */

/*! \brief Alto global description */
class Description: public Element
{
	public:
		Description(const Description&) = delete;
		Description(Description&&) = default;
		virtual ~Description() override {}
		Description& operator=(const Description&) = delete;
		Description& operator=(Description&&) = default;

		enum class MeasurementUnit { Undef = 0, Pixel, MM, Inch };
		/*! \brief Returns the measurement unit of all metrics in the Alto */
		Option<MeasurementUnit> GetMeasurementUnit();
		/*! \brief Returns the filename of the image described by the Alto */
		Option<StringUTF8> GetFilename();

	private:
		/*! \brief Constructor from file */
		Description(const Element &el);
		/*! \brief Constructor from file name */
		Description(const Element &el, const Path &imagename);

	friend class Root;
};

#else
#error	you cannot include this file directly
#endif

