/* Copyright 2006-2014 Yann LEYDIER, INSA-Lyon, CoReNum
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
 * file: CRNIO.cpp
 * \author Yann LEYDIER
 */

#include <CRNIO/CRNIO.h>
#include <CRNIO/CRNConsoleMessenger.h>

#include <cstdio>
#include <sys/stat.h>
#include <fstream>
#include <CRNException.h>
#include <CRNIO/CRNFileShield.h>
#include <CRNUtils/CRNAtScopeExit.h>
#ifdef _MSC_VER
#	include "./3rdParty/_msvc_only_/dirent.hh"
#	include <direct.h> // for mkdir & rmdir
#	define access _access
#else
#	include <dirent.h>
#	include <unistd.h>
#endif
#include <string.h>
#include <CRNi18n.h>

using namespace crn;

/*!
 * If true, Debug, Warning, Verbose and Error don't print anything.
 * \return	a reference to the configuration variable
 */
bool& IO::IsQuiet()
{
	static bool quiet = false;
	return quiet;
}

/*!
 * Controls whether CRNVerbose prints something or not
 * \return	a reference to the configuration variable
 */
bool& IO::IsVerbose()
{
	static bool verbose = false;
	return verbose;
}


/*! Delegate that will print the messages
 * \return	a reference to the delegate
 */
std::shared_ptr<Messenger>& IO::CurrentMessenger()
{
	static std::shared_ptr<Messenger> messenger(std::make_shared<ConsoleMessenger>());
	return messenger;
}

/*****************************************************************************/
/*!
 * Prints a message with a "Debug" flag
 *
 * \param[in] 	msg The message to print
 */
void IO::Debug(const String &msg)
{
	if (!IsQuiet())
	{
		CurrentMessenger()->PrintDebug(msg);
	}
}

/*****************************************************************************/
/*!
 * Prints a message with a "Warning" flag
 *
 * \param[in] 	msg The message to print
 */
void IO::Warning(const String &msg)
{
	if (!IsQuiet())
	{
		CurrentMessenger()->PrintWarning(msg);
	}
}

/*****************************************************************************/
/*!
 * Prints a information message
 *
 * \param[in] 	msg The message to print
 */
void IO::Verbose(const String &msg)
{
	if (IsVerbose() && !IsQuiet())
	{
		CurrentMessenger()->PrintVerbose(msg);
	}
}

/*****************************************************************************/
/*!
 * Prints a message with a "Error" flag
 *
 * \param[in] 	msg The message to print
 */
void IO::Error(const String &msg)
{
	if (!IsQuiet())
	{
		CurrentMessenger()->PrintError(msg);
	}
}

/*!
 * Creates a directory.
 *
 * \throws	ExceptionIO	cannot create directory
 *
 * \param[in]	name	the name of the directory
 */
void IO::Mkdir(const Path &name)
{
	int res = 0;
#if defined(CRN_PF_WIN32)
	Path winname(name);
	winname.ToWindows();
	res = _mkdir(winname.CStr());
#else
	Path unixname(name);
	unixname.ToUnix();
	res = mkdir(unixname.CStr(), S_IRWXU|S_IRWXG|S_IRGRP|S_IXGRP); // user and group can write. other can read & execute
#endif
	if (res)
		throw ExceptionIO(_("Cannot create directory: ") + StringUTF8(name));
}

/*!
 * Checks accessibility of a file or directory
 *
 * \param[in]	name	the path
 * \param[in]	mode	what to check (IO::EXISTS, IO::READ, IO::WRITE, IO::EXECUTE)
 *
 * \return	true if success, false else
 */
bool IO::Access(const Path &name, int mode)
{
	Path lname(name);
	lname.ToLocal();
	return access(lname.CStr(), mode) == 0 ? true : false;
}

/*!
 * Removes a file
 *
 * \throws	ExceptionIO	cannot remove file
 *
 * \param[in]	name	the name of the file
 */
void IO::Rm(const Path &name)
{
	bool ok;
	Path lname(name);
	lname.ToLocal();
	ok = remove(lname.CStr()) == 0;
	if (!ok)
		throw ExceptionIO(_("Cannot remove: ") + StringUTF8(name));

}

/*!
 * Removes a file and protects it with mutex
 *
 * \throws	ExceptionIO	cannot remove file
 *
 * \param[in]	name	the name of the file
 */
void IO::ShieldRm(const Path &name)
{
	std::lock_guard<std::mutex> lock(FileShield::GetMutex(name));
	Rm(name);
}

/*!
 * Removes a directory and its content
 * \param[in]	name	the name of the directory
 *
 * \throws	ExceptionIO	cannot remove file or directory
 */
void IO::Rmdir(const Path &name)
{
	Path lname(name);
	lname.ToLocal();
	DIR* d=opendir(lname.CStr());
	if(!d)
	{
		throw ExceptionIO(_("Cannot open directory: ") + StringUTF8(lname));
	}
	AtScopeExit([d](){ closedir(d); });
	struct dirent* diren=readdir(d);
	while(diren)
	{

		if((!strcmp(diren->d_name,"."))||(!strcmp(diren->d_name,"..")))
		{
			diren=readdir(d);
			continue;
		}
		char fname[1024];
		snprintf(fname,1024,"%s/%s", lname.CStr(), diren->d_name);
		DIR *tempdir;
		tempdir = opendir(fname);
		if (tempdir)
		{
			closedir(tempdir); tempdir = nullptr;
			strcat(fname,"/");
			Rmdir(fname);
		}
		else
		{
			if(remove(fname) !=0)
			{
				throw ExceptionIO(_("Cannot remove file: ") + StringUTF8(fname));
			}
		}
		diren=readdir(d);
	}
	int ret;
#ifdef CRN_PF_WIN32
	ret = _rmdir(lname.CStr());
#else
	ret = remove(lname.CStr());
#endif
	if (ret)
		throw ExceptionIO(_("Cannot remove directory: ") + StringUTF8(name));

}

/*!
 * Copies a file
 *
 * \throws	ExceptionInvalidArgument	source and destination are equal
 * \throws	ExceptionIO	cannot open source or destination file
 *
 * \param[in]  src  the source file
 * \param[in]  dst  the destination file
 */
void IO::Copy(const Path &src, const Path &dst)
{
	if (src == dst)
		throw ExceptionInvalidArgument(_("Cannot copy a file over itself."));

	std::ifstream s;
	std::ofstream d;

	Path locsrc(src);
	locsrc.ToLocal();
	Path locdst(dst);
	locdst.ToLocal();
	s.open(locsrc.CStr(), std::ios::binary);
	if (!s.is_open())
		throw ExceptionInvalidArgument(_("Cannot open source file: ") + StringUTF8(locsrc));
	d.open(locdst.CStr(), std::ios::binary);
	if (!d.is_open())
		throw ExceptionInvalidArgument(_("Cannot open destination file: ") + StringUTF8(locdst));

	d << s.rdbuf();
	d.close();
	s.close();
}

/*! Copies a file and protects source and destination with mutex
 *
 * \throws	ExceptionInvalidArgument	source and destination are equal
 * \throws	ExceptionIO	cannot open source or destination file
 *
 * \param[in]  src  the source file
 * \param[in]  dst  the destination file
 */
void IO::ShieldCopy(const Path &src, const Path &dst)
{
	if (src == dst) // check before creating the mutex to avoid dead lock!
		throw ExceptionInvalidArgument(_("Cannot copy a file over itself."));
	std::lock_guard<std::mutex> locks(FileShield::GetMutex(src));
	std::lock_guard<std::mutex> lockd(FileShield::GetMutex(dst));
	Copy(src, dst);
}

/*!
 * Reads the content of a directory
 *
 * \throws	ExceptionIO	cannot open directory
 *
 * \param[in]  path  the path of the directory to read
 */
IO::Directory::Directory(const Path &path)
{
	Path lname(path);
	lname.ToLocal();
	DIR* d = opendir(lname.CStr());
	if(!d)
	{
		throw ExceptionIO(StringUTF8(_("Cannot open directory: ")) + StringUTF8(lname));
	}
	struct dirent* diren=readdir(d);
	while(diren)
	{

		if((!strcmp(diren->d_name,"."))||(!strcmp(diren->d_name,"..")))
		{
			diren=readdir(d);
			continue;
		}
		Path fname(lname);
		fname /= diren->d_name;
		DIR *tempdir;
		tempdir = opendir(fname.CStr());
		if (tempdir)
		{
			closedir(tempdir); tempdir = nullptr;
			directories.push_back(fname);
		}
		else
		{
			files.push_back(fname);
		}
		diren = readdir(d);
	}
	closedir(d);

}

