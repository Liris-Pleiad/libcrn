/* Copyright 2010-2014 CoReNum, INSA-Lyon
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
 * file: Module.h
 * \author Yann LEYDIER
 */

#ifndef CRNModule_HEADER
#define CRNModule_HEADER

#include <CRNString.h>
#include <CRNIO/CRNIO.h>

#ifdef CRN_PF_WIN32
#	include <windows.h>
// this is insane!!!
#	undef GetClassName
#	undef DELETE
#	undef ABSOLUTE
#	undef max
#	undef min
#	undef RGB
#	include <errno.h>
	inline HMODULE dlopen(const char *P, int G)
	{
		return LoadLibrary(P);
	}
#	define dlsym(D,F) (void*)GetProcAddress((HMODULE)D,F)
#	define dlclose(D) FreeLibrary((HMODULE)D)
	inline const char* dlerror()
	{
					static char szMsgBuf[256];
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
													nullptr,
													GetLastError(),
													MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
													szMsgBuf,
													sizeof szMsgBuf,
													nullptr);
					return szMsgBuf;
	}
#	define RTLD_LAZY   1
#	define RTLD_NOW    2
#	define RTLD_GLOBAL 4
#else
#	include <dlfcn.h>
#endif /* CRN_PF_WIN32 */

/*! \defgroup module Dynamically loaded modules 
 * \ingroup	utils */

namespace crn
{
	/*! \brief A dynamic module manager
	 *
	 * A dynamic module manager.
	 *
	 * The Category class declaration must call the macro CRN_DECLARE_MODULE(Category).
	 *
	 * \author Yann LEYDIER
	 * \date May 2010
	 * \ingroup module
	 */
	template<class Category> class ModuleManager
	{
		public:
			/*! \brief Loads modules contained in a directory
			 *
			 * Loads modules contained in a directory.
			 *
			 * \param[in]  dirname  the path to the module files
			 * \param[in]  ext  the extension of the files to load (leave blank to load all files)
			 * \return  the number of modules loaded
			 */
			int LoadDirectory(const Path &dirname, const String &ext = U"")
			{
				try
				{
					crn::IO::Directory dir(dirname);
					int cnt = 0;
					for (const crn::Path filename : dir.GetFiles())
					{ // for each file in the directory
						if (ext.IsNotEmpty())
						{ // check extension
							if (filename.SubString(filename.Size() - ext.Size()) != ext)
							{
								continue;
							}
						}
						void *handle = dlopen(filename.CStr(), RTLD_NOW | RTLD_GLOBAL);
						if (handle)
						{ // if the file is a loadable library
							handles.push_back(handle);
							Category* (*handleget)(void);
							handleget = (Category* (*)(void))dlsym(handle, Category::GetModuleEntryPoint());
							if (handleget)
							{ // if there is a crn_module_get function
								if (RegisterModule(std::shared_ptr<Category>(handleget())))
									cnt += 1;
							}
							else
							{ // no crn_module_get_XXX function
								CRNDebug(String(U"int module::LoadDirectory(const String &dirname): Cannot find entry point in file ") + filename + String(U" (") + dlerror() + String(U")"));
							}
						}
						else
						{ // not a loadable library
							CRNError(String(U"int module::LoadDirectory(const String &dirname): Cannot open file ") + filename + String(U" (") + dlerror() + String(U")"));
						}
					}
					return cnt;
				}
				catch (...)
				{ // directory not found
					CRNError(crn::String(U"int module::LoadDirectory(const String &dirname): Cannot open directory ") + dirname);
					return 0;
				}
			}

			/*! \brief Retrieves the modules that implement a given interface */
			std::vector<std::shared_ptr<Category>> GetModules() const
			{
				return modules;
			}

			/*! \brief Registers a new module. DO NOT USE THIS METHOD DIRECTLY. Call CRN_BEGIN/END_MODULE. */
			bool RegisterModule(std::shared_ptr<Category> mod)
			{
				if (!mod)
				{
					CRNError(String(U"bool ModuleManager::RegisterModule(Category *mod): The module is of wrong category."));
					return false;
				}
				else
				{
					modules.push_back(mod);
					return true;
				}
			}

			/*! \brief Default constructor */
			ModuleManager() {}
			/*! \brief Destructor frees handles */
			~ModuleManager()
			{
				modules.clear(); // free all pointers before we close the libs!
				for (void *h : handles)
				{
					dlclose(h);
				}
			}
		private:
			std::vector<void*> handles; /*!< handles on the libraries */
			std::vector<std::shared_ptr<Category> > modules; /*!< storage for modules */
	};

}

/*!
 * \brief Internal
 * \ingroup module
 */
#define CRN_MODULE_ENTRY_POINT(category) crn_module_get_##category
/*!
 * \brief Internal
 * \ingroup module
 */
#define CRN_MODULE_ENTRY_POINT_AS_STRING(category) "crn_module_get_"#category

/*!
 * \brief Declares that the current class is a base for dynamic modules.
 *
 * Declares that the current class is a base for dynamic modules.
 *
 * \param[in]  categoryname  the name of the class being declared as a base for dynamic modules.
 * \ingroup module
 */
#define CRN_DECLARE_MODULE(categoryname) public: static const char* GetModuleEntryPoint() { return CRN_MODULE_ENTRY_POINT_AS_STRING(categoryname); }

/*!
 * \brief Begins the declaration of a module
 *
 * Begins the declaration of a module.
 *
 * A binary file may contain only one module. Use this macro in a .cpp file.
 *
 * Example:\n
 * CRN_BEGIN_MODULE(Meow, AnimalTalk)\n
 *   public:\n
 *     Meow();\n
 *     virtual void Talk();\n
 * CRN_END_MODULE(Meow, AnimalTalk)
 *
 * \param[in]  classname  the name of the class to register
 * \param[in]  categoryname  the name of the interface (abstract base class of <i>classname</i>)
 * \ingroup module
 */
#define CRN_BEGIN_MODULE(classname, categoryname) class classname: public categoryname {

/*!
 * \brief End the declaration and registration of a module
 *
 * End the declaration and registration of a module.
 *
 * A binary file may contain only one module. Use this macro in a .cpp file.
 *
 * Example:\n
 * CRN_BEGIN_MODULE(Meow, AnimalTalk)\n
 *   public:\n
 *     Meow();\n
 *     virtual void Talk();\n
 * CRN_END_MODULE(Meow, AnimalTalk)
 *
 * \param[in]  classname  the name of the class to register
 * \param[in]  categoryname  the name of the interface (abstract base class of <i>classname</i>)
 * \ingroup module
 */
#define CRN_END_MODULE(classname, categoryname) }; extern "C" categoryname* CRN_MODULE_ENTRY_POINT(categoryname)() { return new classname; }

#endif

