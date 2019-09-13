/*
*	Author       biwei
*	Date         2016/10/19
*	Copyright    Hangzhou, Netease Inc.
*	Brief
*   Notice
*/

#ifndef _NELP_TYPE_H_
#define _NELP_TYPE_H_

#include "nelp_define.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \file */

typedef  NELP_MASK_HANDLE_TYPE(_HNLPSERVICE)  _HNLPSERVICE;

#define NELP_MSG_ERROR                       100   //!< ���ŷ�������
#define NELP_MSG_PREPARED                    200   //!< ������Ԥ�������
#define NELP_MSG_COMPLETED                   300   //!< �������
#define NELP_MSG_VIDEO_SIZE_CHANGED          400   //!< ��Ƶ�ֱ��ʷ����仯
#define NELP_MSG_VIDEO_RENDERING_START       402   //!< ��һ֡��Ƶ��ʾ
#define NELP_MSG_AUDIO_RENDERING_START       403   //!< ��һ֡��Ƶ����
#define NELP_MSG_BUFFERING_START             500   //!< ��ʼ����
#define NELP_MSG_BUFFERING_END               501   //!< �������
#define NELP_MSG_SEEK_COMPLETED              600   //!< seek���
#define NELP_MSG_PLAYBACK_STATE_CHANGED      700   //!< ������״̬�����仯
#define NELP_MSG_VIDEO_PARSE_ERROR           800   //!< ��Ƶ������ʧ��
#define NELP_MSG_AUDIO_DEVICE_OPEN_FAILED    900   //!< ������ʧ��


/**
 * @���Ż���ģʽ
 */
typedef enum enum_NELP_BUFFER_STRATEGY{
	EN_NELP_TOP_SPEED,  //!< ����ֱ������ģʽ������������Ƶֱ������ʱ��С�������ʱ���п���
	EN_NELP_LOW_DELAY,  //!< ����ֱ������ʱģʽ������������Ƶֱ��,��ʱ��С�����粻��ʱż���Ῠ��
	EN_NELP_FLUENT,     //!< ����ֱ������ģʽ������������Ƶֱ������ʱ�ȵ���ʱģʽ�Դ�һЩ�����������ԽϺ�
	EN_NELP_ANTI_JITTER //!< ����㲥������ģʽ����������Ƶ�㲥
} EN_NELP_BUFFER_STRATEGY;


/**
 * @brief ��������ʼ������
 */
typedef struct struct_NELP_PARAM
{
	char                    *paPlayUrl;        //!< ���ŵ�ַ
	EN_NELP_BUFFER_STRATEGY  enBufferStrategy; //!< ����ģʽ
} ST_NELP_PARAM;


/**
 * @brief ��Ϣ�ṹ��
 */
typedef struct struct_NELP_MESSAGE
{
	int iWhat;  //!< ��Ϣ����
	int iArg1;  //!< ����1
	int iArg2;  //!< ����2
} ST_NELP_MESSAGE;


/**
* @brief RECT�ṹ��
*/
typedef struct struct_NELP_RECT
{
	int iLeft;    //!< ���Ͻ�x����
	int iTop;     //!< ���Ͻ�y����
	int iWidth;   //!< ���
	int iHeight;  //!< �߶�
} ST_NELP_RECT;


/**
* @brief �ص�����Ƶ���ݽṹ
*/
typedef struct struct_NELP_AUDIO_FRAME
{
	int                   iChannels;      //!< ͨ����
	int                   iSamplerate;     //!< ������
	int                   iSize;             //!<����
	unsigned char        *puaUsrData;  //!<��Ƶ����
} ST_NELP_AUDIO_FRAME;

/**
 * @brief �ص�����Ƶ���ݸ�ʽ
 */
typedef enum enum_NELP_MEDIA_FORMAT
{
	EN_YUV420,  //!< YUV420��ʽ
	EN_ARGB8888 //!< ARGB8888��ʽ
} EN_NELP_MEDIA_FORMAT;


/**
 * @brief �ص�����Ƶ���ݽṹ
 */
typedef struct struct_NELP_FRAME
{
	int                   iWidth;      //!< ��Ƶ���
	int                   iHeight;     //!< ��Ƶ�߶�
	unsigned char        *puaUsrData;  //!< ��Ƶ����
	EN_NELP_MEDIA_FORMAT  enMFormat;   //!< ��Ƶ��ʽ
} ST_NELP_FRAME;


/**
 * @brief ��ͼ��ͼƬ���ݸ�ʽ
 */
typedef enum enum_NELP_PICTURE_FORMAT
{
	EN_PIC_RGB888,   //!< RGB888��ʽ
	EN_PIC_ARGB8888  //!< ARGB8888��ʽ
} EN_NELP_PICTURE_FORMAT;


/**
 * @brief ��ͼ��ͼƬ���ݽṹ
 */
typedef struct struct_NELP_PICTURE
{
	int                     iWidth;     //!< ͼƬ���
	int                     iHeight;    //!< ͼƬ�߶�
	unsigned char          *puaUsrData; //!< ͼƬ����
	EN_NELP_PICTURE_FORMAT  enPFormat;  //!< ͼƬ��ʽ
} ST_NELP_PICTURE;

/**
 * @brief ����״̬
 */
typedef enum enum_NELP_PLAYBACK_STATE
{
	EN_NELP_GET_PLAYBACK_STATE_FAILED = -1,   //!< ��ȡʧ��
	EN_NELP_PLAYBACK_STATE_STOPPED,           //!< ֹͣ״̬
	EN_NELP_PLAYBACK_STATE_PLAYING,           //!< ����״̬
	EN_NELP_PLAYBACK_STATE_PAUSED,            //!< ��ͣ״̬
	EN_NELP_PLAYBACK_STATE_SEEKING            //!< seek״̬
} EN_NELP_PLAYBACK_STATE;


/**
 *  @brief ���Ź����е���Ϣ�ص�
 *
 *  @param frame: �ص�����Ƶ��Ϣ
 *  @param msg: ��Ϣ��
 *
 *  @return ��
 */
typedef void(*PFN_NELP_MESSAGE_CB)(_HNLPSERVICE hNLPService, ST_NELP_MESSAGE msg);

/**
 *  @brief ��Ƶ���ݵĻص�
 *
 *  @param frame: �ص�����Ƶ֡�ṹ��ָ�룬��ΪNULL��ʾ���ݸ�ʽת��ʧ��
 *
 *  @return ��
 */
typedef void(*PFN_NELP_VIDEO_FRAME_CB)(_HNLPSERVICE hNLPService, ST_NELP_FRAME *frame);

/**
*  @brief ��Ƶ���ݵĻص�
*
*  @param frame: �ص�����Ƶ��Ϣ
*  @param frame: �ص�����Ƶ��Ϣ
*
*  @return ��
*/
typedef void(*PFN_NELP_AUDIO_FRAME_CB)(_HNLPSERVICE hNLPService, ST_NELP_AUDIO_FRAME *frame);

/**
 *  @brief ��Դ�ͷŽ����Ļص�
 */
typedef void(*PFN_NELP_RESOURCE_RELEASE_SUCCESS_CB)(_HNLPSERVICE hNLPService);


#ifdef __cplusplus
}
#endif

#endif//_NE_LIVEPLAYER_H_
