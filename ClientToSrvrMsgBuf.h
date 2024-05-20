#pragma once

namespace ErrorLogger
{
    constexpr int captionBufSz{32};
    constexpr int clientSktNameBufSz{32};
    constexpr int errMsgBufSz{80};
    constexpr int errFileNameBufSz{80};
    constexpr int instructionsBufSz{256};

    struct ClientToSrvrMsgBuf
    {
        char caption[captionBufSz];
        char clientSktName[clientSktNameBufSz];
        char errMsg[errMsgBufSz];
        char errFileName[errFileNameBufSz];
        char instructions[instructionsBufSz];
    };

    enum class SrvrResponse : int
    {
        Ignore,
        DisablePopups,
        TerminateClient
    };
}
