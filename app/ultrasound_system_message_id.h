#pragma once

/* Ultrasound message format
{
  "message_id" : "",
  "message" : { }    // Defined by specific message id
}
*/

/* The report is confirmed message format
"message_id" : kUltrasoundMessageIdReportConfirmed
"message" : 
{
  "superior_id" : "xxx",
  "superior_name" : "xxx",
  "uid" : "20181204104921291",
  "content" = "����",
  "conclusion" = "����",
  "confirm_status" : 3,
  "confirm_msg" : "������,ͨ��"
}
*/
static const char *kUltrasoundMessageIdReportConfirmed = 
	"ultrasound_message_id_report_confirmed";