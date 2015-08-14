/***********************************************************************
 mfc.cpp - Skeletal main module of the libtabula MFC example program.
	Nothing interesting to see here.  Move along to mfc_dlg.cpp, folks.

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

#include "stdafx.h"
#include "mfc.h"
#include "mfc_dlg.h"

CApp gApplication;

BOOL CApp::InitInstance()
{
	// Set up Windows commomn controls
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize Winsock for libtabula communication, start GUI
	if (AfxSocketInit()) {
		CExampleDlg dlg;
		m_pMainWnd = &dlg;
		dlg.DoModal();
	}
	else {
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
	}

	return FALSE;
}
