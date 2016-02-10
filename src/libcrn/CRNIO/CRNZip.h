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
 * file: CRNZip.h
 * \author Yann LEYDIER
 */

#ifndef CRNZip_HEADER
#define CRNZip_HEADER

#include <CRN.h>
#ifdef CRN_USING_LIBZIP

namespace crn
{
	class StringUTF8;
	class Path;

	/*! \brief Zip archive helper 
	 *
	 * \date	Sep. 2013
	 * \author	Yann LEYDIER
	 * \version	0.1
	 * \ingroup io
	 */
	class Zip
	{
		public:
			/*! \brief Opens an existing Zip file */
			static Zip NewFromFile(const Path &fname, bool check_consistency = true);
			/*! \brief Creates an empty Zip file */
			static Zip Create(const Path &fname, bool overwrite, bool check_consistency = true);

			~Zip();
			Zip(const Zip&) = delete;
			Zip(Zip &&z) noexcept;
			Zip& operator=(const Zip&) = delete;
			Zip& operator=(Zip &&z) noexcept;

			/*! \brief Saves the file */
			void Save();
			/*! \brief Shall the archive be saved on destruction of the object? */
			void SetAutoSave(bool autosave);

			/*! \brief Adds a file to the archive from memory data */
			void AddFile(const StringUTF8 &path, const void *data, size_t len, bool overwrite = true);
			/*! \brief Adds a file to the archive from the filesystem */
			void AddFile(const StringUTF8 &path, const Path &original_file, bool overwrite = true);
			/*! \brief Creates a directory in the archive */
			void AddDirectory(const StringUTF8 &path);
			/*! \brief Checks if a file or a directory exists in the archive */
			bool Exists(const StringUTF8 &path);

			/*! \brief Reads a file as a UTF8 string */
			StringUTF8 ReadTextFile(const StringUTF8 &path);

		private:
			/*! \brief internal cstr */
			Zip(const Path &fname, bool create, bool overwrite, bool check_consistency);

			struct Impl;
			std::unique_ptr<Impl> pimpl;
	};

	CRN_ALIAS_SMART_PTR(Zip)
}

#endif// CRN_USING_LIBZIP

#endif

