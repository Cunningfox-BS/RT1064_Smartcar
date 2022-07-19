#include "Camera.hpp"

extern "C" {
#include "SEEKFREE_MT9V03X.h"
#include "SEEKFREE_MT9V03X_CSI.h"
}

//��Ҫ���õ�����ͷ������
int16 MT9V03X_CFG_CSI[CONFIG_FINISH][2] = {
    {AUTO_EXP,
     63},  //�Զ��ع�����      ��Χ1-63 0Ϊ�ر� ����Զ��ع⿪��
           // EXP_TIME�������õ����ݽ����Ϊ����ع�ʱ�䣬Ҳ�����Զ��ع�ʱ�������
           //һ������ǲ���Ҫ����������ܣ���Ϊ�������ع���һ�㶼�ȽϾ��ȣ�����������߷ǳ������ȵ�������Գ������ø�ֵ������ͼ���ȶ���
    {EXP_TIME, 300},  //�ع�ʱ��          ����ͷ�յ�����Զ����������ع�ʱ�䣬������ù���������Ϊ�������������ع�ֵ
    {FPS, 1000},  //ͼ��֡��          ����ͷ�յ�����Զ���������FPS���������������Ϊ������������FPS
    {SET_COL, Camera::W},  //ͼ��������        ��Χ1-752     K60�ɼ���������188
    {SET_ROW, Camera::H},  //ͼ��������        ��Χ1-480
    {LR_OFFSET, 0},        //ͼ������ƫ����    ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ188 376 752ʱ�޷�����ƫ��
                     //����ͷ��ƫ�����ݺ���Զ��������ƫ�ƣ�������������ü�����������ƫ��
    {UD_OFFSET, 0},  //ͼ������ƫ����    ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ120 240 480ʱ�޷�����ƫ��
                     //����ͷ��ƫ�����ݺ���Զ��������ƫ�ƣ�������������ü�����������ƫ��
    {GAIN, 32},      //ͼ������          ��Χ16-64     ����������ع�ʱ��̶�������¸ı�ͼ�������̶�

    {INIT, 0}  //����ͷ��ʼ��ʼ��
};

//������ͷ�ڲ���ȡ������������
int16 GET_CFG_CSI[CONFIG_FINISH - 1][2] = {
    {AUTO_EXP, 0},   //�Զ��ع�����
    {EXP_TIME, 0},   //�ع�ʱ��
    {FPS, 0},        //ͼ��֡��
    {SET_COL, 0},    //ͼ��������
    {SET_ROW, 0},    //ͼ��������
    {LR_OFFSET, 0},  //ͼ������ƫ����
    {UD_OFFSET, 0},  //ͼ������ƫ����
    {GAIN, 0},       //ͼ������
};

void Camera::init() { mt9v03x_csi_init(); }

uint8_t* Camera::snapshot() { return mt9v03x_csi_image_take(); }

void Camera::release() { mt9v03x_csi_image_release(); }

void Camera::read_config() { get_config(MT9V03X_CSI_COF_UART, GET_CFG_CSI); }

void Camera::write_config() { set_config(MT9V03X_CSI_COF_UART, MT9V03X_CFG_CSI); }

int16_t Camera::get_auto_exposure() { return GET_CFG_CSI[0][1]; }

void Camera::set_auto_exposure(int16_t value) { MT9V03X_CFG_CSI[0][1] = value; }

int16_t Camera::get_exposure_time() { return GET_CFG_CSI[1][1]; }

void Camera::set_exposure_time(int16_t value) { MT9V03X_CFG_CSI[1][1] = value; }

void Camera::set_exposure_time_fast(uint16_t value) { ::set_exposure_time(MT9V03X_CSI_COF_UART, value); }

int16_t Camera::get_fps() { return GET_CFG_CSI[2][1]; }

void Camera::set_fps(int16_t value) { MT9V03X_CFG_CSI[2][1] = value; }

int16_t Camera::get_gain() { return GET_CFG_CSI[7][1]; }

void Camera::set_gain(int16_t value) { MT9V03X_CFG_CSI[7][1] = value; }

Camera camera;
