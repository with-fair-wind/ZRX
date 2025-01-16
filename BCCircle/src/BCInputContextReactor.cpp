#include "BCInputContextReactor.h"

BCInputContextRec *g_pInputContextRec = nullptr;

void BCInputContextRec::beginGetString(const ACHAR *promptString, int initGetFlags)
{
    acutPrintf(_T("\nbeginGetString"));
}

void BCInputContextRec::endGetString(Acad::PromptStatus returnStatus, ACHAR *pString)
{
    acutPrintf(_T("\nendGetString"));
}

void addInputContextReactor()
{
    if (!g_pInputContextRec)
    {
        g_pInputContextRec = new BCInputContextRec();
        acDocManager->curDocument()->inputPointManager()->addInputContextReactor(g_pInputContextRec);
    }
    else
        acDocManager->curDocument()->inputPointManager()->addInputContextReactor(g_pInputContextRec);
}

void removeInputContextReactor()
{
    if (g_pInputContextRec)
    {
        acDocManager->curDocument()->inputPointManager()->removeInputContextReactor(g_pInputContextRec);
    }
}