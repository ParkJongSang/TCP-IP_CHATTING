#include "ReadNWrite.h"

ssize_t writen(int sockFd, void *block, size_t fixedLength)
{
    size_t leftBytes = fixedLength;
    ssize_t writenBytes = 0;
    void *readingPointer = block;

    while (leftBytes > 0)
    {
        if ((writenBytes = write(sockFd, readingPointer, leftBytes)) <= 0)
        {
            if (errno == EINTR)
            {
                writenBytes = 0;
            }
            else if (errno == EPIPE)
            {
                perror("[WRITE]");
                return -1;
            }
            else
            {
                perror("[WRITE]");
                return -1;
            }
        }
        else
        {
            leftBytes -= writenBytes;
            readingPointer += writenBytes;
        }
    }
    return fixedLength - leftBytes;
}

ssize_t writePacket(int sockFd, void *packet, size_t length)
{
    ssize_t writeBytes;

    writeBytes = writen(sockFd, packet, length);

    if (writeBytes != length)
    {
        return writeBytes < 0 ? -1 : 0;
    }
    return writeBytes;
}

ssize_t readn(int sockFd, void *block, size_t fixedLength)
{
    size_t leftBytes = fixedLength;
    ssize_t readBytes = 0;
    void *writingPointer = block;
    while (leftBytes > 0)
    {
        if ((readBytes = read(sockFd, writingPointer, leftBytes)) < 0)
        {
            if (errno == EINTR)
            {
                readBytes = 0;
            }
            else if (errno == EAGAIN)
            {
                continue;
            }
            else if (errno == EPIPE)
            {
                perror("[PIPE_ERROR] : ");
                return -1;
            }
            else
            {
                continue;
            }
        }
        else if (readBytes == 0)
        {
            break;
        }
        else
        {
            leftBytes -= readBytes;
            writingPointer += readBytes;
        }
    }

    return fixedLength - leftBytes;
}

ssize_t readPacket(int sockFd, void *packet, size_t length)
{
    ssize_t readBytes;

    readBytes = readn(sockFd, packet, length);

    if (readBytes != length)
    {
        return readBytes < 0 ? -1 : 0;
    }

    return readBytes;
}
