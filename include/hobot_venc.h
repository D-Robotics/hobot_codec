// Copyright (c) 2021 Horizon Robotics.All Rights Reserved.
//
// The material in this file is confidential and contains trade secrets
// of Horizon Robotics Inc. This is proprietary information owned by
// Horizon Robotics Inc. No part of this work may be disclosed,
// reproduced, copied, transmitted, or used in any way for any purpose,
// without the express written permission of Horizon Robotics Inc.

#include <thread>
#include <functional>
#include <memory>

#include "include/hwcodec.h"

#ifndef INCLUDE_HOBOT_VENC_H_
#define INCLUDE_HOBOT_VENC_H_
class HobotVenc : public HWCodec {
 public:
  HobotVenc(int channel, const char *type);
  ~HobotVenc();
  virtual int InitCodec();
  virtual int UninitCodec();
  virtual int PutData(const uint8_t *pDataIn, int nLen, const struct timespec &time_stamp);
  virtual int GetFrame(TFrameData *pOutFrm);
  virtual int ReleaseFrame(TFrameData *pFrame);

  int SetFps(int VeChn, int InputFps, int OutputFps);
  virtual void SetCodecAttr(const char* tsName, float fVal);

 protected:
  virtual int child_start(int nPicWidth, int nPicHeight);
  virtual int child_stop();

 private:
  int Create(int nVW, int nVH, int bits);
  int init_venc();
  int chnAttr_init();
  int venc_setRcParam(int bitRate);
  int prepare_user_buf_2lane(void *buf, uint32_t size_y, uint32_t size_uv);
  VENC_CHN_ATTR_S m_oVencChnAttr;
  VIDEO_STREAM_S m_curGetStream;

  float m_fJpgQuality;
  float m_fEncQp;

  std::shared_ptr<std::thread> m_spThrdInit;
  // std::atomic<bool> stop_;
  int exec_init();

  // VENC_CHN m_oVeChn;
  int VencChnAttrInit(VENC_CHN_ATTR_S *pVencChnAttr, PAYLOAD_TYPE_E p_enType,
            int p_Width, int p_Height, PIXEL_FORMAT_E pixFmt);
  int venc_h264cbr(VENC_RC_ATTR_S *pstRcParam, int bits,
        int framerate, int intraperiod, int vbvbuf);
  int venc_h264vbr(VENC_RC_ATTR_S *pstRcParam, int intraperiod,
            int intraqp, int framerate);
  int venc_h264avbr(VENC_RC_ATTR_S *pstRcParam, int bits,
        int framerate, int intraperiod, int vbvbuf);
  int venc_h264fixqp(VENC_RC_ATTR_S *pstRcParam, int framerate,
            int intraperiod, int iqp, int pqp, int bqp);
  int venc_h264qpmap(VENC_RC_ATTR_S *pstRcParam, int framerate,
                int intraperiod);
  int venc_h265cbr(VENC_RC_ATTR_S *pstRcParam, int bits,
        int framerate, int intraperiod, int vbvbuf);
  int venc_h265vbr(VENC_RC_ATTR_S *pstRcParam, int intraperiod,
            int intraqp, int framerate);
  int venc_h265avbr(VENC_RC_ATTR_S *pstRcParam, int bits,
        int framerate, int intraperiod, int vbvbuf);
  int venc_h265fixqp(VENC_RC_ATTR_S *pstRcParam, int framerate,
            int intraperiod, int iqp, int pqp, int bqp);
  int venc_h265qpmap(VENC_RC_ATTR_S *pstRcParam, int framerate,
                int intraperiod);
  int venc_mjpgfixqp(VENC_RC_ATTR_S *pstRcParam, int framerate,
                int quality);
};

#endif  // INCLUDE_HOBOT_VENC_H_
