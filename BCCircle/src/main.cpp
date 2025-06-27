#include "BCCircle.h"
#include "widget.h"
#include "BCDimension.h"
#include "BCEntityReactor.h"
#include "BCInputContextReactor.h"
#include "BCPersistentReactor.h"
#include <chapter_13/persreactor.h>
#include <chapter_13/dbreactor.h>
#include <chapter_8/customClass.h>
#include <dynblockReference.h>
#include <BCAssocWatcher.h>
#include <BCDimensionAssoc.h>

#include <QApplication>
#include <QStyleFactory>

#include <sstream>
#include <vector>

void initApp();
void unloadApp();

void test0();
void test1();
void test2();
void test3();
void test4();
void test5();
void test6();

BCAssocWatcher *g_pBCAssocWatcher = nullptr;

QApplication *g_app = nullptr;

namespace QtInit
{
	inline int &getArgc()
	{
		// static int ans = 1;
		static int ans = 0;
		return ans;
	}

	inline char **getArgv()
	{
		// static char *argv[] = {R"(D:\soft\ZWCAD 2025\ZwCAD.exe)"};
		static char *argv[] = {""};
		return argv;
	}
}

void initApp()
{
	if (!qApp)
		g_app = new QApplication(QtInit::getArgc(), QtInit::getArgv());
	QApplication::setStyle(QStyleFactory::create("Fusion"));

	// register a command with the CAD command mechanism
	using funcPtr = void (*)(void);
	std::vector<funcPtr> cmdFuncVec{test0, test1, test2, test3, test4, test5, test6};

	for (std::size_t i = 0; i < cmdFuncVec.size(); ++i)
	{
		std::wostringstream oss;
		oss << _T("test") << i;
		acedRegCmds->addCommand(_T("BC_COMMANDS"),
								oss.str().c_str(),
								oss.str().c_str(),
								ACRX_CMD_TRANSPARENT,
								cmdFuncVec[i]);
	}
	acedRegCmds->addCommand(_T("BC_COMMANDS"), _T("drawBCCircle"), _T("drawBCCircle"), ACRX_CMD_TRANSPARENT, test_mycircle);
	acedRegCmds->addCommand(_T("BC_COMMANDS"), _T("addEntReactor"), _T("addEntReactor"), ACRX_CMD_TRANSPARENT, addEntityReactor);
	acedRegCmds->addCommand(_T("BC_COMMANDS"), _T("removeEntReactor"), _T("removeEntReactor"), ACRX_CMD_TRANSPARENT, removeEntityReactor);
	acedRegCmds->addCommand(_T("BC_COMMANDS"), _T("clearEntReactor"), _T("clearEntReactor"), ACRX_CMD_TRANSPARENT, clearEntityReactor);

	acedRegCmds->addCommand(_T("BC_COMMANDS"), _T("addInputContextReactor"), _T("addInputContextReactor"), ACRX_CMD_MODAL, addInputContextReactor);
	acedRegCmds->addCommand(_T("BC_COMMANDS"), _T("removeInputContextReactor"), _T("removeInputContextReactor"), ACRX_CMD_MODAL, removeInputContextReactor);

	acedRegCmds->addCommand(_T("BC_COMMANDS"),
							_T("PersistentReactor"),
							_T("PersistentReactor"),
							ACRX_CMD_MODAL,
							cmd_PersistentReactor);

	acedRegCmds->addCommand(_T("BC_COMMANDS"),
							_T("WATCHDB"),
							_T("WATCHDB"),
							ACRX_CMD_MODAL,
							watchDb);

	acedRegCmds->addCommand(_T("BC_COMMANDS"),
							_T("CLEARREAC"),
							_T("CLEARREAC"),
							ACRX_CMD_MODAL,
							clearReactors);

	acedRegCmds->addCommand(_T("BC_COMMANDS"),
							_T("ASSOCCIRS"),
							_T("ASSOCCIRS"),
							ACRX_CMD_MODAL,
							assocCircles);

	acedRegCmds->addCommand(_T("BC_COMMANDS"),
							_T("CREATETREE"),
							_T("CREATETREE"),
							ACRX_CMD_MODAL,
							createObjs);
	acedRegCmds->addCommand(_T("BC_COMMANDS"),
							_T("ERASETREE"),
							_T("ERASETREE"),
							ACRX_CMD_MODAL,
							eraseTree);
	acedRegCmds->addCommand(_T("BC_COMMANDS"),
							_T("LISTTREE"),
							_T("LISTTREE"),
							ACRX_CMD_MODAL,
							listTree);
	acedRegCmds->addCommand(_T("BC_COMMANDS"),
							_T("DYNBLKREFTEST"),
							_T("DYNBLKREFTEST"),
							ACRX_CMD_MODAL,
							test_dynamicBlock);
	acedRegCmds->addCommand(_T("BC_COMMANDS"),
							_T("TESTDIMENSION"),
							_T("TESTDIMENSION"),
							ACRX_CMD_MODAL,
							test_dimension);
	acedRegCmds->addCommand(_T("BC_COMMANDS"),
							_T("TESTCAL"),
							_T("TESTCAL"),
							ACRX_CMD_MODAL,
							test_modify_distance);

	// 注册自定义实体
	BCCircle::rxInit();
	MyObject::rxInit();
	DimensionPersistentReactor::rxInit();
	CPersistentReactor::rxInit();
	CustomClass::rxInit();
	BCDimensionAssoc::rxInit();
	BCLineDimensionPE::rxInit();

	g_pBCAssocWatcher = new BCAssocWatcher;
	if (g_pBCAssocWatcher)
		acedEditor->addReactor(g_pBCAssocWatcher);

	BCDimensionAssoc::desc()->addX(BCLineDimensionPE::desc(), &g_pBCAssocWatcher->m_bcLineDimensionPE);

	acrxBuildClassHierarchy();
}

void unloadApp()
{
	foreach (QWidget *widget, QApplication::topLevelWidgets())
		widget->close();
	if (g_app)
	{
		delete g_app;
		g_app = nullptr;
	}

	bc_AddObjOverruleCanNotCopy();

	removeInputContextReactor();
	clearEntityReactor();

	BCDimensionAssoc::desc()->delX(BCLineDimensionPE::desc());

	if (g_pBCAssocWatcher)
	{
		acedEditor->removeReactor(g_pBCAssocWatcher);
		delete g_pBCAssocWatcher;
		g_pBCAssocWatcher = nullptr;
	}

	// 反注册自定义实体
	deleteAcRxClass(BCCircle::desc());
	deleteAcRxClass(MyObject::desc());
	deleteAcRxClass(DimensionPersistentReactor::desc());
	deleteAcRxClass(CPersistentReactor::desc());
	deleteAcRxClass(CustomClass::desc());
	deleteAcRxClass(BCDimensionAssoc::desc());
	deleteAcRxClass(BCLineDimensionPE::desc());

	// 反注册命令
	acedRegCmds->removeGroup(_T("BC_COMMANDS"));
}

void test0()
{
	// test_dim();
	// test_mycircle();
}

void test1()
{
	test_buildlink();
}

void test2()
{
	test_gsMarker();
}

void test3()
{
	bc_AddObjOverruleCanCopy();
}

void test4()
{
	bc_RemoveObjOverrule();
}

void test5()
{
	getRegApp();
}

void test6()
{
	Widget *widget = new Widget;
	widget->setAttribute(Qt::WA_DeleteOnClose);
	widget->setWindowTitle("Qt Widget in ARX");
	widget->show();
}

extern "C" AcRx::AppRetCode
zcrxEntryPoint(AcRx::AppMsgCode msg, void *pkt)
{
	switch (msg)
	{
	case AcRx::kInitAppMsg:
		acrxDynamicLinker->unlockApplication(pkt);
		acrxRegisterAppMDIAware(pkt);
		initApp();
		break;
	case AcRx::kUnloadAppMsg:
		unloadApp();
		break;
	default:
		break;
	}

	return AcRx::kRetOK;
}
