#include "decoder.h"
#include <iostream>

Decoder::Decoder()
    : m_codecCtx(nullptr)
    , m_parser(nullptr)
    , m_codec(nullptr)
    , m_frame(nullptr)
    , m_pkt(nullptr)
    , m_swsCtx(nullptr)
{
}

Decoder::~Decoder()
{
    if (m_codecCtx) avcodec_free_context(&m_codecCtx);
    if (m_parser) av_parser_close(m_parser);
    if (m_frame) av_frame_free(&m_frame);
    if (m_pkt) av_packet_free(&m_pkt);
    if (m_swsCtx) sws_freeContext(m_swsCtx);
}

bool Decoder::init()
{
    m_codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!m_codec) {
        std::cerr << "Codec not found" << std::endl;
        return false;
    }

    m_parser = av_parser_init(m_codec->id);
    if (!m_parser) {
        std::cerr << "Parser not found" << std::endl;
        return false;
    }

    m_codecCtx = avcodec_alloc_context3(m_codec);
    if (!m_codecCtx) {
        std::cerr << "Could not allocate video codec context" << std::endl;
        return false;
    }

    if (avcodec_open2(m_codecCtx, m_codec, nullptr) < 0) {
        std::cerr << "Could not open codec" << std::endl;
        return false;
    }

    m_frame = av_frame_alloc();
    m_pkt = av_packet_alloc();

    return true;
}

std::vector<uint8_t> Decoder::decode(const uint8_t* data, size_t size)
{
    std::vector<uint8_t> decodedData;

    // Use parser to split into packets if needed, but since we get full frames/NALs from WS,
    // we might just feed it directly. However, the parser is safer.
    
    uint8_t* cur_ptr = const_cast<uint8_t*>(data);
    int cur_size = size;

    while (cur_size > 0) {
        int len = av_parser_parse2(m_parser, m_codecCtx, &m_pkt->data, &m_pkt->size,
                                   cur_ptr, cur_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

        cur_ptr += len;
        cur_size -= len;

        if (m_pkt->size) {
            int ret = avcodec_send_packet(m_codecCtx, m_pkt);
            if (ret < 0) {
                std::cerr << "Error sending packet for decoding" << std::endl;
                continue;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(m_codecCtx, m_frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    break;
                else if (ret < 0) {
                    std::cerr << "Error during decoding" << std::endl;
                    break;
                }

                // Convert to YUYV422 (common for webcams) or keep as YUV420P
                // V4L2 loopback often expects YUYV or UYVY. Let's try to convert to YUYV422.
                
                if (!m_swsCtx) {
                    m_swsCtx = sws_getContext(m_frame->width, m_frame->height, m_codecCtx->pix_fmt,
                                            m_frame->width, m_frame->height, AV_PIX_FMT_YUYV422,
                                            SWS_BILINEAR, nullptr, nullptr, nullptr);
                }

                int destSize = m_frame->width * m_frame->height * 2; // YUYV is 2 bytes per pixel
                decodedData.resize(destSize);
                
                uint8_t* dest[4] = { decodedData.data(), nullptr, nullptr, nullptr };
                int destLinesize[4] = { m_frame->width * 2, 0, 0, 0 };

                sws_scale(m_swsCtx, m_frame->data, m_frame->linesize, 0, m_frame->height, dest, destLinesize);
            }
        }
    }

    return decodedData;
}
