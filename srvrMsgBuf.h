#pragma once

namespace ErrorLogger
{
    constexpr int maxCaptionSz{64};
    constexpr int maxClientSktNameLen{64};
    constexpr int maxErrMsgBufSz{256};

    struct SrvrMsgBuf
    {
        char caption[maxCaptionSz];
        char clientSktName[maxClientSktNameLen];
        char errMsgBuf[maxErrMsgBufSz];
    };
}
