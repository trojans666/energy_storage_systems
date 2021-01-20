#ifndef STARTER_H
#define STARTER_H

#include <QObject>
#include <QTranslator>
#include <QString>

#include "subui.h"

/* 翻译文件 */
class Translator : public QTranslator
{
public:
    Translator();
    bool isEmpty() const;
    QString translate( const char *context, const char *sourceText, const char *comment = 0 ) const;
};

class WinControl : public QObject
{
    Q_OBJECT
public:
    WinControl();
    bool callQTModule(const string &nm);
};

#endif // STARTER_H
