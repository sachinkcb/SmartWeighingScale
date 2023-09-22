/* 
  PM_APP.cpp - Application file is used to read the registor values of energy meter ic , calculate the suggested/ required capbank value and also used for detection of 
               power source
            
  Dev: Infiplus Team 
  May 2021  
*/

#include "CMeal.h"
#include "stdio.h"
#include <ArduinoJson.h>

#define SERIAL_DEBUG
#ifdef SERIAL_DEBUG
#define debugPrint(...) Serial.print(__VA_ARGS__)
#define debugPrintln(...) Serial.println(__VA_ARGS__)
#define debugPrintf(...) Serial.printf(__VA_ARGS__)
#define debugPrintlnf(...) Serial.println(F(__VA_ARGS__))
#else
#define debugPrint(...)    //blank line
#define debugPrintln(...)  //blank line
#define debugPrintf(...)   //blank line
#define debugPrintlnf(...) //blank line
#endif

/***************************************************************************
*  Safe copy Function to avoid overflow 
****************************************************************************/
static void safeStrcpy(char * destination, const char * source, int sizeofDest)
{
  strncpy(destination,source,sizeofDest-1);
  destination[sizeofDest-1] = 0;
}

/* Construct */
CMeal::CMeal(){}
/* Destruct */
CMeal::~CMeal(){}
/************************************************************
 * Load meal Setting From File in local Veriables
*************************************************************/
int CMeal::loadSetting(FILESYSTEM* filesystem)
{
  char rdata[1024];
  int ret = 0;
  int Size = filesystem->getFileSize(FNAME_MEALSETTING);
  if (Size >= 1024)
  { 
    debugPrintln("Wrong Setting file size...");
    return 0;
  }
  /*Read File in data streM*/
  ret = filesystem->readFile(FNAME_MEALSETTING,rdata);
  if(ret > 0)
  {
    DynamicJsonDocument mealInfo(1024);    
    /*De serilaize the file json*/
    DeserializationError err = deserializeJson(mealInfo, rdata);
    if (err.code() == DeserializationError::Ok)
    {
      /*Read Meal Setting Version*/
      if(mealInfo.containsKey("mealSettingVer"))
      {
          m_dMealSettingVer = mealInfo["mealSettingVer"];
      }
      else
      {
          m_dMealSettingVer = 0;
      }
      /*Read Meal Number*/
      if(mealInfo.containsKey("mealNum"))
      {
          m_u8MealNum = mealInfo["mealNum"];
      }
      else
      {
          m_u8MealNum = 0;
      }
      /*Read sampleRate*/
      if(mealInfo.containsKey("sampleRate"))
      {
          m_u8SampleRate = mealInfo["sampleRate"];
      }
      else
      {
          m_u8SampleRate = 0;
      }
      /*Read Pond wise meal data*/
      for (uint8_t pnum = 0; pnum < TOTAL_PONDS; pnum++)
      {
        bool dataMissing = false;
        if(mealInfo["mealSettings"][pnum].containsKey("name"))
        {
          safeStrcpy(m_oMealSettingList[pnum].m_cName, (const char *)mealInfo["mealSettings"][pnum]["name"], 10);
        }
        else
        {
          dataMissing = true;
        }
        /*Read wt*/
        if(mealInfo["mealSettings"][pnum].containsKey("wt"))
        {
          m_oMealSettingList[pnum].m_fFeedWeight = mealInfo["mealSettings"][pnum]["wt"];
        }
        else
        {
          dataMissing = true;
        }
        /*Read Number of samples*/
        if(mealInfo["mealSettings"][pnum].containsKey("snum"))
        {
          m_oMealSettingList[pnum].m_u8NumofSamples = mealInfo["mealSettings"][pnum]["snum"];
        }
        else
        {
          dataMissing = true;
        }
        /* If data misssing set default*/
        if(dataMissing)        
        {
          safeStrcpy(m_oMealSettingList[pnum].m_cName, "P0", 10);
          m_oMealSettingList[pnum].m_fFeedWeight = 0;
          m_oMealSettingList[pnum].m_u8NumofSamples = 3;
        }
      }
    }
  }
  /*Clear last meal data*/
  clearPondMealData();
  /*Print Setting*/
  debugPrintln(m_dMealSettingVer);
  debugPrintln(m_u8MealNum);
  for(int i = 0; i < 10; i++)
  {
    debugPrint(m_oMealSettingList[i].m_cName);debugPrint(" : ");
    debugPrint(m_oMealSettingList[i].m_fFeedWeight); debugPrint(" : "); 
    debugPrintln(m_oMealSettingList[i].m_u8NumofSamples);
  }
  return 1;
}

/************************************************************
 * Read meal Setting From File
*************************************************************/
int CMeal::readSetting(FILESYSTEM* filesystem,char *SettingData)
{
  int ret = 0;
  int Size = filesystem->getFileSize(FNAME_MEALSETTING);
  debugPrintln(Size);
  if (Size >= 1024)
  { 
    debugPrintln("Wrong Setting file size...");
    return 0;
  }
  /*Read File in data streM*/
  ret = filesystem->readFile(FNAME_MEALSETTING,SettingData);
  debugPrintln(SettingData);
  if(ret > 0)
  {
    return 1;
  }
  return 0;
}

/************************************************************
 * Write meal setting to File
*************************************************************/
int CMeal::writeSetting(FILESYSTEM* filesystem)
{

}
/************************************************************
 * clear Pond Meal Data
*************************************************************/
void CMeal::clearPondMealData(void)
{
  debugPrintln("Clear last Meal Data....");
  for(int pnum = 0; pnum < TOTAL_PONDS; pnum++)
  {
    m_oPondList[pnum].m_fFeedWeightActaul = 0;
    m_oPondList[pnum].m_u8FeedVerified = 0;
    m_oPondList[pnum].m_u8SamplesVerified = 0;
    m_oPondList[pnum].m_u8checkTrayLeftover = -1;
    memset(m_oPondList[pnum].m_fSampleWtArray,0,sizeof(m_oPondList[pnum].m_fSampleWtArray));    
  }

}

  