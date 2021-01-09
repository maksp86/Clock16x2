#include "statics.h"

char* Statics::ClientID = "";

char* Statics::GetID()
{
    if (strlen(Statics::ClientID) > 0)
        return Statics::ClientID;

    char* buf = new char[20];
    sprintf(buf, "ESPclock-%x", ESP.getChipId());

    ClientID = buf;
    return buf;
};

int Statics::Rfind(char* str, char needle, int endIndex, int startIndex = 0)
{
    if (str && str[endIndex] && startIndex >= 0)
        for (int i = endIndex; i >= startIndex; i--)
        {
            if (str[i] == needle)
            {
                return i;
            }
        }
    return endIndex;
}
