/* Copyright 2013-2014 INSA-Lyon, CoReNum
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
 * file: CRNZip.cpp
 * \author Yann LEYDIER
 */

#include <CRN.h>
#ifdef CRN_USING_LIBZIP

#include <CRNIO/CRNZip.h>
#include <zip.h>
#include <CRNException.h>
#include <CRNIO/CRNIO.h>
#include <CRNi18n.h>

#if LIBZIP_VERSION_MINOR == 10
#define zip_dir_add(arch, name, flags) zip_add_dir(arch, name)
#define zip_file_add(arch, name, source, flags) zip_add(arch, name, source)
using zip_flags_t = int;
#define ZIP_FL_OVERWRITE 0
#define ZIP_FL_ENC_UTF_8 0
#define ZIP_TRUNCATE 0
#endif
using namespace crn;

/*! \cond */
struct Zip::Impl
{
	Impl(const Path &f, bool c):zipfile(nullptr),autosave(true),saved(false),fname(f),check(c) { }
	~Impl()
	{
		if (!zipfile)
			return;
		if (autosave)
		{ 
#if LIBZIP_VERSION_MINOR != 10
			if (!saved)
			{
#endif
				int res = zip_close(zipfile); 
				if (res == -1)
				{
					// do not throw because it is quite evil
					CRNError(zip_strerror(zipfile));
				}
				/* TODO check if needed with mingw-w64
#ifdef CRN_PF_MSVC
				// remove read-only attribute...
				SetFileAttributes(String(fname).CWStr(), FILE_ATTRIBUTE_NORMAL);
#endif
*/
#if LIBZIP_VERSION_MINOR != 10
			}
			else
			{
				zip_discard(zipfile);
			}
#endif
		}
		else
		{
#if LIBZIP_VERSION_MINOR == 10
			throw ExceptionRuntime(_("libzip version is too old to support zip_discard()."));
#else
			zip_discard(zipfile);
#endif
		}
	}

	zip *zipfile;
	bool autosave;
	bool saved;
	const Path &fname;
	bool check;
};
/*! \endcond */

/*! Opens an existing Zip file
 * \throws	ExceptionIO	won't overwrite file
 * \throws	ExceptionIO	cannot open or read file
 * \throws	ExceptionRuntime	file is inconsistent or not a zip file
 * \throws	ExceptionRuntime	not enough memory
 * \param[in]	fname	the path to the archive
 * \param[in]	check_consistency	do extra checks
 * \return	a new Zip handler
 */
Zip Zip::NewFromFile(const Path &fname, bool check_consistency)
{
	return Zip(fname, false, false, check_consistency);
}

/*! Opens an existing Zip file
 * \throws	ExceptionIO	won't overwrite file
 * \throws	ExceptionIO	cannot open or read file
 * \throws	ExceptionRuntime	file is inconsistent or not a zip file
 * \throws	ExceptionRuntime	not enough memory
 * \param[in]	fname	the path to the archive
 * \param[in]	overwrite	overwrites if the file already exists (not working with libzip version 0.10)
 * \param[in]	check_consistency	do extra checks
 * \return	a new Zip handler
 */
Zip Zip::Create(const Path &fname, bool overwrite, bool check_consistency)
{
	return Zip(fname, true, overwrite, check_consistency);
}

/*! 
 * \throws	ExceptionIO	won't overwrite file
 * \throws	ExceptionIO	cannot open or read file
 * \throws	ExceptionRuntime	file is inconsistent or not a zip file
 * \throws	ExceptionRuntime	not enough memory
 * \param[in]	fname	the path to the archive
 * \param[in]	create	new archive or load
 * \param[in]	overwrite	overwrites if the file already exists
 * \param[in]	check_consistency	do extra checks
 */
Zip::Zip(const Path &fname, bool create, bool overwrite, bool check_consistency):
	pimpl(std::make_unique<Impl>(fname, check_consistency))
{
	int flags = 0;
	if (create)
	{
		flags |= ZIP_CREATE;
		if (overwrite)
			flags |= ZIP_TRUNCATE;
		else
			flags |= ZIP_EXCL;
		pimpl->saved = false;
	}
	if (check_consistency)
		flags |= ZIP_CHECKCONS;
	int error = 0;
	zip *zfile = zip_open(fname.CStr(), flags, &error);
	if (error == 0)
		pimpl->zipfile = zfile;
	else
	{
		switch (error)
		{
			case ZIP_ER_EXISTS:
				throw ExceptionIO(_("The file already exists."));
			case ZIP_ER_INCONS:
				throw ExceptionRuntime(_("The file is inconsistent."));
				// case ZIP_ER_INVAL: // cannot happen!
			case ZIP_ER_MEMORY:
				throw ExceptionRuntime(_("Not enough memory."));
			case ZIP_ER_NOZIP:
				throw ExceptionRuntime(_("This is not a zip file."));
			case ZIP_ER_NOENT:
			case ZIP_ER_OPEN:
				throw ExceptionIO(_("Cannot open file."));
			case ZIP_ER_READ:
			case ZIP_ER_SEEK:
				throw ExceptionIO(_("Cannot read file."));
			default:
				throw ExceptionRuntime(_("Unknown zip error."));
		}
	}
}

Zip::~Zip()
{
}

Zip::Zip(Zip &&z) noexcept:
	pimpl(std::move(z.pimpl))
{
}

Zip& Zip::operator=(Zip &&z) noexcept
{
	pimpl = std::move(z.pimpl);
	return *this;
}

/*! Saves the file */
void Zip::Save()
{
	if (!pimpl->saved)
	{
		// close to save
		int res = zip_close(pimpl->zipfile); 
		pimpl->zipfile = nullptr;
		if (res == -1)
		{
			throw ExceptionIO(zip_strerror(pimpl->zipfile));
		}
		/* TODO check if needed with mingw-w64!
#ifdef CRN_PF_MSVC
		// remove read-only attribute...
		SetFileAttributes(String(pimpl->fname).CWStr(), FILE_ATTRIBUTE_NORMAL);
#endif
		*/
		// reopen
		int error = 0;
		zip *zfile = zip_open(pimpl->fname.CStr(), 0, &error);
		if (error == 0)
			pimpl->zipfile = zfile;
		else
		{
			switch (error)
			{
				case ZIP_ER_EXISTS:
					throw ExceptionIO(_("The file already exists."));
				case ZIP_ER_INCONS:
					throw ExceptionRuntime(_("The file is inconsistent."));
					// case ZIP_ER_INVAL: // cannot happen!
				case ZIP_ER_MEMORY:
					throw ExceptionRuntime(_("Not enough memory."));
				case ZIP_ER_NOZIP:
					throw ExceptionRuntime(_("This is not a zip file."));
				case ZIP_ER_NOENT:
				case ZIP_ER_OPEN:
					throw ExceptionIO(_("Cannot open file."));
				case ZIP_ER_READ:
				case ZIP_ER_SEEK:
					throw ExceptionIO(_("Cannot read file."));
				default:
					throw ExceptionRuntime(_("Unknown zip error."));
			}
		}
		pimpl->saved = true;
	}
}

/*!
 * \warning	The data added to the archive MUST NOT be destroyed before the Zip object!
 * \param	autosave	Shall the archive be saved on destruction of the object? (default = yes)
*/
void Zip::SetAutoSave(bool autosave)
{
	pimpl->autosave = autosave;
}

/*! Adds a file to the archive from memory data
 * \throws	ExceptionRuntime	cannot add file (libzip internal error)
 * \param[in]	path	the internal path of the file
 * \param[in]	data	a raw pointer on the data
 * \param[in]	len	the length of the data (in bytes)
 * \param[in]	overwrite	what to do when the file is already in the archive
 */
void Zip::AddFile(const StringUTF8 &path, const void *data, size_t len, bool overwrite)
{
	zip_flags_t flags = ZIP_FL_ENC_UTF_8;
	if (overwrite)
		flags |= ZIP_FL_OVERWRITE;
	zip_source *source = zip_source_buffer(pimpl->zipfile, data, len, 0);
	if (!source)
		throw ExceptionRuntime(zip_strerror(pimpl->zipfile));
	zip_int64_t res = zip_file_add(pimpl->zipfile, path.CStr(), source, flags);
	if (res < 0)
	{
		zip_source_free(source);
		throw ExceptionRuntime(zip_strerror(pimpl->zipfile));
	}
}

/*! Adds a file to the archive from the filesystem
 * \throws	ExceptionRuntime	cannot add file (libzip internal error)
 * \param[in]	path	the internal path of the file
 * \param[in]	original_file	the path on the filesystem to the file to add
 * \param[in]	overwrite	what to do when the file is already in the archive
 */
void Zip::AddFile(const StringUTF8 &path, const Path &original_file, bool overwrite)
{
	zip_flags_t flags = ZIP_FL_ENC_UTF_8;
	if (overwrite)
		flags |= ZIP_FL_OVERWRITE;
	zip_source *source = zip_source_file(pimpl->zipfile, original_file.CStr(), 0, 0);
	if (!source)
		throw ExceptionRuntime(zip_strerror(pimpl->zipfile));
	zip_int64_t res = zip_file_add(pimpl->zipfile, path.CStr(), source, flags);
	if (res < 0)
	{
		zip_source_free(source);
		throw ExceptionRuntime(zip_strerror(pimpl->zipfile));
	}
}

/*! Reads a file as a UTF8 string
 * \throws	ExceptionRuntime	cannot read file (libzip internal error)
 * \param[in]	path	the internal path to the file
 * \return	the content of the file as a UTF8 string
 */
StringUTF8 Zip::ReadTextFile(const StringUTF8 &path)
{
	// get file size
	struct zip_stat fstat;
	if (zip_stat(pimpl->zipfile, path.CStr(), ZIP_STAT_SIZE, &fstat) == -1)
		throw ExceptionRuntime(zip_strerror(pimpl->zipfile));

	// get file handler
	zip_file *f = zip_fopen(pimpl->zipfile, path.CStr(), 0);
	if (!f)
		throw ExceptionRuntime(zip_strerror(pimpl->zipfile));

	// read data
	StringUTF8 str('\0', size_t(fstat.size));
	zip_int64_t res = zip_fread(f, &(str[0]), fstat.size);

	// close file
	zip_fclose(f);

	if (res == -1)
		throw ExceptionRuntime(zip_strerror(pimpl->zipfile));

	return str;
}

/*! Creates a directory in the archive
 * \throws	ExceptionRuntime	cannot read file (libzip internal error)
 * \param[in]	path	the name of the directory
 */
void Zip::AddDirectory(const StringUTF8 &path)
{
	if (zip_dir_add(pimpl->zipfile, path.CStr(), ZIP_FL_ENC_UTF_8) == -1)
		throw ExceptionRuntime(zip_strerror(pimpl->zipfile));
}

/*! Checks if a file or a directory exists in the archive
 * \warning	directories MUST end with a "/"
 * \param[in]	path	the path to the file or the directory
 * \return	true if found, false else
 */
bool Zip::Exists(const StringUTF8 &path)
{
	struct zip_stat fstat;
	if (zip_stat(pimpl->zipfile, path.CStr(), ZIP_STAT_NAME, &fstat) == -1)
		return false;
	else
		return true;
}

#endif// CRN_USING_LIBZIP
