#include <stdint.h>

class IRBlaster {
    public:
        void sendPronto(const char *str);
        bool checkComponent(); //Verifies the IRBlaster component is functioning
        void blastCommonTVPowerCodes();

    private:
        const uint16_t microsecondsTMax = 0xFFFFU;
        const unsigned int bitsInHexadecimal = 4U;
        const unsigned int digitsInProntoNumber = 4U;
        const unsigned int numbersInPreamble = 4U;
        const unsigned int hexMask = 0xFU;
        const uint32_t referenceFrequency = 4145146UL;
        const uint32_t microsecondsInSeconds = 1000000UL;

        unsigned int toFrequencyKHz(uint16_t code) {
            return ((referenceFrequency / code) + 500) / 1000;
        }

        static bool componentChecked;
        static void IRReceived();
};
