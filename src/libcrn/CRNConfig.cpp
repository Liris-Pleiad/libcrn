/* Copyright 2006-2014 INSA Lyon
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
 * file: CRNConfig.cpp
 * \author Yann LEYDIER
 */

#include <CRNConfig.h>
#include <CRNException.h>
#include <CRNIO/CRNIO.h>
#include <CRNi18n.h>

#ifdef CRN_USING_GDKPB
#	include <gdk-pixbuf/gdk-pixbuf.h>
#endif
#ifdef CRN_ENABLE_OPENMP
#	include <omp.h>
#endif

using namespace crn;

Config::Config():
	conf("crn")
{
	if (!conf.Load())
	{
		// default init
		conf.SetData(topDirKey(), Path(CRN_PROJECT_PATH));
		conf.SetData(localeDirKey(), Path(CRN_LOCALE_FULL_PATH));
		conf.SetData(staticDataDirKey(), Path(CRN_DATA_FULL_PATH));
		conf.SetData(verboseKey(), Prop3::True());
		conf.Save();
	}
	IO::IsVerbose() = conf.GetProp3(verboseKey()).IsTrue();
	// misc init
#ifdef CRN_USING_GDKPB
#	if !GLIB_VERSION_2_36
	g_type_init();
#	endif
#endif

	// i18n
	char *loc = getenv("LANG");
	CRNdout << "libcrn LANG env: " << (loc == nullptr ? "none" : loc) << std::endl;
	
#if !defined(CRN_USING_GLIB_INTL) && !defined(CRN_USING_LIBINTL)
	loc = setlocale(LC_ALL, "C");
#else
	loc = setlocale(LC_ALL, "");
#endif
	
	if (!loc)
		CRNdout << "libcrn setlocale failed" << std::endl;
	else
		CRNdout << "libcrn locale = " << loc << std::endl;
	
	loc = CRNbindtextdomain(GETTEXT_PACKAGE, conf.GetPath(localeDirKey()).CStr());
	if (!loc)
		CRNdout << "libcrn: no bound path. should be " << conf.GetPath(localeDirKey()).CStr() << std::endl;
	else
		CRNdout << "libcrn: path set to " << loc << std::endl;
	loc = CRNbind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	if (!loc)
		CRNdout << "libcrn: no bound codeset. should be UTF-8" << std::endl;
	else
		CRNdout << "libcrn: Codeset = " << loc << std::endl;
	loc = CRNtextdomain(GETTEXT_PACKAGE);
	if (!loc)
		CRNdout << "libcrn textdomain failed" << std::endl;
	else
		CRNdout << "libcrn text domain = " << loc << std::endl;
	CRNdout << _("Using default language.") << std::endl;
}

Config::~Config()
{
}

Config& Config::getInstance()
{
	static Config conf;
	return conf;
}

String Config::topDirKey()
{
	return U"FilesPath";
}

String Config::localeDirKey()
{
	return U"LocalePath";
}

String Config::staticDataDirKey()
{
	return U"StaticDataPath";
}

String Config::verboseKey()
{
	return U"Verbose";
}

bool Config::Save()
{
	getInstance().conf.SetData(verboseKey(), Prop3(IO::IsVerbose()));
	return getInstance().conf.Save().IsNotEmpty();
}

/*! Sets the top directory name
 * \param[in]	dir	the new top directory
 */
void Config::SetTopDataPath(const Path &dir)
{
	getInstance().conf.SetData(topDirKey(), dir);
}

/*! Gets the top directory name 
 * \return	the top directory
 */
Path Config::GetTopDataPath()
{
	return getInstance().conf.GetPath(topDirKey());
}

/*! Gets the translation files path 
 * \return	the translation files directory
 */
Path Config::GetLocalePath()
{
	return getInstance().conf.GetPath(localeDirKey());
}

/*! Sets the translation files path
 * \param[in]	dir	the new translation files directory
 */
void Config::SetLocalePath(const crn::Path &dir)
{
	getInstance().conf.SetData(localeDirKey(), dir);
}

/*! Gets the data directory name 
 * \return	the data files directory
 */
Path Config::GetStaticDataPath()
{
	return getInstance().conf.GetPath(staticDataDirKey());
}

/*! Sets the data directory name 
 * \param[in]	dir	the new data files directory
 */
void Config::SetStaticDataPath(const crn::Path &dir)
{
	getInstance().conf.SetData(staticDataDirKey(), dir);
}

/*! \cond */
CRN_BEGIN_CLASS_CONSTRUCTOR(Config)
	ConfigurationFile::Initialize();
	getInstance();
CRN_END_CLASS_CONSTRUCTOR(Config)
/*! \endcond */

