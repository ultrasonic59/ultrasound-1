#ifndef CORE_SERVICE_TEMPLATE_EVENT_HANDLER_H_
#define CORE_SERVICE_TEMPLATE_EVENT_HANDLER_H_
#pragma once

#include "core_service/report_template.h"

namespace core {

class TemplateEventHandler
{
public:
    virtual ~TemplateEventHandler() {}

    virtual void OnGetTemplateSuccessed(const std::list<ReportTemplate>& list) = 0;
    virtual void OnGetTemplateFailed(int error_code, const std::string& cs) = 0;
    virtual void OnQueryTemplateSuccessed(const std::list<ReportTemplate>& list) = 0;
    virtual void OnQueryTemplateFailed(int error_code, const std::string& cs) = 0;
    virtual void OnCreateTemplateSuccessed() = 0;
    virtual void OnCreateTemplateFailed(int error_code, const std::string& message) = 0;
    virtual void OnUpdateTemplateSuccessed() = 0;
    virtual void OnUpdateTemplateFailed(int error_code, const std::string& message) = 0;
    virtual void OnDeleteTemplateSuccessed() = 0;
    virtual void OnDeleteTemplateFailed(int error_code, const std::string& message) = 0;
};

}

#endif//CORE_SERVICE_TEMPLATE_EVENT_HANDLER_H_