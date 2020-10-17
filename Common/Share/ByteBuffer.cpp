#include "ByteBuffer.h"

void ByteBuffer::bulidHexString(std::string &targetBuffer, const char *pInValue, int nInLen)
{
    // 	LPCSTR pOldTargetBuffer = pTargetBuffer;
    //
    // 	// 0x
    // 	pTargetBuffer = strmov(pTargetBuffer, "0x");
    //
    // 	pTargetBuffer += mysql_hex_string(pTargetBuffer, pInValue, nInLen);
    //
    // 	return pTargetBuffer - pOldTargetBuffer;

    targetBuffer.assign(pInValue, pInValue + nInLen);
    for (size_t i = 0; i < targetBuffer.size(); ++i)
    {
        uint8_t& value = (uint8_t&)targetBuffer[i];
        switch (value)
        {
            case 0:
                value = 255;
                break;
            case 10:
                value = 254;
                targetBuffer.insert(targetBuffer.begin() + (++i), 1);
                break;
            case 13:
                value = 254;
                targetBuffer.insert(targetBuffer.begin() + (++i), 2);
                break;
            case 34:
                value = 254;
                targetBuffer.insert(targetBuffer.begin() + (++i), 3);
                break;
            case 39:
                value = 254;
                targetBuffer.insert(targetBuffer.begin() + (++i), 4);
                break;
            case 92:
                value = 254;
                targetBuffer.insert(targetBuffer.begin() + (++i), 5);
                break;
            case 254:
                targetBuffer.insert(targetBuffer.begin() + (++i), 6);
                break;
            case 255:
                value = 254;
                targetBuffer.insert(targetBuffer.begin() + (++i), 7);
                break;
        }
    }
}

void ByteBuffer::queryHexString(std::string &targetBuffer, const char *pInValue, int nInLen)
{
    targetBuffer.assign(pInValue, pInValue + nInLen);
    for (size_t i = 0; i < targetBuffer.size(); ++i)
    {
        uint8_t& value = (uint8_t&)targetBuffer[i];
        if (value == 0)
        {
            targetBuffer.erase(targetBuffer.begin() + i, targetBuffer.end());
        }
        else
        {
            if (value == 255)
                value = 0;
            else if(value == 254)
            {
                if (i + 1 < targetBuffer.size())
                {
                    switch (targetBuffer[i+1])
                    {
                        case 1:
                            targetBuffer[i] = 10;
                            break;
                        case 2:
                            targetBuffer[i] = 13;
                            break;
                        case 3:
                            targetBuffer[i] = 34;
                            break;
                        case 4:
                            targetBuffer[i] = 39;
                            break;
                        case 5:
                            targetBuffer[i] = 92;
                            break;
                        case 6:
                            targetBuffer[i] = 254;
                            break;
                        case 7:
                            targetBuffer[i] = 255;
                            break;
                    }
                    targetBuffer.erase(targetBuffer.begin() + i + 1);
                }
                else
                {
                    targetBuffer.erase(targetBuffer.begin() + i, targetBuffer.end());
                }
            }
        }
    }
}

