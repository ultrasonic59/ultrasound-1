#ifndef CORE_SERVICE_REPORT_TEMPLATE_H_
#define CORE_SERVICE_REPORT_TEMPLATE_H_
#pragma once

#include <list>
#include <string>

#include "json.h"

namespace core {

class ReportTemplate
{
public:
    bool LoadFromJson(const Json::Value& value);
    void SaveToJson(Json::Value& value);

    long Hospital_id() const { return hospital_id; }
    void Hospital_id(long val) { hospital_id = val; }

    std::string Doctor_id() const { return doctor_id; }
    void Doctor_id(std::string val) { doctor_id = val; }

    std::string Disease_description() const { return disease_description; }
    void Disease_description(std::string val) { disease_description = val; }

    std::string Disease_type() const { return disease_type; }
    void Disease_type(std::string val) { disease_type = val; }

    std::string Disease_name() const { return disease_name; }
    void Disease_name(std::string val) { disease_name = val; }

    std::string Doctor() const { return doctor; }
    void Doctor(std::string val) { doctor = val; }

    std::string Disease_conclusion() const { return disease_conclusion; }
    void Disease_conclusion(std::string val) { disease_conclusion = val; }

    std::string Hospital() const { return hospital; }
    void Hospital(std::string val) { hospital = val; }

    std::string Id() const { return id; }
    void Id(std::string val) { id = val; }

    std::string Create_time() const { return create_time; }
    void Create_time(std::string val) { create_time = val; }
private:
    long hospital_id;
    std::string doctor_id;
    std::string disease_description;
    std::string disease_type;
    std::string disease_name;
    std::string doctor;
    std::string disease_conclusion;
    std::string hospital;
    std::string id;
    std::string create_time;

};
}

#endif//CORE_SERVICE_REPORT_TEMPLATE_H_
