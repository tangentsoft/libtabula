/***********************************************************************
 mfc.cpp - Declares the MFC application object.  Nothing interesting to
	see here.  Move along to mfc_dlg.cpp, folks.

 Copyright © 2007 by Educational Technology Resources, Inc.  Others 
 may also hold copyrights on code in this file.  See the CREDITS.md
 file in the top directory of the distribution for details.

 This file is part of libtabula.

 libtabula is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published
 by the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.

 libtabula is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with libtabula; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
 USA
***********************************************************************/

#pragma once

class CApp : public CWinApp
{
public:
	CApp() { }
	virtual BOOL InitInstance();
};
