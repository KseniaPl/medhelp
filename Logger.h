#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

namespace logger
{
    static void logWrite(std::string text)
    {
        qDebug((QString::fromStdString(text)).toUtf8());
        //int f =  this->GetWindowTextLength();
        //this->SetSel(this->GetWindowTextLength(), this->GetWindowTextLength());
        //ReplaceSel(text);
    }

    static void logWriteError(std::string text)
    {
        qDebug("Logger ERROR: " +(QString::fromStdString(text)).toUtf8());
        //qDebug("!ERROR! logger " + text.toUtf8());
        //int f =  this->GetWindowTextLength();
        //this->SetSel(this->GetWindowTextLength(), this->GetWindowTextLength());
        //ReplaceSel(text);
    }
}

#endif //LOGGER_H
