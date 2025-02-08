#include <BCAssocWatcher.h>

void BCAssocWatcher::commandWillStart(const ACHAR *cmdStr)
{
    if (_tcsicmp(cmdStr, _T("u")) && _tcsicmp(cmdStr, _T("redo")) && _tcsicmp(cmdStr, _T("undo")))
    {
        m_bcLineDimensionPE.open();
    }
}

void BCAssocWatcher::commandEnded(const ACHAR *cmdStr)
{
    m_bcLineDimensionPE.close(true);
}