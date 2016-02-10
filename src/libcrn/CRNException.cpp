/* Copyright 2009-2014 INSA Lyon
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
 * file: CRNException.cpp
 * \author Yann LEYDIER, Guillaume ETIENVENT
 */

#include <CRNException.h>
#include <CRNStringUTF8.h>
#ifdef CRN_PF_UNIX
#	ifndef CRN_PF_ANDROID
#		include <execinfo.h>
#		include <cxxabi.h>
#		include <cstring>
#	endif
#endif
#include <iostream>
#ifdef __GNUG__
#	if !defined(CRN_PF_ANDROID) && !defined(CRN_PF_WIN32)
#		define DEMANGLE
#	endif
#endif

using namespace crn;

static const std::string getCallStack()
{
	if (!Exception::TraceStack())
		return "";
#ifdef CRN_PF_WIN32
	return "";
#else
#	ifdef CRN_PF_ANDROID
	return "";
#	else
	const size_t maxdepth = 20;
	void *array[maxdepth];
	size_t size = backtrace(array, maxdepth);
	char **strings = backtrace_symbols(array, int(size));
	std::string str;
	if (size > 1)
		for (size_t tmp = 1; tmp < size; ++tmp)
		{
			std::string line(strings[tmp]);
			char *begin = nullptr, *end = nullptr;
			for (char *j = strings[tmp]; *j != '\0'; ++j)
			{
				if (*j == '(')
				{
					begin = j;
				}
				else if (*j == '+')
				{
					end = j;
					break;
				}
			}
			if (begin && end)
			{
				*end = '\0';
				begin += 1;
				int status = -1;
				char *ret = abi::__cxa_demangle(begin, nullptr, nullptr, &status);
				if (status == 0)
				{
					const char *excstring = "crn::Exception";
					if (strncmp(ret, excstring, strlen(excstring)) != 0)
						line = ret;
					else
						line = "";
				}
				free(ret);
				*end = '+';
			}
			if (line.size())
			{
				str += line;
				str += "\n";
			}
		}
	free(strings);
	if (size == maxdepth)
		str += "…";
	return str;
#	endif
#endif
}

/*! Shall the stack be traced at each exception thrown (default: false).
 * \warning SLOW!
 * \return	a reference (ie editable variable) to the flag
 */
bool& Exception::TraceStack()
{
	static bool trace = false;
	return trace;
}

/*! Default constructor */
Exception::Exception() noexcept:
	context(getCallStack())
{
}

/*! Constructor with a message
 * \param[in]	msg	the message describing the exception
 */
Exception::Exception(const crn::StringUTF8 &msg) noexcept:
	message(msg.Std()),
	context(getCallStack())
{
}

/*! Constructor with a message
 * \param[in]	msg	the message describing the exception
 */
Exception::Exception(const char *msg) noexcept:
	message(msg),
	context(getCallStack())
{
}

/*! Constructor with a message
 * \param[in]	ex	the message describing the exception
 */
Exception::Exception(const Exception &ex) noexcept:
	std::exception(ex),
	message(ex.message),
	context(ex.context)
{
}

/*! Destructor */
Exception::~Exception() noexcept
{
}

/*! String containing the call stack at the moment of throw */
const std::string& Exception::GetContext() const noexcept
{
	return context;
}

/*! String containing a description of the exception */
const std::string& Exception::GetMessage() const noexcept
{
	return message;
}

/*! String containing a description of the exception */
const char* Exception::what() const noexcept
{
	return message.c_str();
}

static void crnterminate()
{
	try
	{
		throw;
	}
	catch (const Exception &ex)
	{
		const char *exname = typeid(ex).name();
#ifdef DEMANGLE
		int status = -1;
		char *ret = abi::__cxa_demangle(exname, nullptr, nullptr, &status);
		if (status == 0)
		{
			exname = ret;
		}
#endif
		std::cerr << "Unhandled <" << exname << ">: " << std::endl;
#ifdef DEMANGLE
		free(ret);
#endif
		std::cerr << "what: " << ex.what() << std::endl;
		std::cerr << "context: " << ex.GetContext().c_str() << std::endl;
	}
	catch (const std::exception &ex)
	{
		const char *exname = typeid(ex).name();
#ifdef DEMANGLE
		int status = -1;
		char *ret = abi::__cxa_demangle(exname, nullptr, nullptr, &status);
		if (status == 0)
		{
			exname = ret;
		}
#endif
		std::cerr << "Unhandled <" << exname << ">: " << std::endl;
#ifdef DEMANGLE
		free(ret);
#endif
		std::cerr << "what: " << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unexpected exception!" << std::endl;
	}
	abort();
}

/*! Sets the default exception handler to print message and context to the standard error */
void Exception::SetDefaultHandler()
{
	std::set_terminate(crnterminate);
}

/*! Default constructor */
ExceptionLogic::ExceptionLogic() noexcept { }
/*! Constructor with a message */
ExceptionLogic::ExceptionLogic(const crn::StringUTF8 &msg) noexcept:Exception(msg) { }
/*! Constructor with a message */
ExceptionLogic::ExceptionLogic(const char *msg) noexcept:Exception(msg) { }

/*! Default constructor */
ExceptionDomain::ExceptionDomain() noexcept { }
/*! Constructor with a message */
ExceptionDomain::ExceptionDomain(const crn::StringUTF8 &msg) noexcept:ExceptionLogic(msg) { }
/*! Constructor with a message */
ExceptionDomain::ExceptionDomain(const char *msg) noexcept:ExceptionLogic(msg) { }

/*! Default constructor */
ExceptionNotFound::ExceptionNotFound() noexcept { }
/*! Constructor with a message */
ExceptionNotFound::ExceptionNotFound(const crn::StringUTF8 &msg) noexcept:ExceptionLogic(msg) { }
/*! Constructor with a message */
ExceptionNotFound::ExceptionNotFound(const char *msg) noexcept:ExceptionLogic(msg) { }

/*! Default constructor */
ExceptionInvalidArgument::ExceptionInvalidArgument() noexcept { }
/*! Constructor with a message */
ExceptionInvalidArgument::ExceptionInvalidArgument(const crn::StringUTF8 &msg) noexcept:ExceptionLogic(msg) { }
/*! Constructor with a message */
ExceptionInvalidArgument::ExceptionInvalidArgument(const char *msg) noexcept:ExceptionLogic(msg) { }

/*! Default constructor */
ExceptionDimension::ExceptionDimension() noexcept { }
/*! Constructor with a message */
ExceptionDimension::ExceptionDimension(const crn::StringUTF8 &msg) noexcept:ExceptionLogic(msg) { }
/*! Constructor with a message */
ExceptionDimension::ExceptionDimension(const char *msg) noexcept:ExceptionLogic(msg) { }

/*! Default constructor */
ExceptionRuntime::ExceptionRuntime() noexcept { }
/*! Constructor with a message */
ExceptionRuntime::ExceptionRuntime(const crn::StringUTF8 &msg) noexcept:Exception(msg) { }
/*! Constructor with a message */
ExceptionRuntime::ExceptionRuntime(const char *msg) noexcept:Exception(msg) {}

/*! Default constructor */
ExceptionProtocol::ExceptionProtocol() noexcept { }
/*! Constructor with a message */
ExceptionProtocol::ExceptionProtocol(const crn::StringUTF8 &msg) noexcept:ExceptionRuntime(msg) { }
/*! Constructor with a message */
ExceptionProtocol::ExceptionProtocol(const char *msg) noexcept:ExceptionRuntime(msg) { }

/*! Default constructor */
ExceptionUninitialized::ExceptionUninitialized() noexcept { }
/*! Constructor with a message */
ExceptionUninitialized::ExceptionUninitialized(const crn::StringUTF8 &msg) noexcept:ExceptionRuntime(msg) { }
/*! Constructor with a message */
ExceptionUninitialized::ExceptionUninitialized(const char *msg) noexcept:ExceptionRuntime(msg) { }

/*! Default constructor */
ExceptionTODO::ExceptionTODO() noexcept { }
/*! Constructor with a message */
ExceptionTODO::ExceptionTODO(const crn::StringUTF8 &msg) noexcept:ExceptionRuntime(msg) { }
/*! Constructor with a message */
ExceptionTODO::ExceptionTODO(const char *msg) noexcept:ExceptionRuntime(msg) { }

/*! Default constructor */
ExceptionIO::ExceptionIO() noexcept { }
/*! Constructor with a message */
ExceptionIO::ExceptionIO(const crn::StringUTF8 &msg) noexcept:ExceptionRuntime(msg) { }
/*! Constructor with a message */
ExceptionIO::ExceptionIO(const char *msg) noexcept:ExceptionRuntime(msg) { }

