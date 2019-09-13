#include "report_template.h"

bool core::ReportTemplate::LoadFromJson(const Json::Value & value)
{
    hospital_id = value["hospital_id"].asInt();
    doctor_id = value["doctor_id"].asString();
    disease_description = value["disease_description"].asString();
    disease_type = value["disease_type"].asString();
    disease_name = value["disease_name"].asString();
    doctor = value["doctor"].asString();
    disease_conclusion = value["disease_conclusion"].asString();
    hospital = value["hospital"].asString();
    id = value["_id"].asString();
    create_time = value["_ts"].asString();
    return true;
}

void core::ReportTemplate::SaveToJson(Json::Value & value)
{
}
