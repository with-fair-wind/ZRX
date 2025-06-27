#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include <SDKDDKVer.h>

#define _AFX_ALL_WARNINGS

#include <afxwin.h>
#include <afxext.h>
#include <afxdisp.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif

// cad
#include <rxobject.h>
#include <rxregsvc.h>
#include <rxdlinkr.h>
#include <rxditer.h>

#include <aced.h>
#include <actrans.h>
#include <adslib.h>
#include <acestext.h>
#include <adscodes.h>
#include <acedads.h>
#include <acedCmdNF.h>
#include <acdocman.h>
#include <AcString.h>

#include <dbmain.h>
#include <dbents.h>
#include <dbcurve.h>
#include <dbregion.h>
#include <dbdict.h>
#include <dbsymtb.h>
#include <dbapserv.h>
#include <dbxutil.h>
#include <dbjig.h>
#include <dbproxy.h>
#include <dbsol3d.h>
#include <dbidar.h>
#include <dbgroup.h>
#include <dbxrecrd.h>

#include <geassign.h>
#include <gegbl.h>
#include <gegblabb.h>
#include <gept2dar.h>
#include <gemat3d.h>

#include <migrtion.h>

#include <ol_errno.h>

#include <dbdynblk.h>
#include <dbobjptr.h>

#include <rxmfcapi.h>
// #include <aduiPaletteSet.h>

// standard
#include <tchar.h>
#include <map>
#include <utility>

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif