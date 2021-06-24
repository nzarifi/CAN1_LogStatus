#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <ctype.h>

#include "MasterRackStatus.h"

#define BUFF_LEN  2048
#define SYS_STATUS_STRUCT_LEN	    1992
#define MASTER_STATUS_STRUCT_LEN    1968
#define CAN_MSG_DATA_LEN            8
#define STATUS_LEN_MSG_ID           0x143
#define STATUS_DATA_MSG_ID          0x144
#define MAX_STR                     300
#define STATUS_LOGFILENAME          "CAN1_LogStatus.txt"
#define CSV_LOGFILENAME             "BatteryStatusData.csv"
#define FIRST_HEADER_COMMON_CELL    ",,,,,,,,,,,"
#define FIRST_HEADER_7_CELL         ",,,,,,,,,,,,,Cassette01,Cassette02,Cassette03,Cassette04,Cassette05,Cassette06,Cassette07,Cassette01,Cassette02,Cassette03,Cassette04,Cassette05,Cassette06,Cassette07,Cassette01,Cassette02,Cassette03,Cassette04,Cassette05,Cassette06,Cassette07,,,"
#define FIRST_HEADER_10_CELL        ",,,,,,,,,,,,,Cassette01,Cassette02,Cassette03,Cassette04,Cassette05,Cassette06,Cassette07,Cassette08,Cassette09,Cassette10,Cassette01,Cassette02,Cassette03,Cassette04,Cassette05,Cassette06,Cassette07,Cassette08,Cassette09,Cassette10,Cassette01,Cassette02,Cassette03,Cassette04,Cassette05,Cassette06,Cassette07,Cassette08,Cassette09,Cassette10,,,"
#define CSV_COMMON_HEADER           "TimeStamp_ms,EWS,EWS Return,Contactor,SOC,Total Current,Errors,MasterOperationState,Power Off Reason,Max Cass Temp,ChargerPresence,"																	 
#define CSV_HEADER_7_CELL           "Contactor,Current,String Voltage,V Before Fuse,V After Fuse,V After Contactor,Vref,3V3,5V,24V,Internal Temp,Errors,OperationState,Voltage,Voltage,Voltage,Voltage,Voltage,Voltage,Voltage,Temperature 1,Temperature 1,Temperature 1,Temperature 1,Temperature 1,Temperature 1,Temperature 1,Temperature 2,Temperature 2,Temperature 2,Temperature 2,Temperature 2,Temperature 2,Temperature 2,Temperature min,Temperature max,"
#define CSV_HEADER_10_CELL          "Contactor,Current,String Voltage,V Before Fuse,V After Fuse,V After Contactor,Vref,3V3,5V,24V,Internal Temp,Errors,OperationState,Voltage,Voltage,Voltage,Voltage,Voltage,Voltage,Voltage,Voltage,Voltage,Voltage,Temperature 1,Temperature 1,Temperature 1,Temperature 1,Temperature 1,Temperature 1,Temperature 1,Temperature 1,Temperature 1,Temperature 1,Temperature 2,Temperature 2,Temperature 2,Temperature 2,Temperature 2,Temperature 2,Temperature 2,Temperature 2,Temperature 2,Temperature 2,Temperature min,Temperature max,"
#define VOLTAGE_DIVISOR             (float)10000.0												  
#define V_BEFORE_FUSE               (float)0.00255589    
#define V_AFTER_FUSE                (float)0.00171664
#define V_AFTER_CONTACTOR           (float)0.00171664
#define VREF                        (float)0.0001510643  //niloo
#define V3                          (float)0.0000762951
#define V5                          (float)0.000114443
#define V24                         (float)0.000419623  
uint8_t CAN_StatusBuff[BUFF_LEN] ={[0 ... 2047] = 0x00};  //Buffer to Convert all the 0x144 Binary Status Message Data
MasterMakerStatus_t *BattStatusPtr;						  //Pointer to SystemController MasterMakerStatus_t Structure Buffer

FILE *fp_read = NULL;   //Status Log File Pointer
FILE *fp_write = NULL;  //CSV File Pointer
char str[MAX_STR] = {0};      //Buffer to hold read line from StatusLog
char* filename_read = STATUS_LOGFILENAME;
char* filename_write = CSV_LOGFILENAME;

long Number = 0;
char* token;
uint32_t TimeStamp_ms;
uint32_t StartTime_ms = 0;
uint16_t MsgID; 
uint16_t StatusMsgLength;
uint16_t StatusMsgCtr = 0; 
uint16_t MaxStatusMessages;
uint8_t  NumRacks = 4;  //Default to 4 Racks to Dump to CSV														   


void DumpBuff(void)  //Dumps the CAN_StatusBuff to the console to check against the received CAN Data
{
    uint16_t i;
    printf("\nCAN Status_Msg TimeStamp:%d  MsgLength:%d\n",StartTime_ms,StatusMsgLength);
    for (i=0;i<StatusMsgLength;i+=8)
    {
        printf("\r%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",CAN_StatusBuff[i],CAN_StatusBuff[i+1],CAN_StatusBuff[i+2],CAN_StatusBuff[i+3],
               CAN_StatusBuff[i+4],CAN_StatusBuff[i+5],CAN_StatusBuff[i+6],CAN_StatusBuff[i+7]);
    }
}


void WriteBuffToCSV(void)
{
   static BOOL bCSV_Header_Written = FALSE;
   static uint8_t NumCells;
   //static uint8_t NumRacks;
   float Vc1,Vc2,Vc3,Vc4,Vc5,Vc6,Vc7,Vc8,Vc9,Vc10;
   uint16_t Tc1_1,Tc2_1,Tc3_1,Tc4_1,Tc5_1,Tc6_1,Tc7_1,Tc8_1,Tc9_1,Tc10_1,Tc1_2,Tc2_2,Tc3_2,Tc4_2,Tc5_2,Tc6_2,Tc7_2,Tc8_2,Tc9_2,Tc10_2;																																										  
   uint8_t i;

   // define functions to validate enum values if value does not exist in enum dictionary, write integer NOT string 
   const int Two_Vals[] = {0,1};
   int Check_Two_EnumVal(int x)
        {      
            for(int i=0; i<(sizeof(Two_Vals)/sizeof(*Two_Vals)); i++)
                {
                     if(Two_Vals[i] == x){ return 1; }
                }
             return 0;
                 
        }  
   const int Three_Vals[] = {0,1,2};
   int Check_Three_EnumVal(int x)
        {      
            for(int i=0; i<(sizeof(Three_Vals)/sizeof(*Three_Vals)); i++)
                {
                     if(Three_Vals[i] == x){ return 1; }
                }
             return 0;
                 
        }
    const int Four_Vals[] = {0,1,2,3};
    int Check_Four_EnumVal(int x)
        {      
            for(int i=0; i<(sizeof(Four_Vals)/sizeof(*Four_Vals)); i++)
                {
                     if(Four_Vals[i] == x){ return 1; }
                }
             return 0;       
        }
    const int TwentyFour_EnumVal[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
    int Check_TwentyFour_EnumVal(int x)
        {      
            for(int i=0; i<(sizeof(TwentyFour_EnumVal)/sizeof(*TwentyFour_EnumVal)); i++)
            {
                if(TwentyFour_EnumVal[i] == x){ return 1; }
            }
                return 0;
        } 

    const int Fifteen_EnumVal[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    int Check_Fifteen_EnumVal(int x)
        {      
            for(int i=0; i<(sizeof(Fifteen_EnumVal)/sizeof(*Fifteen_EnumVal)); i++)
            {
                if(Fifteen_EnumVal[i] == x){ return 1; }
            }
                return 0;
        }


   if (bCSV_Header_Written == FALSE)    /*Do this 1 time*/
   {
       bCSV_Header_Written = TRUE;
       BattStatusPtr = (MasterMakerStatus_t *) CAN_StatusBuff;   //Set the Point to Start at the Converted 0x144 Message Data
       //Get Number of Cells and Racks in this Battery  stringVoltage in 100uV units?
       //printf("BattStatusPtr->bMSCtrlStatus[0].batteryStringStatus[0].stringVoltage:%d\n",BattStatusPtr->bMSCtrlStatus[0].batteryStringStatus[0].stringVoltage);
       if ((BattStatusPtr->bMSCtrlStatus[0].batteryStringStatus[0].stringVoltage/10000) < 33)
       {
           NumCells = 7;
       }
       else
           NumCells = 10;

       //NumRacks = BattStatusPtr->bMSCtrlStatus[0].stateHandlerStatus.bStringNumber; Not Valid
       
       //Open CSV file for writing
       fp_write = fopen(filename_write, "w");
       if (fp_write == NULL)
       {
           printf("Could not open file %s\n\r",filename_write);
           fclose(fp_read);   //close the opened Readfile
           exit(1);
       }
       else
       {
           printf("%s Opened for Writing\n\r",filename_write);
       }
       //Write CSV File Header Line Identifying all the Columns
       //First Header
       fprintf(fp_write,FIRST_HEADER_COMMON_CELL);
       
       for (i=0;i<NumRacks;++i)
       {
            
            if (NumCells == 7)
            {
				sprintf(str,"Cabinet0%.1d",i+1);
				strcat(str,FIRST_HEADER_7_CELL);   
            }
            
            else
            {
                //fprintf(fp_write,CSV_HEADER_10_CELL);
                sprintf(str,"Cabinet0%.1d",i+1);  //only adds Rack Number to first column
                strcat(str,FIRST_HEADER_10_CELL);
                
            }
            fprintf(fp_write,"%s",str);   
       }
       fprintf(fp_write,"\n");        
       //Second Header
       fprintf(fp_write,CSV_COMMON_HEADER);
       for (i=0;i<NumRacks;++i)
       {
            if (NumCells == 7)
            {
                fprintf(fp_write,CSV_HEADER_7_CELL);
				//sprintf(str,"Rack_%.1d",i+1);
				//strcat(str,CSV_HEADER_7_CELL);
            }
            else
            {
                //fprintf(fp_write,CSV_HEADER_10_CELL);
                //sprintf(str,"Rack_%.1d",i+1);  //only adds Rack Number to first column
                //strcat(str,CSV_HEADER_10_CELL);
                fprintf(fp_write,"%s",CSV_HEADER_10_CELL);
            }
            //fprintf(fp_write,"%s",str);
       } 
       fprintf(fp_write,"\n");   
   }

   if (NumCells == 7)    //Write 7 Cell CSV Data
   {
       //Put Comma Separated 7 Cell CSV Data Writing Here
       fprintf(fp_write,"%d,",StartTime_ms);     
       if (Check_Two_EnumVal(BattStatusPtr->eWSStatus.eWSReturn) ==0){
            fprintf(fp_write,"%d,",BattStatusPtr->eWSStatus.eWSReturn);
            }
       else
            {
            fprintf(fp_write,"%s,",EWSReturn_names[BattStatusPtr->eWSStatus.eWSReturn]);
            } 

       if (Check_Three_EnumVal(BattStatusPtr->eWSStatus.eWSActivation) ==0){
            fprintf(fp_write,"%d,",BattStatusPtr->eWSStatus.eWSActivation);
            }
       else
            {
            fprintf(fp_write,"%s,",EWSActivation_names[BattStatusPtr->eWSStatus.eWSActivation]);
            }
       if (Check_Four_EnumVal(BattStatusPtr->bMSCtrlStatus[0].contactorStatus.contactorState) ==0){
            fprintf(fp_write,"%d,",BattStatusPtr->bMSCtrlStatus[0].contactorStatus.contactorState);
            }
       else
            {
            fprintf(fp_write,"%s,",ContactorStates_names[BattStatusPtr->bMSCtrlStatus[0].contactorStatus.contactorState]);
            }
       fprintf(fp_write,"%d,%d,",BattStatusPtr->masterStateHandlerStatus.stateOfChargePack,BattStatusPtr->masterStateHandlerStatus.globalPackCurrent);
       fprintf(fp_write,"%d,",BattStatusPtr->masterMakerGeneralStatus.masterSeriousErrorOccurred);
       
       if (Check_TwentyFour_EnumVal(BattStatusPtr->masterStateHandlerStatus.masterOperationState) ==0){
           fprintf(fp_write,"%d,",BattStatusPtr->masterStateHandlerStatus.masterOperationState);
           }
       else
           {
           fprintf(fp_write,"%s,",MasterOperationState_names[BattStatusPtr->masterStateHandlerStatus.masterOperationState]);
           }
       fprintf(fp_write,"%d,",BattStatusPtr->masterStateHandlerStatus.buzzer[1]);
       fprintf(fp_write,"%d,",BattStatusPtr->masterStateHandlerStatus.maxCellTemp);
       

       if (Check_Three_EnumVal(BattStatusPtr->chargerStatus.chargerPresence) ==0){
            fprintf(fp_write,"%d,",BattStatusPtr->chargerStatus.chargerPresence);
            }
       else
            {
            fprintf(fp_write,"%s,",ChargerPresence_names[BattStatusPtr->chargerStatus.chargerPresence]);
            }
       
       for (i=0;i<NumRacks;++i)
       {
            if (Check_Three_EnumVal(BattStatusPtr->bMSCtrlStatus[i].contactorStatus.contactorConnectionIndication) ==0){
            fprintf(fp_write,"%d,",BattStatusPtr->bMSCtrlStatus[i].contactorStatus.contactorConnectionIndication);
            }
            else
            {
            fprintf(fp_write,"%s,",ContactorConnectionIndication_names[BattStatusPtr->bMSCtrlStatus[i].contactorStatus.contactorConnectionIndication]);
            }
           
           fprintf(fp_write,"%d,",BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.stringCurrentA);
           fprintf(fp_write,"%f,",(float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].stringVoltage/VOLTAGE_DIVISOR);
           fprintf(fp_write,"%f,%f,",(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.voltBeforeFuse*V_BEFORE_FUSE,(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.voltAfterFuse*V_AFTER_FUSE);
           fprintf(fp_write,"%f,",(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.voltAfterContactor*V_AFTER_CONTACTOR);
           fprintf(fp_write,"%f,%f,",(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.voltVREF*VREF,(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.volt3V3*V3);
           fprintf(fp_write,"%f,%f,",(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.volt5V*V5,(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.volt24V*V24);
           fprintf(fp_write,"%d,",BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.TemperatureInternalTherm);
           fprintf(fp_write,"%d,",BattStatusPtr->bMSCtrlStatus[i].generalStatus.seriousErrorOccurred);
           
           if (Check_Fifteen_EnumVal(BattStatusPtr->bMSCtrlStatus[i].stateHandlerStatus.operationState) ==0){
           fprintf(fp_write,"%d,",BattStatusPtr->bMSCtrlStatus[i].stateHandlerStatus.operationState);
           }
           else
           {
           fprintf(fp_write,"%s,",OperationState_names[BattStatusPtr->bMSCtrlStatus[i].stateHandlerStatus.operationState]);
           }
           //Vcells
           Vc1 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[0]/VOLTAGE_DIVISOR);
           Vc2 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[1]/VOLTAGE_DIVISOR);
           Vc3 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[2]/VOLTAGE_DIVISOR);
           Vc4 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[3]/VOLTAGE_DIVISOR);
           Vc5 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[4]/VOLTAGE_DIVISOR);
           Vc6 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[5]/VOLTAGE_DIVISOR);
           Vc7 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[6]/VOLTAGE_DIVISOR);
           fprintf(fp_write,"%f,%f,%f,%f,%f,%f,%f,",Vc1,Vc2,Vc3,Vc4,Vc5,Vc6,Vc7);
           Tc1_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][0];
           Tc2_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][1];
           Tc3_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][2];
           Tc4_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][3];
           Tc5_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][4];
           Tc6_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][5];
           Tc7_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][6];
           fprintf(fp_write,"%d,%d,%d,%d,%d,%d,%d,",Tc1_1,Tc2_1,Tc3_1,Tc4_1,Tc5_1,Tc6_1,Tc7_1);
           Tc1_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][0];
           Tc2_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][1];
           Tc3_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][2];
           Tc4_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][3];
           Tc5_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][4];
           Tc6_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][5];
           Tc7_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][6];
           fprintf(fp_write,"%d,%d,%d,%d,%d,%d,%d,",Tc1_2,Tc2_2,Tc3_2,Tc4_2,Tc5_2,Tc6_2,Tc7_2);
           int numbers[14] = {Tc1_1,Tc2_1,Tc3_1,Tc4_1,Tc5_1,Tc6_1,Tc7_1,Tc1_2,Tc2_2,Tc3_2,Tc4_2,Tc5_2,Tc6_2,Tc7_2};
           int min,max,j;
           min=max=numbers[0];
           for(j=0; j<14; j++)
            {
                if(min>numbers[j])
		            min=numbers[j];   
		        if(max<numbers[j])
		            max=numbers[j];       
            }
            fprintf(fp_write,"%d,%d,",min,max);

       }
       fprintf(fp_write,"\n");    //End of CSV Line
   }
   else                  //Write 10 Cell CSV Data
   {
       //Put Comma Separated 10 Cell CSV Data Writing Here
       fprintf(fp_write,"%d,",StartTime_ms);
        if (Check_Two_EnumVal(BattStatusPtr->eWSStatus.eWSReturn) ==0){
            fprintf(fp_write,"%d,",BattStatusPtr->eWSStatus.eWSReturn);
            }
       else
            {
            fprintf(fp_write,"%s,",EWSReturn_names[BattStatusPtr->eWSStatus.eWSReturn]);
            } 

        if (Check_Three_EnumVal(BattStatusPtr->eWSStatus.eWSActivation) ==0){
            fprintf(fp_write,"%d,",BattStatusPtr->eWSStatus.eWSActivation);
            }
       else
            {
            fprintf(fp_write,"%s,",EWSActivation_names[BattStatusPtr->eWSStatus.eWSActivation]);
            } 

        if (Check_Four_EnumVal(BattStatusPtr->bMSCtrlStatus[0].contactorStatus.contactorState) ==0){
            fprintf(fp_write,"%d,",BattStatusPtr->bMSCtrlStatus[0].contactorStatus.contactorState);
            }
       else
            {
            fprintf(fp_write,"%s,",ContactorStates_names[BattStatusPtr->bMSCtrlStatus[0].contactorStatus.contactorState]);
            }
       
       
       fprintf(fp_write,"%d,%d,",BattStatusPtr->masterStateHandlerStatus.stateOfChargePack,BattStatusPtr->masterStateHandlerStatus.globalPackCurrent);
       fprintf(fp_write,"%d,",BattStatusPtr->masterMakerGeneralStatus.masterSeriousErrorOccurred);
       
        if (Check_TwentyFour_EnumVal(BattStatusPtr->masterStateHandlerStatus.masterOperationState) ==0){
           fprintf(fp_write,"%d,",BattStatusPtr->masterStateHandlerStatus.masterOperationState);
           }
           else
           {
           fprintf(fp_write,"%s,",MasterOperationState_names[BattStatusPtr->masterStateHandlerStatus.masterOperationState]);
           }
       fprintf(fp_write,"%d,",BattStatusPtr->masterStateHandlerStatus.buzzer[1]);
       fprintf(fp_write,"%d,",BattStatusPtr->masterStateHandlerStatus.maxCellTemp); 

       if (Check_Three_EnumVal(BattStatusPtr->chargerStatus.chargerPresence) ==0){
            fprintf(fp_write,"%d,",BattStatusPtr->chargerStatus.chargerPresence);
            }
       else
            {
            fprintf(fp_write,"%s,",ChargerPresence_names[BattStatusPtr->chargerStatus.chargerPresence]);
            }
       
       
       for (i=0;i<NumRacks;++i)
       {
            if (Check_Three_EnumVal(BattStatusPtr->bMSCtrlStatus[i].contactorStatus.contactorConnectionIndication) ==0){
            fprintf(fp_write,"%d,",BattStatusPtr->bMSCtrlStatus[i].contactorStatus.contactorConnectionIndication);
            }
            else
            {
            fprintf(fp_write,"%s,",ContactorConnectionIndication_names[BattStatusPtr->bMSCtrlStatus[i].contactorStatus.contactorConnectionIndication]);
            }
           
           fprintf(fp_write,"%d,",BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.stringCurrentA); 
           fprintf(fp_write,"%f,",(float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].stringVoltage/VOLTAGE_DIVISOR);
           fprintf(fp_write,"%f,%f,",(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.voltBeforeFuse*V_BEFORE_FUSE,(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.voltAfterFuse*V_AFTER_FUSE);
           fprintf(fp_write,"%f,",(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.voltAfterContactor*V_AFTER_CONTACTOR);                               
           fprintf(fp_write,"%f,%f,",(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.voltVREF*VREF,(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.volt3V3*V3);
           fprintf(fp_write,"%f,%f,",(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.volt5V*V5,(float)BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.volt24V*V24);
           fprintf(fp_write,"%d,",BattStatusPtr->bMSCtrlStatus[i].analogInpStatus.TemperatureInternalTherm);
           fprintf(fp_write,"%d,",BattStatusPtr->bMSCtrlStatus[i].generalStatus.seriousErrorOccurred);
           
           if (Check_Fifteen_EnumVal(BattStatusPtr->bMSCtrlStatus[i].stateHandlerStatus.operationState) ==0){
           fprintf(fp_write,"%d,",BattStatusPtr->bMSCtrlStatus[i].stateHandlerStatus.operationState);
           }
           else
           {
           fprintf(fp_write,"%s,",OperationState_names[BattStatusPtr->bMSCtrlStatus[i].stateHandlerStatus.operationState]);
           }

           //Vcells
           Vc1 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[0]/VOLTAGE_DIVISOR);
           Vc2 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[1]/VOLTAGE_DIVISOR);
           Vc3 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[2]/VOLTAGE_DIVISOR);
           Vc4 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[3]/VOLTAGE_DIVISOR);
           Vc5 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[4]/VOLTAGE_DIVISOR);
           Vc6 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[5]/VOLTAGE_DIVISOR);
           Vc7 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[6]/VOLTAGE_DIVISOR);
           Vc8 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[7]/VOLTAGE_DIVISOR);
           Vc9 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[8]/VOLTAGE_DIVISOR);
           Vc10 = ((float)BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellVolt[9]/VOLTAGE_DIVISOR);
           fprintf(fp_write,"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,",Vc1,Vc2,Vc3,Vc4,Vc5,Vc6,Vc7,Vc8,Vc9,Vc10);

           Tc1_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][0];
           Tc2_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][1];
           Tc3_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][2];
           Tc4_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][3];
           Tc5_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][4];
           Tc6_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][5];
           Tc7_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][6];
           Tc8_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][7];
           Tc9_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][8];
           Tc10_1 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[0][9];
           fprintf(fp_write,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,",Tc1_1,Tc2_1,Tc3_1,Tc4_1,Tc5_1,Tc6_1,Tc7_1,Tc8_1,Tc9_1,Tc10_1);
           Tc1_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][0];
           Tc2_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][1];
           Tc3_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][2];
           Tc4_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][3];
           Tc5_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][4];
           Tc6_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][5];
           Tc7_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][6];
           Tc8_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][7];
           Tc9_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][8];
           Tc10_2 = BattStatusPtr->bMSCtrlStatus[i].batteryStringStatus[0].cellTemp[1][9];
           fprintf(fp_write,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,",Tc1_2,Tc2_2,Tc3_2,Tc4_2,Tc5_2,Tc6_2,Tc7_2,Tc8_2,Tc9_2,Tc10_2);
           int numbers[20] = {Tc1_1,Tc2_1,Tc3_1,Tc4_1,Tc5_1,Tc6_1,Tc7_1,Tc8_1,Tc9_1,Tc10_1,Tc1_2,Tc2_2,Tc3_2,Tc4_2,Tc5_2,Tc6_2,Tc7_2,Tc8_2,Tc9_2,Tc10_2};
           int min,max,j;
           min=max=numbers[0];
           for(j=0; j<20; j++)
            {
                if(min>numbers[j])
		            min=numbers[j];   
		        if(max<numbers[j])
		            max=numbers[j];       
            }
            fprintf(fp_write,"%d,%d,",min,max);
 
       }
       fprintf(fp_write,"\n");    //End of CSV Line
   }
}

 
int main (int argc, char *argv[])
{
 uint16_t NumberOfRcvdStatusMsgs;
 uint8_t *InBuffPtr;                                       //Point to place converted Status Data
 uint8_t i;                                                          //

 BattStatusPtr = (MasterMakerStatus_t *) CAN_StatusBuff;   //Set the Point to Start at the Converted 0x144 Message Data

 printf("MasterMakerGeneralStatus_t :%d\n\r",sizeof(MasterMakerGeneralStatus_t));
 printf("MasterStateHandlerStatus_t :%d\n\r",sizeof(MasterStateHandlerStatus_t));
 printf("CAN1HostCommunStatus_t :%d\n\r",sizeof(CAN1HostCommunStatus_t));
 printf("ChargerStatus_t :%d\n\r",sizeof(ChargerStatus_t));
 printf("EWSStatus_t :%d\n\r",sizeof(EWSStatus_t));
 printf("BMSCtrlGeneralStatus_t :%d\n\r",sizeof(BMSCtrlGeneralStatus_t));
 printf("ContactorStatus_t:%d\n\r",sizeof(ContactorStatus_t));
 printf("AnalogInpStatus_t :%d\n\r",sizeof(AnalogInpStatus_t));
 printf("StateHandlerStatus_t :%d\n\r",sizeof(StateHandlerStatus_t));
 printf("BalancerActiveState_t :%d\n\r",sizeof(BalancerActiveState_t));
 printf("BatteryStringStatus_t :%d\n\r",sizeof(BatteryStringStatus_t));
 printf("PassiveBalancingStatus_t :%d\n\r",sizeof(PassiveBalancingStatus_t));
 printf("BMSCtrlStatus_t :%d\n\r",sizeof(BMSCtrlStatus_t));
 printf("MasterMakerStatus_t :%d\n\r",sizeof(MasterMakerStatus_t));

 printf("\nEnter NumberOfRacks(1-4):");   //Enter Number of Racks to Display for DCIR 
 if (gets(str) != NULL)
 {
   token = strtok(str, "\0 \n");
   if (token[0]!=0)
   {
    Number = strtol((const char *)token, NULL, 0);  //Get NumberOfRacks To Display
    if ((Number>0)&&(Number<=4))
    {
      NumRacks = (uint8_t)Number;
    }
   }
 }																					  
 //Open CAN1_LogStatus.txt for Text Reading
 fp_read = fopen(filename_read, "r");
 if (fp_read == NULL)
 {
     printf("Could not open file %s",filename_read);
     return 1;
 }
 else
    printf("%s fp_read Opened\n\r",filename_read);
 
//Process All lines in the StatusLog file
while ( fgets(str, MAX_STR, fp_read) != NULL)  //Process all the lines in the Status Log File
 {
     //puts(str);
     /*Header in File is TimeStamp, ignore all MsgID's other than 0x143-StatusMsgLen and 0x144-StatusMsgData*/
     token = strtok(str, " \n");  // from the start of string data grab the 1st Token it is either 'TimeStamp' or an alphanumeric number delimitor is the space char
     if (isdigit(token[0]) == FALSE)     //if First Token is not a digit ignore this line as its probably a 'TimeStamp heading'
     {
         continue;
     }

     /*All CAN Msg Lines are :  Timestamp_ms 0xMsgID hexData0 hexData1 hexData2 hexData3 hexData4 hexData5 hexData6 hexData7*/
     Number = strtol((const char *)token, NULL, 0);  //1st Token TimeStamp
     TimeStamp_ms = (uint32_t)Number;
     //printf("TimeStamp:%d\n",TimeStamp_ms);

     token = strtok(NULL, " \n");                      //2nd Token = Hex CAN MSGID
     Number = strtol((const char *)token, NULL, 0);  //Convert next Hexadecim to number 0x tells strtol to treat as Hexadecimal
     MsgID = (uint16_t) Number;
     //printf("MsgID:0x%4x\n",MsgID);
     if ((MsgID == STATUS_LEN_MSG_ID) || (MsgID == STATUS_DATA_MSG_ID))  // 0x143==323 0x144==324
     {
         if (MsgID == STATUS_LEN_MSG_ID)
         {
             token = strtok(NULL, " \n");                    //3rd Token = Hex LSB STATUS LENGTH
             Number = strtol((const char *)token, NULL, 16); //Convert AscII Hex to number
             StatusMsgLength = (uint16_t) Number;
             token = strtok(NULL, " \n");                    //4th Token = Hex MSB STATUS LENGTH
             Number = strtol((const char *)token, NULL, 16); //Convert AscII Hex to number
             StatusMsgLength += (uint16_t)(Number*256);       //Non-Zero StatusMsgLength is Flag to Start Receiving the 0x144 Data Blocks
             MaxStatusMessages = StatusMsgLength/CAN_MSG_DATA_LEN;
             //printf("Status_MsgLength:%d  NumberOfMsgsToRead:%d\n",StatusMsgLength,MaxStatusMessages);
             InBuffPtr = (uint8_t *) CAN_StatusBuff;   //reSet the Pointer to Buff Start for the Converted 0x144 Message Data as we just rcvd Start Header for beginning of DataStructure
             NumberOfRcvdStatusMsgs = 0;               //reset MsgCtr
             StartTime_ms = TimeStamp_ms;              //Start Timestamp for the Status DataBlock as BMS Copied Image of this to be sent when the DataLength was calculated
         }
         else
         if ((MsgID == STATUS_DATA_MSG_ID) && (StatusMsgLength != 0)) //Status Msg Length was processed so we can start receiving the Data
         {
             //Process the Status Data into the CAN_StatusBuff
             for (i=0;i<CAN_MSG_DATA_LEN;++i) 
             {
                 token = strtok(NULL, " \n");                      //3rd Token = Hex LSB STATUS LENGTH
                 if (token == NULL)                              //Error in rcd message abort this record and restart looking for next 0x143
                 {
                     printf("Error 0x144 Record Aborted: %s\n",str);
                     StatusMsgLength = 0;                       //REstart looking for 0x143
                     continue;
                 }
                 Number = strtol((const char *)token, NULL, 16); //Convert AscII Hex to number
                 *InBuffPtr++ = (uint8_t)Number;                 //put Byte into next buff location
             }
             ++NumberOfRcvdStatusMsgs;
             //printf("NumberOfRcvdStatusMsgs:%d\n",NumberOfRcvdStatusMsgs);
             if (NumberOfRcvdStatusMsgs == MaxStatusMessages)
             {
                 //DumpBuff();
                 WriteBuffToCSV();
                 StatusMsgLength = 0;
                 continue;
             }
         }
         else
             continue;
     }
     else
     {
         continue;  //ignore message process next data
     }
 }
 printf("Closing fp_read:%s fp_write:%s\n\r",filename_read,filename_write);
 fclose(fp_read);
 fclose(fp_write);
 return 0;
 
}

