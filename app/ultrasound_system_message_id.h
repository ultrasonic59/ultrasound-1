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
  "content" = "描述",
  "conclusion" = "结论",
  "confirm_status" : 3,
  "confirm_msg" : "审核意见,通过"
}
*/
static const char *kUltrasoundMessageIdReportConfirmed = 
	"ultrasound_message_id_report_confirmed";