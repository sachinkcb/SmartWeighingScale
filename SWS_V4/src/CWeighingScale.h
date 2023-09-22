#ifndef CWeighingScale_H
#define CWeighingScale_H

#include "CDeviceConfig.h"
#include "CMeal.h"
#include "BSP.h"

#define MAX_FARME_SIZE 100

#define UPDATE_FRAME 5
#define REQUSET_FRAME 4
#define OK 0
#define WRONG_FRAME 0
const char OK_FRAME[] = "$O#";
const char ERROR_FRAME[] = "$E#";

class Cframe
{
    public:
        char m_cMode;
        uint8_t m_u8Operation;
        uint8_t m_u8pondNum;
        float m_fWeight;
        uint8_t m_u8SampleNum;
};

class CWeighingScale
{
    private:
        bool isValidFrame;
        bool m_bFrameReady;
        char m_ReceivedFrame[MAX_FARME_SIZE];
        uint8_t rpos;
        bool m_frameStared;
        Cframe m_oFrame;

    public:
    CMeal m_oMeal;
    /* Construct */
    CWeighingScale();
    /* Destruct */
    ~CWeighingScale();
    uint8_t decode(char c);
    uint8_t ParseCommnads(void);
    uint8_t ProcessFeedData(void);
    uint8_t ProcessSampleData(void);
    uint8_t ProcessChecktryData(void);
    uint8_t SendFrame(const char *sendFrame,int repeatCnt = 0);

};

#endif