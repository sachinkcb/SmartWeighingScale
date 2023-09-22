/* 
  PM_APP.cpp - Application file is used to read the registor values of energy meter ic , calculate the suggested/ required capbank value and also used for detection of 
               power source
            
  Dev: Infiplus Team 
  May 2021  
*/

#include "CWeighingScale.h"

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

/* Construct */
CWeighingScale::CWeighingScale()
{
  m_bFrameReady = false;
}
/* Destruct */
CWeighingScale::~CWeighingScale(){}
/**************************************************
 * decode incomming data form Scale
 * Input : single char
 * output: result
**************************************************/
uint8_t CWeighingScale::decode(char c)
{
  //debugPrint(c);
  switch(c)
  {
    case '$': // Start Of Frame
      {
        m_frameStared = true;
        isValidFrame = false;
        m_bFrameReady = false;
        rpos = 0;       
      }
      break;
    case '#':
      {
        if(m_frameStared)
        {
          m_frameStared = false;
          m_ReceivedFrame[rpos]= 0;
          isValidFrame = true;
        }
      }
      break;
    default:
      if(rpos < MAX_FARME_SIZE && m_frameStared)
        m_ReceivedFrame[rpos++] = c;
      break;
  }
  if(isValidFrame)
  {
    isValidFrame = false;
    debugPrintln();
    debugPrintln(m_ReceivedFrame);
    m_bFrameReady = true;
  }
}
/**************************************************
 * Parse Commands 
 * Input : single char
 * output: result
**************************************************/
uint8_t CWeighingScale::ParseCommnads(void)
{
  int res = 0;
  /*Parse Command */
  if(m_bFrameReady)
  {
    debugPrintln("Valid Frame...");
    sscanf(m_ReceivedFrame,"%c,%d,%d,%f,%d",&m_oFrame.m_cMode,&m_oFrame.m_u8Operation,&m_oFrame.m_u8pondNum,&m_oFrame.m_fWeight,&m_oFrame.m_u8SampleNum);
    /*As Array of ponds starts with 0, P1 is at 0 Position*/
    /*Validation*/
    if(m_oFrame.m_u8pondNum > 0)
    {
        m_oFrame.m_u8pondNum = m_oFrame.m_u8pondNum -1;
    }
    else
    {
      m_bFrameReady = false;
      /*Send OK wt to avoide Machine getting stuck in loop*/
      if(m_oFrame.m_cMode == 'C')
        SendFrame(ERROR_FRAME,5);
      debugPrintln("Wrong Frame...");
      return WRONG_FRAME;
    }
    // debugPrintln(m_oFrame.m_cMode);
    // debugPrintln(m_oFrame.m_u8Operation);
    // debugPrintln(m_oFrame.m_u8pondNum);
    // debugPrintln(m_oFrame.m_fWeight);
    // debugPrintln(m_oFrame.m_u8SampleNum);
    // debugPrintf("Wt: %07.3f",m_oFrame.m_fWeight);
    debugPrintf("\nMode: %c, Op: %d, Pnum: %d, Wt : %07.3f, Snum: %d\n",m_oFrame.m_cMode,m_oFrame.m_u8Operation,m_oFrame.m_u8pondNum,m_oFrame.m_fWeight,m_oFrame.m_u8SampleNum);
    /*Take action As per data recived*/
    switch (m_oFrame.m_cMode)
    {
    case 'F':
      /* Process Feed Data */
      res = ProcessFeedData();
      break;
    case 'S':
      /* process Sample data */
      res = ProcessSampleData();
      break;
    case 'C':
      /* process Check tray data */
      res = ProcessChecktryData();
      break;  
    default:
      break;
    }
    m_bFrameReady = false;
    return res;
  }
  return WRONG_FRAME;
}

/**************************************************
 * Process Send farme to weighing scale
 * Input : sendFrame
 * output: result
**************************************************/
uint8_t CWeighingScale::SendFrame(const char *sendFrame,int repeatCnt)
{ 
  delay(400);
  for(int rep = 0; rep <= repeatCnt; rep++)  
  {
    Serial1.write(sendFrame);
    delay(10);
  }
  return 1;
}


/**************************************************
 * Process Feed Data form Scale
 * Input : 
 * output: result
**************************************************/
uint8_t CWeighingScale::ProcessFeedData(void)
{
  debugPrintln("Receiced Feed data Frame...");
  char sendFrame[100];
  if(m_oFrame.m_u8Operation == 1) //For set data
  {
    /*Assign actual wt if its less than 100Kg max capacity of scale ,big vale is wrong data*/
    if( m_oFrame.m_fWeight <= (m_oMeal.m_oMealSettingList[m_oFrame.m_u8pondNum].m_fFeedWeight + 0.200))
    {
      m_oMeal.m_oPondList[m_oFrame.m_u8pondNum].m_fFeedWeightActaul = m_oFrame.m_fWeight;
      m_oMeal.m_oPondList[m_oFrame.m_u8pondNum].m_u8FeedVerified = 1; 
      /*Send OK wt Frame Frame to Scale*/
      SendFrame(OK_FRAME,5);
      return UPDATE_FRAME;
    }
    else
    {
      SendFrame(ERROR_FRAME,5);
    }
  }
  else if(m_oFrame.m_u8Operation == 0) //For get data
  {
    /*If weight in frame is not matching with desired wt set it 
    the scale is programmed for sending back the command for verification, this logic will send the command again if wt not matching*/
    if(m_oFrame.m_fWeight !=  m_oMeal.m_oMealSettingList[m_oFrame.m_u8pondNum].m_fFeedWeight)
    {
      /*Create Frame*/
      sprintf(sendFrame, "$F,0,%d,%07.3f,0#",m_oFrame.m_u8pondNum + 1, m_oMeal.m_oMealSettingList[m_oFrame.m_u8pondNum].m_fFeedWeight);
      debugPrint(sendFrame);
      /*Send Frame to Scale*/
      SendFrame(sendFrame);
    } 
    else
    {
      debugPrint("Send Feed Command recived by scale correctly");
    }   
  } 
  return OK; 
}
/**************************************************
 * Process Sample Data form Scale
 * Input : 
 * output: result
**************************************************/
uint8_t CWeighingScale::ProcessSampleData(void)
{
  debugPrintln("Receiced Sample data Frame...");
  char sendFrame[100];

  if(m_oFrame.m_u8Operation == 1) //For set data
  {
    /*Sample wt in KG*/
    float sampleWt =  (m_oMeal.m_oMealSettingList[m_oFrame.m_u8pondNum].m_fFeedWeight * m_oMeal.m_u8SampleRate)/1000 ;
    /*sample Wt should be below Sample wt + 10gm*/
    if(m_oFrame.m_u8SampleNum > 0 && m_oFrame.m_fWeight <= (sampleWt + 0.010))
    {
      /*Assign actual wt measured*/
      m_oMeal.m_oPondList[m_oFrame.m_u8pondNum].m_fSampleWtArray[m_oFrame.m_u8SampleNum-1] = m_oFrame.m_fWeight;
      debugPrintln(m_oFrame.m_fWeight);
      debugPrintln(m_oMeal.m_oPondList[m_oFrame.m_u8pondNum].m_fSampleWtArray[m_oFrame.m_u8SampleNum-1]);
      m_oMeal.m_oPondList[m_oFrame.m_u8pondNum].m_u8SamplesVerified = m_oFrame.m_u8SampleNum; 
      /*Send OK wt Frame Frame to Scale*/
      SendFrame(OK_FRAME,5);
      return UPDATE_FRAME;
    }  
    else
    {
      SendFrame(ERROR_FRAME,5);
    }  
  }
  else if(m_oFrame.m_u8Operation == 0) //For get data
  {
    /*Send wt for the selected Pond $S,0,1,0.135,1#*/
    int numOfSamples = m_oMeal.m_oMealSettingList[m_oFrame.m_u8pondNum].m_u8NumofSamples;
    /*Calculate sample Wt Per CheckTray =  Wt per pond * sample rate in Kg */
    float sampleWt =  (m_oMeal.m_oMealSettingList[m_oFrame.m_u8pondNum].m_fFeedWeight * m_oMeal.m_u8SampleRate)/1000 ;
    /*If weight in frame is not matching with desired wt set it 
    the scale is programmed for sending back the command for verification, this logic will send the command again if wt not matching*/
    if(m_oFrame.m_fWeight != sampleWt || m_oFrame.m_u8SampleNum != numOfSamples)
    {
      /*Create Frame*/
      sprintf(sendFrame,"$S,0,%d,%07.3f,%d#", m_oFrame.m_u8pondNum + 1, sampleWt , numOfSamples);
      debugPrint(sendFrame);
      /*Send Frame to Scale*/
      SendFrame(sendFrame);
    } 
    else
    {
      debugPrint("Send Sample Command recived by scale correctly");
    }    
  } 
  return OK; 
}
/**************************************************
 * Process Checktry Data form Scale
 * Input : 
 * output: result
**************************************************/
uint8_t CWeighingScale::ProcessChecktryData(void)
{
  debugPrintln("Receiced Checktray data Frame...");  
  /*Send wt for the selected Pond $S,0,1,0.135,1#*/
  int numOfSamples = m_oMeal.m_oMealSettingList[m_oFrame.m_u8pondNum].m_u8NumofSamples;
  /*Calculate sample Wt Per CheckTray =  Wt per pond * sample rate in Kg */
  float sampleWt =  (m_oMeal.m_oMealSettingList[m_oFrame.m_u8pondNum].m_fFeedWeight * m_oMeal.m_u8SampleRate)/1000 ;
  debugPrintln(sampleWt);
  if(m_oFrame.m_u8Operation == 1 && m_oFrame.m_fWeight <= (sampleWt * numOfSamples)) //For set data
  {
    /*Check tay sample can not be more than 1 kg*/
    if(m_oFrame.m_fWeight <= 1) 
    /*Assign actual wt measured*/
    m_oMeal.m_oPondList[m_oFrame.m_u8pondNum].m_u8checkTrayLeftover = m_oFrame.m_fWeight;
    debugPrintln(m_oFrame.m_fWeight);
    debugPrintln(m_oMeal.m_oPondList[m_oFrame.m_u8pondNum].m_u8checkTrayLeftover);
    /*Send OK wt Frame Frame to Scale*/
    SendFrame(OK_FRAME,5);
    return UPDATE_FRAME;
  }
  else
  {
    /*Send Error wt Frame Frame to Scale*/
    SendFrame(ERROR_FRAME,5);
  }
  return OK;
}