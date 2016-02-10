/* Copyright 2012-2014 CoReNum, INSA-Lyon
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
 * file: CRNCharsetConverter.h
 * \author Yann LEYDIER
 */

#ifndef CRNCharsetConverter_HEADER
#define CRNCharsetConverter_HEADER

#include <iconv.h>
#include <CRNException.h>

namespace crn
{
	class StringUTF8;

	/*! \brief Character set converter
	 *
	 * A class to convert from any character set to UTF-8 and vice versa
	 *
	 * \version 0.1
	 * \author Yann LEYDIER
	 * \date	Apr 2012
	 * \ingroup utils
	 */
	class CharsetConverter
	{
		public:
			enum class Status { OK, BUFFER, INVALID, INCOMPLETE };

			class Exception: public crn::Exception
			{
				public:
					/*! \brief Default constructor */
					explicit Exception() noexcept;
					/*! \brief Constructor with a message */
					explicit Exception(const crn::StringUTF8 &msg) noexcept;
					/*! \brief Constructor with a message */
					explicit Exception(const char *msg) noexcept;
			};

			class ExceptionInvalidCharacter: public Exception
			{
				public:
					/*! \brief Default constructor */
					explicit ExceptionInvalidCharacter() noexcept;
					/*! \brief Constructor with a message */
					explicit ExceptionInvalidCharacter(const crn::StringUTF8 &msg) noexcept;
					/*! \brief Constructor with a message */
					explicit ExceptionInvalidCharacter(const char *msg) noexcept;
			};

			class ExceptionIncompleteCode: public Exception
			{
				public:
					/*! \brief Default constructor */
					explicit ExceptionIncompleteCode() throw();
					/*! \brief Constructor with a message */
					explicit ExceptionIncompleteCode(const crn::StringUTF8 &msg) noexcept;
					/*! \brief Constructor with a message */
					explicit ExceptionIncompleteCode(const char *msg) noexcept;
			};

			/*! \brief Constructor */
			CharsetConverter(const std::string &to_code, bool translit = true, bool throw_exceptions = true);
			CharsetConverter(const CharsetConverter&) = delete;
			CharsetConverter(CharsetConverter&&) = delete;
			CharsetConverter& operator=(const CharsetConverter&) = delete;
			CharsetConverter& operator=(CharsetConverter&&) = delete;
			/*! \brief Destructor */
			~CharsetConverter();

			/*! \brief Changes the charset to convert */
			void Reset(const std::string &to_code, bool translit = true);
			/*! \brief Converts from unicode to the selected charset */
			std::string FromUTF8(const crn::StringUTF8 &str, Status *stat = nullptr) const;
			/*! \brief Converts to unicode */
			crn::StringUTF8 ToUTF8(const std::string &str, Status *stat = nullptr) const;

			/*! \brief Returns the used charset */
			const std::string& GetCharset() const { return current_code; }

		private:
			/*! \brief Converts from unicode to the selected charset */
			std::string fromUTF8(const crn::StringUTF8 &str, size_t buff, Status *stat) const;
			/*! \brief Converts to unicode */
			crn::StringUTF8 toUTF8(const std::string &str, size_t buff, Status *stat) const;

			mutable iconv_t toutf, fromutf; /*!< Internal wrapper to iconv */
			bool silent; /*!< true if the converter does nothing */
			std::string current_code; /*!< The current conversion */
			bool throws; /*!< Shall exceptions be thrown if an error occurs? */
	};
}

#include <CRNUtils/CRNCharsetConverterPtr.h>
#endif

