/* Copyright 2007-2014 Yann LEYDIER, CoReNum, INSA-Lyon
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
 * file: CRNException.h
 * \author Yann LEYDIER, Guillaume ETIEVENT
 */

#ifndef CRNEXCEPTION_HEADER
#define CRNEXCEPTION_HEADER

#include <CRN.h>
#include <exception>
#include <string>

/*! \defgroup exception Exceptions 
 * \ingroup	base */

namespace crn
{
	class StringUTF8;
	/*! \addtogroup exception */
	/*@{*/

	/*! \brief Base class for exceptions */
	class Exception: public std::exception
	{
		public:
			/*! \brief Default constructor */
			explicit Exception() noexcept;
			/*! \brief Constructor with a message */
			explicit Exception(const StringUTF8 &msg) noexcept;
			/*! \brief Constructor with a message */
			explicit Exception(const char *msg) noexcept;
			/*! \brief Copy constructor */
			explicit Exception(const Exception &ex) noexcept;
			/*! \brief Destructor */
			virtual ~Exception() noexcept override;
			/*! \brief String containing the call stack at the moment of throw */
			const std::string& GetContext() const noexcept;
			/*! \brief String containing a description of the exception */
			const std::string& GetMessage() const noexcept;
			/*! \brief String containing a description of the exception */
			virtual const char* what() const noexcept override;

			/*! \brief Sets the default exception handler to print message and context to the standard error */
			static void SetDefaultHandler();

			/*! \brief Shall the stack be traced at each exception thrown? (very slow) */
			static bool& TraceStack(); 

		private:
			std::string message; /*!< Description of the error */
			std::string context; /*!< Call stack at creation of the exception */
	};

	/*! \brief A generic logic error */
	class ExceptionLogic: public Exception
	{
		public:
			/*! \brief Default constructor */
			explicit ExceptionLogic() noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionLogic(const crn::StringUTF8 &msg) noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionLogic(const char *msg) noexcept;
	};

	/*! \brief A generic domain error */
	class ExceptionDomain: public ExceptionLogic
	{
		public:
			/*! \brief Default constructor */
			explicit ExceptionDomain() noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionDomain(const crn::StringUTF8 &msg) noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionDomain(const char *msg) noexcept;
	};

	/*! \brief An item was not found in a container */
	class ExceptionNotFound: public ExceptionLogic
	{
		public:
			/*! \brief Default constructor */
			explicit ExceptionNotFound() noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionNotFound(const crn::StringUTF8 &msg) noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionNotFound(const char *msg) noexcept;
	};

	/*! \brief Invalid argument error (e.g.: nullptr pointer) */
	class ExceptionInvalidArgument: public ExceptionLogic
	{
		public:
			/*! \brief Default constructor */
			explicit ExceptionInvalidArgument() noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionInvalidArgument(const crn::StringUTF8 &msg) noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionInvalidArgument(const char *msg) noexcept;
	};

	/*! \brief A dimension error */
	class ExceptionDimension: public ExceptionLogic
	{
		public:
			/*! \brief Default constructor */
			explicit ExceptionDimension() noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionDimension(const crn::StringUTF8 &msg) noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionDimension(const char *msg) noexcept;
	};

	/*! \brief A generic runtime error */
	class ExceptionRuntime: public Exception
	{
		public:
			/*! \brief Default constructor */
			explicit ExceptionRuntime() noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionRuntime(const crn::StringUTF8 &msg) noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionRuntime(const char *msg) noexcept;
	};

	/*! \brief A protocol is not implemented */
	class ExceptionProtocol: public ExceptionRuntime
	{
		public:
			/*! \brief Default constructor */
			explicit ExceptionProtocol() noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionProtocol(const crn::StringUTF8 &msg) noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionProtocol(const char *msg) noexcept;
	};

	/*! \brief Unintialized object error */
	class ExceptionUninitialized: public ExceptionRuntime
	{
		public:
			/*! \brief Default constructor */
			explicit ExceptionUninitialized() noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionUninitialized(const crn::StringUTF8 &msg) noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionUninitialized(const char *msg) noexcept;
	};

	/*! \brief Something to be done */
	class ExceptionTODO: public ExceptionRuntime
	{
		public:
			/*! \brief Default constructor */
			explicit ExceptionTODO() noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionTODO(const crn::StringUTF8 &msg) noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionTODO(const char *msg) noexcept;
	};

	/*! \brief I/O error */
	class ExceptionIO: public ExceptionRuntime
	{
		public:
			/*! \brief Default constructor */
			explicit ExceptionIO() noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionIO(const crn::StringUTF8 &msg) noexcept;
			/*! \brief Constructor with a message */
			explicit ExceptionIO(const char *msg) noexcept;
	};

	/*@}*/
}

#endif
