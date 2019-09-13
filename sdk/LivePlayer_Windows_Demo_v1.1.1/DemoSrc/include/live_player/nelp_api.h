/*
*	Author       biwei
*	Date         2016/10/19
*	Copyright    Hangzhou, Netease Inc.
*	Brief
*   Notice
*/

#ifndef _NELP_API_H_
#define _NELP_API_H_

#include "nelp_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \file */

/**
* @brief ��ȡSDK�汾��
*
* @return ��
*/
EXPORTS_API const char* Nelp_GetSDKVersion();

/**
 * @brief ����������ʵ��
 * @param paLogpath:    ��־�ļ�·��(��: "D:\\log"; ������ΪNULL���Ĭ�ϱ��浽��ǰ�Ĺ���Ŀ¼��)
 * @param phNLPService: ������ʵ��������
 *
 * @return NELP_RET: NELP_OK �ɹ�; NELP_ERR ʧ��
 */
EXPORTS_API NELP_RET Nelp_Create(const char *paLogpath, NELP_OUT _HNLPSERVICE *phNLPService);

/**
 * @brief ��ʼ���������������ŵ�ַ������ģʽ��
 * @param hNLPService: ������ʵ��
 * @param pstParam:    ��ز���
 *
 * @return NELP_RET: NELP_OK �ɹ�; NELP_ERR ʧ��
 */
EXPORTS_API NELP_RET Nelp_InitParam(_HNLPSERVICE hNLPService, ST_NELP_PARAM *pstParam);

/**
* @brief �������Ѿ���ʼ������δ�رյ�����£�����ֹ֮ͣǰ�Ĳ��Ų��л����µ�url��ʼ���š�ע�⣺���ô˽ӿڳɹ�������Ҫ�ϲ����յ�NELP_MSG_PREPARED����������Nelp_Start��
* @param hNLPService: ������ʵ��
* @param pstNewParam: �µĲ��Ų���
*
* @return NELP_RET: NELP_OK �ɹ�; NELP_ERR ʧ��
*/
EXPORTS_API NELP_RET Nelp_SwitchContentUrl(_HNLPSERVICE hNLPService, ST_NELP_PARAM *pstNewParam);

/**
* @brief ����������ʱʱ�䣬��prepareToPlay֮ǰ����
* @param hNLPService: ������ʵ��
* @param timeout: ��ʱʱ�䣨��λ���룩
*
* @return ��
*/
EXPORTS_API void Nelp_SetPlaybackTimeout(_HNLPSERVICE hNLPService, int timeout);

/**
 * @brief ��ʼ�������������������������ȡ�ע�⣺Ϊ��ֹ��UI�̣߳��˽ӿ�Ϊ�첽�ӿڣ��յ�NELP_MSG_PREPARED��Ϣ�ű�ʾ��ʼ����ɡ�
 * @param hNLPService: ������ʵ��
 *
 * @return NELP_RET: NELP_OK �ɹ�; NELP_ERR ʧ��
 */
EXPORTS_API NELP_RET Nelp_PrepareToPlay(_HNLPSERVICE hNLPService);

/**
 * @brief ��ʼ���š�ע�⣺�˽ӿ��漰��Ƶ��Ⱦ������UI�̵߳��á����鲥������ʼ����ɣ��յ�NELP_MSG_PREPARED�����ٵ��ô˽ӿڣ�������Nelp_PrepareToPlay֮�����̵��ô˽ӿڣ�������ֱ����������ʼ����ɡ�
 * @param hNLPService: ������ʵ��
 *
 * @return NELP_RET: NELP_OK �ɹ�; NELP_ERR ʧ��
 */
EXPORTS_API NELP_RET Nelp_Start(_HNLPSERVICE hNLPService);

/**
 * @brief ��ͣ���ţ���Nelp_Resume���������ţ��������ڵ㲥��
 * @param hNLPService: ������ʵ��
 *
 * @return ��
 */
EXPORTS_API void Nelp_Pause(_HNLPSERVICE hNLPService);

/**
* @brief �������ţ��������ڵ㲥��
* @param hNLPService: ������ʵ��
*
* @return ��
*/
EXPORTS_API void Nelp_Resume(_HNLPSERVICE hNLPService);

/**
* @brief ֹͣ���ţ�Nelp_Start�������
* @param hNLPService: ������ʵ��
*
* @return ��
*/
EXPORTS_API void Nelp_Stop(_HNLPSERVICE hNLPService);

/**
 * @brief �رղ��������ͷ������Դ��Nelp_PrepareToPlay����������˽ӿ�ͬʱ������_HNLPSERVICEʵ��������������뽫����ÿա��˽ӿ���һ��ͬ���ӿڡ�
 * @param hNLPService: ������ʵ��
 *
 * @return ��
 */
EXPORTS_API void Nelp_Shutdown(_HNLPSERVICE hNLPService);

/**
* @brief ������ʾ���ھ���Լ���ʾ����SDK��ֱ���ڴ����ϻ�����Ƶ���棬���Ź�����Ҳ�ɶ�ε��ô˽ӿ������»�������ע���˽ӿ��漰��Ƶ��Ⱦ������UI�̵߳��á�
* @param hNLPService:       ������ʵ��
* @param hWnd:              ��ʾ���ھ��
* @param pstDisplayRect:    ��������
*
* @return NELP_RET: NELP_OK �ɹ�; NELP_ERR ʧ��
*/
EXPORTS_API NELP_RET Nelp_SetDisplay(_HNLPSERVICE hNLPService, void* hWnd, ST_NELP_RECT* pstDisplayRect);

/**
* @brief ��������
* @param hNLPService: ������ʵ��
* @param fVolume: ������С (��Χ: 0.0 ~ 1.0  0.0:����   1.0:���)
*
* @return ��
*/
EXPORTS_API void Nelp_SetVolume(_HNLPSERVICE hNLPService, float fVolume);

/**
* @brief ����/ȡ������
* @param hNLPService: ������ʵ��
* @param bIsMute: true ��������; false ȡ������
*
* @return ��
*/
EXPORTS_API void Nelp_SetMute(_HNLPSERVICE hNLPService, bool bIsMute);

/**
* @brief ��ȡ�������ĵ�ǰ״̬
* @param hNLPService: ������ʵ��
*
* @return EN_NELP_PLAYBACK_STATE: ������״̬   ����	EN_NELP_GET_PLAYBACK_STATE_FAILED ���ʾ��ȡʧ��
*/
EXPORTS_API EN_NELP_PLAYBACK_STATE Nelp_GetPlaybackState(_HNLPSERVICE hNLPService);

/**
 * @brief ��ȡ��ǰ����λ��
 * @param hNLPService: ������ʵ��
 *
 * @return ��ǰ����λ��(��λ: ��) < 0: ��ȡʧ��
 */
EXPORTS_API long long Nelp_GetCurrentPlaybackTime(_HNLPSERVICE hNLPService);

/**
 * @brief ��ȡ�ļ���ʱ�����������ڵ㲥��
 * @param hNLPService: ������ʵ��
 *
 * @return �ļ���ʱ��(��λ: ��)   < 0: ��ȡʧ��
 */
EXPORTS_API long long Nelp_GetDuration(_HNLPSERVICE hNLPService);

/**
 * @brief ��ȡ��ǰ�ɲ��ŵ�λ�ã����ѻ����λ��
 * @param hNLPService: ������ʵ��
 *
 * @return ��ǰ�ɲ���λ��(��λ: ��)   < 0: ��ȡʧ��
 */
EXPORTS_API long long Nelp_GetPlayableDuration(_HNLPSERVICE hNLPService);

/**
 * @brief ָ����ĳһʱ��㲥�ţ��������ڵ㲥��
 * @param hNLPService: ������ʵ��
 * @param llTime: ָ���Ĳ���ʱ���(��λ: �� s)
 *
 * @return NELP_RET: NELP_OK  �ɹ�    NELP_ERR  ʧ��
 */
EXPORTS_API NELP_RET Nelp_SeekTo(_HNLPSERVICE hNLPService, long long llTime);

/**
* @brief ��ȡ��ǰ��Ƶ�����ARGB��ʽ��ͼ���˽ӿڲ�֧�ֶ��̵߳���
* @param hNLPService: ������ʵ��
* @param ePFormat: ͼƬ��ʽ
*
* @return ST_NELP_PICTURE*: ��������ͼƬ��ԭʼ���ݺ���Ϣ
*/
EXPORTS_API ST_NELP_PICTURE* Nelp_GetSnapshot(_HNLPSERVICE hNLPService, EN_NELP_PICTURE_FORMAT ePFormat);

/**
 * @brief ע���ȡ��Ϣ�Ļص�, �û���Ҫʵ�ֻص�����cb�����������Ϣ
 * @param hNLPService:  ������ʵ��
 * @param pMessageCB:   ��ȡ��Ϣ�Ļص�

 * @return ��
 */
EXPORTS_API void Nelp_RegisterMessageCB(_HNLPSERVICE hNLPService, PFN_NELP_MESSAGE_CB pMessageCB);

/**
 * @brief ע���ȡ��Ƶ֡���ݵĻص����û���Ҫʵ�ֻص�����cb��������Ƶ֡
 * @param hNLPService:   ������ʵ��
 * @param pAudioFrameCB:  ��ȡ��Ƶ֡�Ļص�
 *
 * @return ��
 */
EXPORTS_API void Nelp_RegisterGetAudioFrameCB(_HNLPSERVICE hNLPService, PFN_NELP_AUDIO_FRAME_CB pAudioFrameCB);

/**
* @brief ע���ȡ��Ƶ֡���ݵĻص����û���Ҫʵ�ֻص�����cb��������Ƶ֡
* @param hNLPService:   ������ʵ��
* @param eMFormat:      �ص�����Ƶ��ʽ
* @param pVideoFrameCB:  ��ȡ��Ƶ֡�Ļص�
*
* @return ��
*/
EXPORTS_API void Nelp_RegisterGetVideoFrameCB(_HNLPSERVICE hNLPService, EN_NELP_MEDIA_FORMAT eMFormat, PFN_NELP_VIDEO_FRAME_CB pVideoFrameCB);

/**
* @brief ע���ȡ��Դ�ͷŽ�����Ϣ�Ļص�(��Դ�ͷ����첽��), �û���Ҫʵ�ֻص�����cb�����������Ϣ
* @param hNLPService:  ������ʵ��
* @param pReleaseCB:   ��ȡ��Դ�ͷŽ�����Ϣ�Ļص�
*
* @return ��
*/
EXPORTS_API void Nelp_RegisterResourceReleaseSuccessCB(_HNLPSERVICE hNLPService, PFN_NELP_RESOURCE_RELEASE_SUCCESS_CB pReleaseCB);

#ifdef __cplusplus
}
#endif

#endif
