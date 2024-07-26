// Copyright (c) 2024，D-Robotics.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <thread>
#include <functional>
#include <memory>

#include "include/hobot_codec_base.h"

#include "libmm/hb_media_codec.h" 
#include "libmm/hb_media_error.h" 
 
#ifndef INCLUDE_HOBOT_VENC_H_
#define INCLUDE_HOBOT_VENC_H_

class HobotVenc : public HobotCodecBase {
 public:
  HobotVenc();
  ~HobotVenc();
  int Init(const std::shared_ptr<HobotCodecParaBase>& sp_hobot_codec_para) override;
  int DeInit() override;
  int Start(int nPicWidth, int nPicHeight) override;
  int Stop() override;
  int Input(const uint8_t *pDataIn, int nPicWidth, int nPicHeight, int nLen,
            const struct timespec &time_stamp) override;
  int GetOutput(std::shared_ptr<OutputFrameDataType> pFrame) override;
  int ReleaseOutput(const std::shared_ptr<OutputFrameDataType>& pFrame) override;

 protected:
  int CheckParams(const std::shared_ptr<HobotCodecParaBase>& sp_hobot_codec_para) override;

 private:
  media_codec_id_t m_enPalType;
  CodecImgFormat frame_fmt_ = CodecImgFormat::FORMAT_INVALID;

  media_codec_context_t *context_ = nullptr;
  media_codec_buffer_t outputBuffer_; 

  // 对于编码，pixel为1
  const int byte_per_pixel_ = 1;

   float m_fJpgQuality = 50.0;
  
  // 当输入为yuv420和yuv422格式时
  // JPEG/MJPEG编码时，要求输入宽高都是16对齐
  // H264和H265编码时，要求宽是16字节对齐，高是8字节对齐
  int aline_w_ = 16;
  int aline_h_ = 8;

  // 真正的初始化，Start中获取到输入分辨率后进行
  int FormalInit();
  CodecImgFormat ConvertPalType(const media_codec_id_t& pal_type);

};

#endif  // INCLUDE_HOBOT_VENC_H_
