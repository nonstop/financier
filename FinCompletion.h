#ifndef FINCOMPLETION_H_
#define FINCOMPLETION_H_

#include <QStringList>

class FinCompletion
{
public:
    static FinCompletion* Instance();
    
    void reload();
    const QStringList& getCompletionList();
    ~FinCompletion()
    {}
private:
    FinCompletion();
    QStringList completionList;
};

#endif /* FINCOMPLETION_H_ */

