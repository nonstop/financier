
#include "FinCompletion.h"
#include "FinDatabase.h"
#include <QStringList>

FinCompletion* FinCompletion::Instance()
{
    static FinCompletion *fc = NULL;
    if (!fc)
        fc = new FinCompletion;
    return fc;
}

FinCompletion::FinCompletion()
{
    FinDatabase::Instance()->loadCompletion(completionList);
}

const QStringList& FinCompletion::getCompletionList()
{
    return completionList;
}

void FinCompletion::reload()
{
    completionList.clear();
    FinDatabase::Instance()->loadCompletion(completionList);
}

