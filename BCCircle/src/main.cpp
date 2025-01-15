#include "BCCircle.h"
#include "widget.h"

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

QApplication *g_app = nullptr;

namespace QtInit
{
	inline int &getArgc()
	{
		static int ans = 1;
		return ans;
	}

	inline char **getArgv()
	{
		static char *argv[] = {R"(D:\soft\ZWCAD 2025\ZwCAD.exe)"};
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
		acedRegCmds->addCommand(_T("HELLOWORLD_COMMANDS"),
								oss.str().c_str(),
								oss.str().c_str(),
								ACRX_CMD_TRANSPARENT,
								cmdFuncVec[i]);
	}

	// 注册自定义实体
	BCCircle::rxInit();
	MyObject::rxInit();

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

	// 反注册自定义实体
	deleteAcRxClass(BCCircle::desc());
	deleteAcRxClass(MyObject::desc());

	// 反注册命令
	acedRegCmds->removeGroup(_T("HELLOWORLD_COMMANDS"));
}

void test0()
{
	test_mycircle();
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
