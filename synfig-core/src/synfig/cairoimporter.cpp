/* === S Y N F I G ========================================================= */
/*!	\file cairoimporter.cpp
**	\brief It is the base class for all the cairo importers.
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**	Copyright (c) 2012 Carlos López
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "canvas.h"
#include "cairoimporter.h"
#include "surface.h"
#include <algorithm>
#include "string.h"
#include <map>
#include <ctype.h>
#include <functional>

#endif

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

using namespace etl;
using namespace std;
using namespace synfig;

CairoImporter::Book* synfig::CairoImporter::book_;

map<String, CairoImporter::LooseHandle> *__open_cairoimporters;

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

bool
CairoImporter::subsys_init()
{
	book_=new Book();
	__open_cairoimporters=new map<String, CairoImporter::LooseHandle>();
	return true;
}

bool
CairoImporter::subsys_stop()
{
	delete book_;
	delete __open_cairoimporters;
	return true;
}

CairoImporter::Book&
CairoImporter::book()
{
	return *book_;
}

CairoImporter::Handle
CairoImporter::open(const String &filename)
{
	if(filename.empty())
	{
		synfig::error(_("Importer::open(): Cannot open empty filename"));
		return 0;
	}

	// If we already have an importer open under that filename,
	// then use it instead.
	if(__open_cairoimporters->count(filename))
	{
		//synfig::info("Found importer already open, using it...");
		return (*__open_cairoimporters)[filename];
	}

	if(filename_extension(filename) == "")
	{
		synfig::error(_("Importer::open(): Couldn't find extension"));
		return 0;
	}

	String ext(filename_extension(filename));
	if (ext.size()) ext = ext.substr(1); // skip initial '.'
	std::transform(ext.begin(),ext.end(),ext.begin(),&::tolower);


	if(!CairoImporter::book().count(ext))
	{
		synfig::error(_("Importer::open(): Unknown file type -- ")+ext);
		return 0;
	}

	try {
		CairoImporter::Handle importer;
		importer=CairoImporter::book()[ext](filename.c_str());
		(*__open_cairoimporters)[filename]=importer;
		return importer;
	}
	catch (String str)
	{
		synfig::error(str);
	}
	return 0;
}

CairoImporter::CairoImporter()//:
	//gamma_(2.2)
{
}


CairoImporter::~CairoImporter()
{
	// Remove ourselves from the open importer list
	map<String,CairoImporter::LooseHandle>::iterator iter;
	for(iter=__open_cairoimporters->begin();iter!=__open_cairoimporters->end();++iter)
		if(iter->second==this)
		{
			__open_cairoimporters->erase(iter);
		}
}
