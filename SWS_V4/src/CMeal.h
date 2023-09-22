#ifndef CMeal_H
#define CMeal_H

#include <stdint.h>
#include <FILESYSTEM.h>
#define TOTAL_PONDS 10
#define FNAME_MEALSETTING    "/mealSetting.txt"

/*Class Pond data to hold pond meal data*/
class CPond
{
    public:   
    char m_cName[10];
    float m_fFeedWeight;
    float m_fFeedWeightActaul;
    uint8_t m_u8FeedVerified;
    uint8_t m_u8SamplesVerified;
    uint8_t m_u8NumofSamples;
    float m_u8checkTrayLeftover;
    float m_fSampleWtArray[10];
};

/*Meal Data*/
class CMeal
{
    private:

    public:
        /* Construct */
        CMeal();
        /* Destruct */
        ~CMeal();
        uint8_t m_u8MealNum;
        uint8_t m_u8SampleRate;
        CPond m_oPondList[TOTAL_PONDS]; 
        CPond m_oMealSettingList[TOTAL_PONDS]; 
        uint32_t m_dMealSettingVer;
        int loadSetting(FILESYSTEM* filesystem);
        int readSetting(FILESYSTEM* filesystem,char *SettingData);
        int writeSetting(FILESYSTEM* filesystem);
        void clearPondMealData(void);
};

#endif