#pragma once

#include <BCLineDimensionPE.h>

class BCAssocWatcher : public AcEditorReactor
{
public:
    virtual void commandWillStart(const ACHAR *cmdStr) override;
    virtual void commandEnded(const ACHAR *cmdStr) override;

public:
    BCLineDimensionPE m_bcLineDimensionPE;
};