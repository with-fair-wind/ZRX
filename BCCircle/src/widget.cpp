#include "widget.h"
#include "./ui_form.h"
#include <BCCircle.h>

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Form)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, [this]
            { QMessageBox::information(this, "info", "hello world", QMessageBox::Ok); });

    setWindowIcon(QIcon(":/resource/images/safe.png"));

    connect(ui->pushButton_2, &QPushButton::clicked, this, &Widget::on_clicked);
}

void Widget::on_clicked()
{
#define V_4
    AcApDocument *pDoc = acDocManager->curDocument();
#ifdef V_1
    if (pDoc)
    {
        acDocManager->lockDocument(pDoc);
        if (acedAlert(_T("HELLO BOCHAO !")) == RTNORM)
        {
            acutPrintf(_T("\nAlert"));
        }
        acDocManager->unlockDocument(pDoc);
    }
#elif defined(V_2)
    if (pDoc)
    {
        acDocManager->lockDocument(pDoc);
        AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
        if (pDb != nullptr)
        {
            AcGeVector3d xdir = pDb->ucsxdir();
            AcGeVector3d ydir = pDb->ucsydir();
            AcGeVector3d zdir = xdir.crossProduct(ydir);

            AcDbBlockTableRecord *pBlkTabRec = nullptr;
            if (acdbOpenObject(pBlkTabRec, pDb->currentSpaceId(), AcDb::kForWrite) == Acad::eOk)
            {
                AcDbPolyline *pline = new AcDbPolyline();
                pline->setDatabaseDefaults(pDb);
                pline->setNormal(zdir);
                pline->setConstantWidth(2.0);
                pline->setColorIndex(3);

                pline->addVertexAt(0, AcGePoint2d::kOrigin);
                pline->addVertexAt(1, AcGePoint2d(100, 100));
                pline->addVertexAt(2, AcGePoint2d(200, 50));

                AcDbObjectId entId = AcDbObjectId::kNull;
                if (pBlkTabRec->appendAcDbEntity(entId, pline) == Acad::eOk)
                    pline->close();
                else
                {
                    delete pline;
                    pline = nullptr;
                }
                pBlkTabRec->close();
            }
        }
        acDocManager->unlockDocument(pDoc);
    }
#elif defined(V_3)
    if (pDoc)
    {
        acDocManager->lockDocument(pDoc);
        test_mycircle();
        acDocManager->unlockDocument(pDoc);
    }
#elif defined(V_4)
    acDocManagerPtr()->sendStringToExecute(acDocManagerPtr()->curDocument(), _T("drawBCCircle "), Adesk::kFalse, Adesk::kTrue);
    // CWnd::FromHandle(acedGetAcadFrame()->m_hWnd)->SetActiveWindow();
    // CWnd::FromHandle(acedGetAcadFrame()->m_hWnd)->SetFocus();
#endif
}

Widget::~Widget()
{
    delete ui;
}
