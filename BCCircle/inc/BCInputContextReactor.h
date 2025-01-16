#pragma once

class BCInputContextRec : public AcEdInputContextReactor
{
protected:
    virtual void beginGetString(const ACHAR *promptString, int initGetFlags) override;
    virtual void endGetString(Acad::PromptStatus returnStatus, ACHAR *pString) override;
};

extern BCInputContextRec *g_pInputContextRec;

void addInputContextReactor();
void removeInputContextReactor();