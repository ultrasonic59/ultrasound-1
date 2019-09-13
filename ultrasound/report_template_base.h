#pragma once
#include <QString>
#include <QObject>

class QModelIndex;

class ReportTemplateBase : public QObject
{
    Q_OBJECT
public:
    virtual ~ReportTemplateBase() = default;
    virtual void Init() = 0;
};
