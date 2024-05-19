#pragma once

namespace ErrorLogger
{
    constexpr int captionBufSz{64};
    constexpr int clientSktNameBufSz{64};
    constexpr int errMsgBufSz{256};

    struct ClientToSrvrMsgBuf
    {
        char caption[captionBufSz];
        char clientSktName[clientSktNameBufSz];
        char errMsg[errMsgBufSz];
    };

    enum class SrvrResponse : int
    {
        Ignore,
        DisablePopups,
        TerminateClient
    };
}