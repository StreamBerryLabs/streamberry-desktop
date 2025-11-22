#pragma once

#include <vector>
#include <cstdint>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

class Decoder {
public:
    Decoder();
    ~Decoder();

    bool init();
    std::vector<uint8_t> decode(const uint8_t* data, size_t size);

private:
    AVCodecContext* m_codecCtx;
    AVCodecParserContext* m_parser;
    const AVCodec* m_codec;
    AVFrame* m_frame;
    AVPacket* m_pkt;
    SwsContext* m_swsCtx;
};
