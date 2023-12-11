/*
SmartFarm code
Filename : app.c
Programmer(s) : 김태훈, 이성훈
*/


/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             PIN CONNTECTED
*	Temperatuer Sensor: 	PC1 (ADC1 Channel 11)
	SoilHumid Sensor: 		PC2 (ADC1 Channel 12)
	PhotoResistor Sensor: PC3(ADC1 Channel 13)
	Heater:								PD12(via relay module)
	Pump:									PD13(via relay module)
	LED:									PD9,PD10,PD11,PE10,PE11
	bluetooth:						PD5(USART2_TX), PD6(USART2_RX)
	button								PC4(Pull-up)
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

CPU_FP32 temperThreshold = 30.0;
CPU_FP32 soilHumidThreshold= 40.0;
CPU_FP32 photoThreshold=3000;

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static double PRECISION = 0.00000000000001;
static int MAX_NUMBER_STRING_SIZE = 32;

/*
		Temperatuer Sensor: PC1 : ADC1 Channel 11
		SoilHumid Sensor: PC2 : ADC1 Channel 12
		PhotoResistor Sensor: PC3: ADC1 Channel 13
	*/
OS_MUTEX ADCMutex;
OS_Q TempQ;
OS_Q HumidQ;
OS_Q PhotoQ;


OS_MEM Partition;
CPU_INT08U PartitionStorage[MEM_BLOCK_CNT][MEM_BLOCK_SIZE];

static  OS_TCB   AppTaskStartTCB;
static  OS_TCB   AppTaskTemperTCB;
static  OS_TCB   AppTaskSoilHumidTCB;
static  OS_TCB   AppTaskPhotoResistorTCB;
static  OS_TCB	 AppTaskSensorStaticTCB;
static  OS_TCB   AppTaskBlueToothTCB;
static	OS_TCB	 AppTaskPumpTCB;
static  OS_TCB	 AppTaskHeaterTCB;
static  OS_TCB   AppTaskLightTCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  CPU_STK  AppTaskTemperStk[APP_TASK_TEMPER_STK_SIZE];
static  CPU_STK  AppTaskSoilHumidStk[APP_TASK_SOILHUMID_STK_SIZE];
static  CPU_STK  AppTaskPhotoResistorStk[APP_TASK_PHOTORESISTOR_STK_SIZE];
static  CPU_STK	 AppTaskSensorStaticStk[APP_TASK_SENSORSTAT_STK_SIZE];
static  CPU_STK  AppTaskBlueToothStk[APP_TASK_BLUETOOTH_STK_SIZE];
static	CPU_STK	 AppTaskPumpStk[APP_TASK_PUMP_STK_SIZE];
static  CPU_STK	 AppTaskHeaterStk[APP_TASK_HEATER_STK_SIZE];
static  CPU_STK  AppTaskLightStk[APP_TASK_LIGHT_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskCreate (void);
static  void  AppObjCreate  (void);
static  void  AppTaskStart  (void *p_arg);
static  void  AppTaskTemper (void *p_arg);
static  void  AppTaskSoilHumid (void *p_arg);
static  void  AppTaskPhotoResistor( void *p_arg);
static  void  AppTaskSensorStatic(void *p_arg);
static  void  AppTaskBlueTooth(void *p_arg);
static  void  AppTaskPump(void *p_arg);
static  void  AppTaskHeater(void *p_arg);
static  void  AppTaskLight(void *p_arg);

void  ADC_ISR_Handler(void);
void  EXTI_ISR_Handler(void);

static  void  ADC_Configure(u8 ADC_Channel);
static  void  GPIO_Configure(void);
static  void  RCC_Configure(void);
static  void  Interrupt_Configure(void);
static  void  EXTI_Configure(void);

static char * dtoa(char *s, double n);
static float map(float x, float input_min, float input_max, float output_min, float output_max);
double MyAtof(char *p_pszStr);
//static float atof(char*s);
//https://gopae02.tistory.com/18
//

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err;


    BSP_IntDisAll();                                            /* Disable all interrupts.                              */

    OSInit(&err);  												/* Init uC/OS-III.                                      */
	/*Create Memory partition*/
	OSMemCreate(&Partition,"My Partition",(void*)&PartitionStorage[0][0],MEM_BLOCK_CNT,MEM_BLOCK_SIZE,&err);

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

		if (err != OS_ERR_NONE) {
    // 에러 처리 로직
    APP_TRACE_INFO(("err while creating task...\n\r"));
		}
    OSStart(&err);      		/* Start multitasking (i.e. give control to uC/OS-III). */
		
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
		CPU_CHAR buf[100];
		CPU_CHAR tmp[100];
		CPU_TS ts;
	CPU_CHAR* bluetoothblock;
   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr SysTick increments                     */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

    CPU_IntDisMeasMaxCurReset();

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
    BSP_Ser_Init(9600);                                       /* Enable Serial Interface                              */
	
#endif
    
    

	/*Configure*/
	RCC_Configure();
	GPIO_Configure();
	EXTI_Configure();
	Interrupt_Configure();
	
	BSP_LED_On(0);
		
		APP_TRACE_INFO(("Enter Temperature Threshold...\n\r"));
		BSP_Ser_RdStr(buf,100);
		BSP_Ser_RdByte();
		if(buf[0]!='\0')
			temperThreshold=(CPU_FP32)MyAtof(buf);
		BSP_LED_Off(1);
		
		APP_TRACE_INFO(("Enter SoilHumid Threshold...\n\r"));
		BSP_Ser_RdStr(buf,100);
		BSP_Ser_RdByte();
		if(buf[0]!='\0')
			soilHumidThreshold=(CPU_FP32)MyAtof(buf);
		BSP_LED_Off(2);
		
		APP_TRACE_INFO(("Enter Photoresistor Threshold...\n\r"));
		BSP_Ser_RdStr(buf,100);
		BSP_Ser_RdByte();
		if(buf[0]!='\0')
			photoThreshold=(CPU_FP32)MyAtof(buf);
		BSP_LED_Off(3);
		
		buf[0]='\0';
		Str_Cat(buf,"TemperThreshold: ");
		Str_Cat(buf,dtoa(tmp,temperThreshold));
		Str_Cat(buf," SoilHumidThreshold: ");
		Str_Cat(buf,dtoa(tmp,soilHumidThreshold));
		Str_Cat(buf," photoThreshold: ");
		Str_Cat(buf,dtoa(tmp,photoThreshold));
		Str_Cat(buf,"\n");
		
		APP_TRACE_INFO((buf));
		
	
	APP_TRACE_INFO(("Creating Application Events...\n\r"));
    AppObjCreate();
    APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
    AppTaskCreate();                                            /* Create Application Tasks                             */
    
                                                 /* Create Application Objects                           */
	BSP_LED_Off(0);
    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
				OSTimeDlyHMSM(0,1,0,0,OS_OPT_TIME_HMSM_STRICT,&err);
        //BSP_LED_Toggle(0);
				/*
        OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,&ts,&err);
				if(err==OS_ERR_NONE){
					bluetoothblock=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
					if(err==OS_ERR_NONE){
				
						bluetoothblock[0]='\0';
						Str_Cat(bluetoothblock,"Enter Temperature Threshold...\n");
						OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
						
						if(err!=OS_ERR_NONE) OSMemPut(&Partition,bluetoothblock,&err);
						
						if(err==OS_ERR_NONE){
							BSP_Ser_RdStr(buf,100);
							BSP_Ser_RdByte();
							if(buf[0]!='\0')
								temperThreshold=(CPU_FP32)MyAtof(buf);
						}
					}
					
					bluetoothblock=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
					if(err==OS_ERR_NONE){
				
						bluetoothblock[0]='\0';
						Str_Cat(bluetoothblock,"Enter SoilHumid Threshold...\n");
						OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
						
						if(err!=OS_ERR_NONE) OSMemPut(&Partition,bluetoothblock,&err);
						
						if(err==OS_ERR_NONE){
							BSP_Ser_RdStr(buf,100);
							BSP_Ser_RdByte();
							if(buf[0]!='\0')
								soilHumidThreshold=(CPU_FP32)MyAtof(buf);
						}
					}
					
					bluetoothblock=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
					if(err==OS_ERR_NONE){
				
						bluetoothblock[0]='\0';
						Str_Cat(bluetoothblock,"Enter PhotoResistor Threshold...\n");
						OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
						
						if(err!=OS_ERR_NONE) OSMemPut(&Partition,bluetoothblock,&err);
						
						if(err==OS_ERR_NONE){
							BSP_Ser_RdStr(buf,100);
							BSP_Ser_RdByte();
							if(buf[0]!='\0')
								photoThreshold=(CPU_FP32)MyAtof(buf);
						}
					}
					
					bluetoothblock=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
					if(err==OS_ERR_NONE){
				
						bluetoothblock[0]='\0';
						Str_Cat(bluetoothblock,"each value has been changed: \n");
						Str_Cat(bluetoothblock,dtoa(tmp,temperThreshold));
						Str_Cat(bluetoothblock," ");
						Str_Cat(bluetoothblock,dtoa(tmp,soilHumidThreshold));
						Str_Cat(bluetoothblock," ");
						Str_Cat(bluetoothblock,dtoa(tmp,photoThreshold));
						Str_Cat(bluetoothblock,"\n");
						OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
						
						if(err!=OS_ERR_NONE) OSMemPut(&Partition,bluetoothblock,&err);
						
						BSP_Ser_RdStr(buf,100);
						if(buf[0]!='\0')
						photoThreshold=(CPU_FP32)MyAtof(buf);
					}
				}
				*/
    }
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*
* Description:  This function creates the application tasks.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
    OS_ERR  err;
    OSTaskCreate((OS_TCB     *)&AppTaskTemperTCB, 
							 (CPU_CHAR   *)"App Temperature",
							 (OS_TASK_PTR )AppTaskTemper,
							 (void       *)0,
							 (OS_PRIO     )APP_TASK_TEMPER_PRIO,
							 (CPU_STK    *)&AppTaskTemperStk[0],
							 (CPU_STK_SIZE)APP_TASK_TEMPER_STK_SIZE / 10,
							 (CPU_STK_SIZE)APP_TASK_TEMPER_STK_SIZE,
							 (OS_MSG_QTY  )10,
							 (OS_TICK     )0,
							 (void       *)0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 (OS_ERR     *)&err);

    OSTaskCreate((OS_TCB     *)&AppTaskSoilHumidTCB, 
							 (CPU_CHAR   *)"App Soil Humid",
							 (OS_TASK_PTR )AppTaskSoilHumid,
							 (void       *)0,
							 (OS_PRIO     )APP_TASK_SOILHUMID_PRIO,
							 (CPU_STK    *)&AppTaskSoilHumidStk[0],
							 (CPU_STK_SIZE)APP_TASK_SOILHUMID_STK_SIZE / 10,
							 (CPU_STK_SIZE)APP_TASK_SOILHUMID_STK_SIZE,
							 (OS_MSG_QTY  )10,
							 (OS_TICK     )0,
							 (void       *)0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 (OS_ERR     *)&err);
    
    OSTaskCreate((OS_TCB     *)&AppTaskPhotoResistorTCB, 
							 (CPU_CHAR   *)"App PhotoResistor",
							 (OS_TASK_PTR )AppTaskPhotoResistor,
							 (void       *)0,
							 (OS_PRIO     )APP_TASK_PHOTORESISTOR_PRIO,
							 (CPU_STK    *)&AppTaskPhotoResistorStk[0],
							 (CPU_STK_SIZE)APP_TASK_PHOTORESISTOR_STK_SIZE / 10,
							 (CPU_STK_SIZE)APP_TASK_PHOTORESISTOR_STK_SIZE,
							 (OS_MSG_QTY  )10,
							 (OS_TICK     )0,
							 (void       *)0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 (OS_ERR     *)&err);

	OSTaskCreate((OS_TCB     *)&AppTaskSensorStaticTCB, 
							 (CPU_CHAR   *)"App SensorStatic",
							 (OS_TASK_PTR )AppTaskSensorStatic,
							 (void       *)0,
							 (OS_PRIO     )APP_TASK_SENSORSTAT_PRIO,
							 (CPU_STK    *)&AppTaskSensorStaticStk[0],
							 (CPU_STK_SIZE)APP_TASK_SENSORSTAT_STK_SIZE / 10,
							 (CPU_STK_SIZE)APP_TASK_SENSORSTAT_STK_SIZE,
							 (OS_MSG_QTY  )0,
							 (OS_TICK     )0,
							 (void       *)0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 (OS_ERR     *)&err);

    OSTaskCreate((OS_TCB     *)&AppTaskBlueToothTCB, 
							 (CPU_CHAR   *)"App Bluetooth",
							 (OS_TASK_PTR )AppTaskBlueTooth,
							 (void       *)0,
							 (OS_PRIO     )APP_TASK_BLUETOOTH_PRIO,
							 (CPU_STK    *)&AppTaskBlueToothStk[0],
							 (CPU_STK_SIZE)APP_TASK_BLUETOOTH_STK_SIZE / 10,
							 (CPU_STK_SIZE)APP_TASK_BLUETOOTH_STK_SIZE,
							 (OS_MSG_QTY  )30,
							 (OS_TICK     )0,
							 (void       *)0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 (OS_ERR     *)&err);

	OSTaskCreate((OS_TCB     *)&AppTaskPumpTCB, 
							 (CPU_CHAR   *)"App Pump",
							 (OS_TASK_PTR )AppTaskPump,
							 (void       *)0,
							 (OS_PRIO     )APP_TASK_PUMP_PRIO,
							 (CPU_STK    *)&AppTaskPumpStk[0],
							 (CPU_STK_SIZE)APP_TASK_PUMP_STK_SIZE / 10,
							 (CPU_STK_SIZE)APP_TASK_PUMP_STK_SIZE,
							 (OS_MSG_QTY  )0,
							 (OS_TICK     )0,
							 (void       *)0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 (OS_ERR     *)&err);

	OSTaskCreate((OS_TCB     *)&AppTaskHeaterTCB, 
							 (CPU_CHAR   *)"App Heater",
							 (OS_TASK_PTR )AppTaskHeater,
							 (void       *)0,
							 (OS_PRIO     )APP_TASK_HEATER_PRIO,
							 (CPU_STK    *)&AppTaskHeaterStk[0],
							 (CPU_STK_SIZE)APP_TASK_HEATER_STK_SIZE / 10,
							 (CPU_STK_SIZE)APP_TASK_HEATER_STK_SIZE,
							 (OS_MSG_QTY  )0,
							 (OS_TICK     )0,
							 (void       *)0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 (OS_ERR     *)&err);

	OSTaskCreate((OS_TCB     *)&AppTaskLightTCB, 
							 (CPU_CHAR   *)"App LightOn",
							 (OS_TASK_PTR )AppTaskLight,
							 (void       *)0,
							 (OS_PRIO     )APP_TASK_LIGHT_PRIO,
							 (CPU_STK    *)&AppTaskLightStk[0],
							 (CPU_STK_SIZE)APP_TASK_LIGHT_STK_SIZE / 10,
							 (CPU_STK_SIZE)APP_TASK_LIGHT_STK_SIZE,
							 (OS_MSG_QTY  )0,
							 (OS_TICK     )0,
							 (void       *)0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 (OS_ERR     *)&err);
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION EVENTS
*
* Description:  This function creates the application kernel objects.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{
	OS_ERR err;
	OSMutexCreate(&ADCMutex,"ADCMutex",&err);

	OSQCreate((OS_Q*)&TempQ,(CPU_CHAR*)"Temperature Queue",(OS_MSG_QTY)10,(OS_ERR*)&err);
	OSQCreate((OS_Q*)&HumidQ,(CPU_CHAR*)"SoilHumid Queue",(OS_MSG_QTY)10,(OS_ERR*)&err);
	OSQCreate((OS_Q*)&PhotoQ,(CPU_CHAR*)"Photo Queue",(OS_MSG_QTY)10,(OS_ERR*)&err);


	
}


/*
*********************************************************************************************************
*                                                AppTaskTemper
*********************************************************************************************************
*/

static void AppTaskTemper(void* p_arg)
{
	CPU_INT32U value;
	CPU_FP32 temperature;
	OS_MSG_SIZE size;
	OS_ERR err;
	CPU_TS ts;
	CPU_CHAR* bluetoothblock;
	CPU_CHAR* bluetoothblock2;
	CPU_CHAR temper_temp[100];
	CPU_FP32* floatvalueblock;
	//OSTaskSemSet(&AppTaskTemperTCB,1,&err);
	while(DEF_ON){
		//APP_TRACE_INFO(("AppTaskTemper..."));
		BSP_LED_Toggle(1);
		OSMutexPend((OS_MUTEX*)&ADCMutex,(OS_TICK)0,(OS_OPT)OS_OPT_PEND_BLOCKING,(CPU_TS*)&ts,(OS_ERR*)&err);
		ADC_Configure(ADC_Channel_11);

		value = (CPU_INT32U)OSTaskQPend((OS_TICK)0,(OS_OPT)OS_OPT_PEND_BLOCKING,&size,(CPU_TS*)&ts, (OS_ERR*)&err);
		//APP_TRACE_INFO(("AppTaskTemper.2.."));
		OSMutexPost((OS_MUTEX*)&ADCMutex,(OS_OPT)OS_OPT_POST_NONE,(OS_ERR*)&err);

		temperature = ((CPU_FP32)value*3.3/4096.0)*100.0; /* 수정 필요*/
		//temperature=value;
		
		
		
		bluetoothblock=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
		if(err==OS_ERR_NONE){
			bluetoothblock[0]='\0';
			Str_Cat(bluetoothblock,"temperature: ");
			Str_Cat(bluetoothblock,dtoa(temper_temp,temperature));
			Str_Cat(bluetoothblock,"\n");
			
			OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
			if(err!=OS_ERR_NONE){
						OSMemPut(&Partition,bluetoothblock,&err);
			}
			
		}
		
		floatvalueblock=(CPU_FP32*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
		if(err==OS_ERR_NONE){
			floatvalueblock[0]=temperature;
			OSQPost((OS_Q*)&TempQ,(void*)floatvalueblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
			if(err!=OS_ERR_NONE){
				OSMemPut(&Partition,(void*)floatvalueblock,&err);
			}
		}
		
		
		if(temperature<temperThreshold){
			OSTaskSemPend(0,OS_OPT_PEND_NON_BLOCKING,&ts,&err);
			if(err==OS_ERR_NONE){
				bluetoothblock2=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
				if(err==OS_ERR_NONE){
					
						bluetoothblock2[0]='\0';
						Str_Cat(bluetoothblock2,"temperature is too low: ");
						Str_Cat(bluetoothblock2,dtoa(temper_temp,temperature));
						Str_Cat(bluetoothblock2,"\n");
					
						OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock2,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_LIFO,(OS_ERR*)&err);
					if(err!=OS_ERR_NONE){
						OSMemPut(&Partition,bluetoothblock2,&err);
					}
					
						OSTaskSemPost(&AppTaskHeaterTCB,OS_OPT_POST_NONE,&err);
					  //APP_TRACE_INFO(("too low temp info\r\n"));
					
				}
				else{
					//APP_TRACE_INFO(("something wrong with memory allocation..."));
				}
			}
			
		}
	//APP_TRACE_INFO(("AppTaskTemper.4.."));
		
		OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}

/*
*********************************************************************************************************
*                                                AppTaskSoilHumid
*********************************************************************************************************
*/

static void AppTaskSoilHumid(void* p_arg)
{
	CPU_INT32U value;
	CPU_FP32 SoilHumid;
	OS_MSG_SIZE size;
	OS_ERR err;
	CPU_TS ts;
	
	CPU_CHAR* bluetoothblock;
	CPU_CHAR* bluetoothblock2;
	CPU_FP32* floatvalueblock;
	CPU_CHAR soil_temp[100];
	//OSTaskSemSet(&AppTaskSoilHumidTCB,1,&err);
	while(DEF_ON){
		BSP_LED_Toggle(2);
		OSMutexPend((OS_MUTEX*)&ADCMutex,(OS_TICK)0,(OS_OPT)OS_OPT_PEND_BLOCKING,(CPU_TS*)&ts,(OS_ERR*)&err);
		ADC_Configure(ADC_Channel_12);

		value = (CPU_INT32U)OSTaskQPend((OS_TICK)0,(OS_OPT)OS_OPT_PEND_BLOCKING,&size,(CPU_TS*)&ts, (OS_ERR*)&err);
		OSMutexPost((OS_MUTEX*)&ADCMutex,(OS_OPT)OS_OPT_POST_NONE,(OS_ERR*)&err);

		SoilHumid = map((CPU_FP32)value,400.0,4096.0,100.0,0.0); 
		
		floatvalueblock=(CPU_FP32*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
		if(err==OS_ERR_NONE){
			floatvalueblock[0]=SoilHumid;
			OSQPost((OS_Q*)&HumidQ,(void*)floatvalueblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
			if(err!=OS_ERR_NONE){
				//APP_TRACE_INFO(("Something went wrong\r\n"));
				OSMemPut(&Partition,floatvalueblock,&err);
			}
		}
		
		
		bluetoothblock=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
		if(err==OS_ERR_NONE){
			
						bluetoothblock[0]='\0';
						Str_Cat(bluetoothblock,"SoilHumid: ");
						Str_Cat(bluetoothblock,dtoa(soil_temp,SoilHumid));
						Str_Cat(bluetoothblock,"\n");
			
			OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
			if(err!=OS_ERR_NONE){
						OSMemPut(&Partition,bluetoothblock,&err);
			}
		}
		
		if(SoilHumid<soilHumidThreshold){
			OSTaskSemPend(0,OS_OPT_PEND_NON_BLOCKING,&ts,&err);
			if(err==OS_ERR_NONE){
				bluetoothblock2=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
				
				
				
				//APP_TRACE_INFO(("too low soil info2\r\n"));
				if(err==OS_ERR_NONE){
					bluetoothblock2[0]='\0';
						Str_Cat(bluetoothblock2,"warning: SoilHumid is too low: ");
						Str_Cat(bluetoothblock2,dtoa(soil_temp,SoilHumid));
						Str_Cat(bluetoothblock2,"\n");
					OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock2,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_LIFO,(OS_ERR*)&err);
					if(err!=OS_ERR_NONE){
						OSMemPut(&Partition,bluetoothblock2,&err);
					}
					OSTaskSemPost(&AppTaskPumpTCB,OS_OPT_POST_NONE,&err);
					
					//APP_TRACE_INFO(("too low soil info\r\n"));
				}
				else{
					//APP_TRACE_INFO(("something wrong with memory allocation..."));
				}
				
			}
			
		}
		
		
		OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}

/*
*********************************************************************************************************
*                                                AppTaskPhotoResistor
*********************************************************************************************************
*/

static void AppTaskPhotoResistor(void* p_arg)
{
	CPU_INT32U value;
	CPU_FP32 photoresistor;
	OS_MSG_SIZE size;
	OS_ERR err;
	CPU_TS ts;
	CPU_CHAR* bluetoothblock;
	CPU_CHAR* bluetoothblock2;
	CPU_FP32* floatvalueblock;
	CPU_CHAR photo_temp[100];
	//OSTaskSemSet(&AppTaskPhotoResistorTCB,1,&err);
	while(DEF_ON){
		BSP_LED_Toggle(3);
		OSMutexPend((OS_MUTEX*)&ADCMutex,(OS_TICK)0,(OS_OPT)OS_OPT_PEND_BLOCKING,(CPU_TS*)&ts,(OS_ERR*)&err);
		ADC_Configure(ADC_Channel_13);

		value = (CPU_INT32U)OSTaskQPend((OS_TICK)0,(OS_OPT)OS_OPT_PEND_BLOCKING,&size,(CPU_TS*)&ts, (OS_ERR*)&err);
		OSMutexPost((OS_MUTEX*)&ADCMutex,(OS_OPT)OS_OPT_POST_NONE,(OS_ERR*)&err);

		photoresistor = (CPU_FP32)value;
		
		
		floatvalueblock=(CPU_FP32*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
		if(err==OS_ERR_NONE){
			floatvalueblock[0]=photoresistor;
			OSQPost((OS_Q*)&PhotoQ,(void*)floatvalueblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
			if(err!=OS_ERR_NONE){
				OSMemPut(&Partition,floatvalueblock,&err);
			}
		}
		
		
		bluetoothblock=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
		if(err==OS_ERR_NONE){
			bluetoothblock[0]='\0';
			Str_Cat(bluetoothblock,"photoresistor: ");
			Str_Cat(bluetoothblock,dtoa(photo_temp,photoresistor));
			Str_Cat(bluetoothblock,"\n");
			
			OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
			if(err!=OS_ERR_NONE){
						OSMemPut(&Partition,bluetoothblock,&err);
					}
		}
		
		if(photoresistor>photoThreshold){
			OSTaskSemPend(0,OS_OPT_PEND_NON_BLOCKING,&ts,&err);
			if(err==OS_ERR_NONE){
				bluetoothblock2=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
				//APP_TRACE_INFO(("too lowlight2\r\n"));
				if(err==OS_ERR_NONE){
					bluetoothblock2[0]='\0';
					Str_Cat(bluetoothblock2,"warning: too dark : ");
			Str_Cat(bluetoothblock2,dtoa(photo_temp,photoresistor));
			Str_Cat(bluetoothblock2,"\n");
					
				OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock2,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_LIFO,(OS_ERR*)&err);
					if(err!=OS_ERR_NONE){
						OSMemPut(&Partition,bluetoothblock2,&err);
					}
				OSTaskSemPost(&AppTaskLightTCB,OS_OPT_POST_NONE,&err);
					//APP_TRACE_INFO(("too lowlight\r\n"));
				}
				
			}
			
		}
		
		
		OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}

/*
*********************************************************************************************************
*                                                AppTaskSensorStatic
*********************************************************************************************************
*/
/*
*/

static  void  AppTaskSensorStatic(void *p_arg)
{
	
	OS_MSG_SIZE size;
	CPU_TS ts;
	OS_ERR err;
	CPU_FP32 temperSum=0.0;
	CPU_FP32 humidSum=0.0;
	CPU_FP32 photoSum=0.0;

	CPU_INT32U tempercnt=0;
	CPU_INT32U humidcnt=0;
	CPU_INT32U photocnt=0;

	CPU_FP32* floatvalueblock;

	CPU_BOOLEAN temperFlag=FALSE;
	CPU_BOOLEAN humidFlag=FALSE;
	CPU_BOOLEAN photoFlag=FALSE;

	CPU_CHAR* bluetoothblock;
	CPU_CHAR tmp[100];
	while(DEF_TRUE)
	{
		floatvalueblock=(CPU_FP32*)OSQPend((OS_Q*)&TempQ,(OS_TICK)0,(OS_OPT)OS_OPT_PEND_NON_BLOCKING,(OS_MSG_SIZE*)&size,(CPU_TS*)&ts,(OS_ERR*)&err);
		if(err==OS_ERR_NONE){
			temperSum+=floatvalueblock[0];
			tempercnt++;
			if(tempercnt>0 && tempercnt%5==0){
				temperFlag=TRUE;
			}
			OSMemPut((OS_MEM*)&Partition,(void*)floatvalueblock,&err);
		}
		//APP_TRACE_INFO(("stat1\r\n"));
		floatvalueblock=(CPU_FP32*)OSQPend((OS_Q*)&HumidQ,(OS_TICK)0,(OS_OPT)OS_OPT_PEND_NON_BLOCKING,(OS_MSG_SIZE*)&size,(CPU_TS*)&ts,(OS_ERR*)&err);
		if(err==OS_ERR_NONE){
			humidSum+=floatvalueblock[0];
			humidcnt++;
			if(humidcnt>0 && humidcnt%5==0){
				humidFlag=TRUE;
			}
			OSMemPut((OS_MEM*)&Partition,(void*)floatvalueblock,&err);
		}
		floatvalueblock=(CPU_FP32*)OSQPend((OS_Q*)&PhotoQ,(OS_TICK)0,(OS_OPT)OS_OPT_PEND_NON_BLOCKING,(OS_MSG_SIZE*)&size,(CPU_TS*)&ts,(OS_ERR*)&err);
		if(err==OS_ERR_NONE){
			photoSum+=floatvalueblock[0];
			photocnt++;
			if(photocnt>0 && photocnt%5==0){
				photoFlag=TRUE;
			}
			OSMemPut((OS_MEM*)&Partition,(void*)floatvalueblock,&err);
		}

		if(temperFlag && humidFlag && photoFlag){
			bluetoothblock=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
			if(err==OS_ERR_NONE){
				
				bluetoothblock[0]='\0';
				Str_Cat(bluetoothblock,"temperAvg : ");
			Str_Cat(bluetoothblock,dtoa(tmp,temperSum/tempercnt));
			Str_Cat(bluetoothblock,", humidAvg : ");
				Str_Cat(bluetoothblock,dtoa(tmp,humidSum/humidcnt));
				Str_Cat(bluetoothblock,", photoAvg : ");
				Str_Cat(bluetoothblock,dtoa(tmp,photoSum/photocnt));
				Str_Cat(bluetoothblock,"\n");
				//APP_TRACE_INFO((bluetoothblock));
				//sprintf(bluetoothblock,"TemperAvg: %f, humidAvg: %f, photoResistorAvg: %f\n\r",temperSum/tempercnt,humidSum/humidcnt,photoSum/photocnt);
				OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
				if(err!=OS_ERR_NONE){
					OSMemPut(&Partition,bluetoothblock,&err);
				}
				temperFlag=FALSE;
				humidFlag=FALSE;
				photoFlag=FALSE;
			}
			
		}
		if(tempercnt==1000000){
			temperSum=0.0;
			tempercnt=0;
			temperFlag=FALSE;
		}
		if(humidcnt==1000000){
			humidSum=0.0;
			humidcnt=0;
			humidFlag=FALSE;
		}
		if(photocnt==1000000){
			photoSum=0.0;
			photocnt=0;
			photoFlag=FALSE;
		}
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
	}

/*
OS_ERR err;
OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err);
*/
}

/*
*********************************************************************************************************
*                                                AppTaskBlueTooth
*********************************************************************************************************
*/

static void  AppTaskBlueTooth(void* p_arg)
{
	CPU_CHAR* bluetoothblock=NULL;
	OS_ERR err;
	OS_MSG_SIZE size;
	CPU_TS ts;
	int i;
	while(DEF_TRUE){
		bluetoothblock=(CPU_CHAR*)OSTaskQPend(0,(OS_OPT)OS_OPT_PEND_BLOCKING,(OS_MSG_SIZE*)&size,(CPU_TS*)&ts,(OS_ERR*)&err);
		
		
		if(bluetoothblock!=NULL && err==OS_ERR_NONE){
			/*
			for(i=0;bluetoothblock[i]!='\0';++i){
				BSP_Ser_WrByte(bluetoothblock[i]);//this method uses interrupt for TC.
				
				//this method is polling. not recommend.
				//USART_SendData(USART2,bluetoothblock[i]);
			//while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
			//USART_ClearITPendingBit(USART2, USART_IT_TC);
        	//USART_ClearFlag(USART2, USART_IT_TC);
				
			//OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);
		}
			*/
			BSP_Ser_WrStr(bluetoothblock);
		OSMemPut((OS_MEM*)&Partition,(void*)bluetoothblock,&err);
		bluetoothblock=NULL;
		}

	}
	OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
}

/*
*********************************************************************************************************
*                                                AppTaskHeater
*********************************************************************************************************
*/

static void AppTaskHeater(void* p_arg)
{
	//heater connected to PD12
	OS_ERR err;
	CPU_TS ts;
	CPU_CHAR* bluetoothblock;
	OSTaskSemPost(&AppTaskTemperTCB,OS_OPT_NONE,&err);
	while(DEF_ON){
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,&ts,&err);
		if(err==OS_ERR_NONE){
			//GPIOD->BSRR|=GPIO_BSRR_BS12;
			//GPIO_SetBits(GPIOD,GPIO_Pin_12);
			GPIO_ResetBits(GPIOD,GPIO_Pin_12);
			bluetoothblock=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
			bluetoothblock[0]='\0';
			Str_Cat(bluetoothblock,"Heater ON!\n");
			OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
			
			if(err!=OS_ERR_NONE) OSMemPut(&Partition,bluetoothblock,&err);
			
			OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err);
		
		//GPIOD->BRR|=GPIO_BRR_BR12;
		//GPIO_ResetBits(GPIOD,GPIO_Pin_12);
			GPIO_SetBits(GPIOD,GPIO_Pin_12);
		//APP_TRACE_INFO(("Heater off!\r\n"));
		OSTaskSemPost(&AppTaskTemperTCB,OS_OPT_NONE,&err);
		}
		
	}
}

/*
*********************************************************************************************************
*                                                AppTaskPump
*********************************************************************************************************
*/

static void AppTaskPump(void* p_arg)
{
	//Pump connected to PD13
	OS_ERR err;
	CPU_TS ts;
	CPU_CHAR* bluetoothblock;
	OSTaskSemPost(&AppTaskSoilHumidTCB,OS_OPT_NONE,&err);
	while(DEF_ON){
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,&ts,&err);
		if(err==OS_ERR_NONE){
			//GPIOD->BSRR|=GPIO_BSRR_BS13;
			GPIO_SetBits(GPIOD,GPIO_Pin_13);
			bluetoothblock=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
			bluetoothblock[0]='\0';
			Str_Cat(bluetoothblock,"Pump ON!\n");
			OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
			
			if(err!=OS_ERR_NONE) OSMemPut(&Partition,bluetoothblock,&err);
			
			OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err);
		//GPIOD->BRR|=GPIO_BRR_BR13;
		GPIO_ResetBits(GPIOD,GPIO_Pin_13);
		OSTaskSemPost(&AppTaskSoilHumidTCB,OS_OPT_NONE,&err);
		}
		
	}
}

/*
*********************************************************************************************************
*                                                AppTaskLight
*********************************************************************************************************
*/

static void AppTaskLight(void* p_arg)
{
	//LED connected to PD9,PD10,PD11
	OS_ERR err;
	CPU_TS ts;
	CPU_CHAR* bluetoothblock;
	OSTaskSemPost(&AppTaskPhotoResistorTCB,OS_OPT_NONE,&err);
	while(DEF_ON){
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,&ts,&err);
		if(err==OS_ERR_NONE){
			
			//GPIOD->BSRR|=GPIO_BSRR_BS9;
			//GPIOD->BSRR|=GPIO_BSRR_BS10;
			//GPIOD->BSRR|=GPIO_BSRR_BS11;
			GPIO_SetBits(GPIOD,GPIO_Pin_9);
			GPIO_SetBits(GPIOD,GPIO_Pin_10);
			GPIO_SetBits(GPIOD,GPIO_Pin_11);
			GPIO_SetBits(GPIOE,GPIO_Pin_10);
			GPIO_SetBits(GPIOE,GPIO_Pin_11);
			
			bluetoothblock=(CPU_CHAR*)OSMemGet((OS_MEM*)&Partition,(OS_ERR*)&err);
			bluetoothblock[0]='\0';
			Str_Cat(bluetoothblock,"LED ON!\n");
			OSTaskQPost((OS_TCB*)&AppTaskBlueToothTCB,(void*)bluetoothblock,(OS_MSG_SIZE)MEM_BLOCK_SIZE,(OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err);
			
			if(err!=OS_ERR_NONE) OSMemPut(&Partition,bluetoothblock,&err);
			
			OSTimeDlyHMSM(0,1,0,0,OS_OPT_TIME_HMSM_STRICT,&err);
		//GPIOD->BRR=GPIO_BRR_BR9;
		//GPIOD->BRR=GPIO_BRR_BR10;
		//GPIOD->BRR=GPIO_BRR_BR11;
		
		GPIO_ResetBits(GPIOD,GPIO_Pin_9);
		GPIO_ResetBits(GPIOD,GPIO_Pin_10);
		GPIO_ResetBits(GPIOD,GPIO_Pin_11);
		GPIO_ResetBits(GPIOE,GPIO_Pin_10);
		GPIO_ResetBits(GPIOE,GPIO_Pin_11);
		
		OSTaskSemPost(&AppTaskPhotoResistorTCB,OS_OPT_NONE,&err);
		}
		
	}
}

/*
*********************************************************************************************************
*                                                ADC_ISR_Handler
*********************************************************************************************************
*/
 void  ADC_ISR_Handler(void)
{
	CPU_INT16U value;
	OS_ERR err;
	/*
		Temperatuer Sensor: PC1 : ADC1 Channel 11
		SoilHumid Sensor: PC2 : ADC1 Channel 12
		PhotoResistor Sensor: PC3: ADC1 Channel 13
	*/
	//APP_TRACE_INFO(("ADC_ISR_Handler start...\n\r"));
	if(ADC_GetITStatus(ADC1,ADC_IT_EOC) != RESET) { 
			//APP_TRACE_INFO(("ADC_ISR_Handler start EOC...\n\r"));
    	value = ADC_GetConversionValue(ADC1); 
		if(ADC1->SQR3 == ADC_Channel_11){
			OSTaskQPost(&AppTaskTemperTCB,(void*)value,(OS_MSG_SIZE)sizeof(value),(OS_OPT)OS_OPT_POST_FIFO,&err);
		}
		else if(ADC1->SQR3==ADC_Channel_12){
			OSTaskQPost(&AppTaskSoilHumidTCB,(void*)value,(OS_MSG_SIZE)sizeof(value),(OS_OPT)OS_OPT_POST_FIFO,&err);
		}
		else{
			OSTaskQPost(&AppTaskPhotoResistorTCB,(void*)value,(OS_MSG_SIZE)sizeof(value),(OS_OPT)OS_OPT_POST_FIFO,&err);
		}
  
  		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
  	}

	
}
/*
*********************************************************************************************************
*                                                EXTI_ISR_Handler
*********************************************************************************************************
*/
void  EXTI_ISR_Handler(void){
	OS_ERR err;
	 if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
      if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == Bit_RESET) {
         OSTaskSemPost(&AppTaskStartTCB,OS_OPT_POST_NONE,&err);
      }
        EXTI_ClearITPendingBit(EXTI_Line4);
   }
}

/*
*********************************************************************************************************
*                                                ADC_Configure(u8 ADC_Channel)
*********************************************************************************************************
*/
static void ADC_Configure(u8 ADC_Channel){
	//APP_TRACE_INFO(("ADC configure start...\n\r"));
	ADC_InitTypeDef ADC_12; 
	
	ADC_12.ADC_ContinuousConvMode = DISABLE; 
    ADC_12.ADC_DataAlign = ADC_DataAlign_Right; 
    ADC_12.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_12.ADC_Mode = ADC_Mode_Independent; 
    ADC_12.ADC_NbrOfChannel = 1; 
    ADC_12.ADC_ScanConvMode = DISABLE;
    
	ADC_Init(ADC1, &ADC_12); 
    ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_239Cycles5);
    ADC_ITConfig(ADC1,ADC_IT_EOC, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

//APP_TRACE_INFO(("ADC configure end...\n\r"));

}
/*
*********************************************************************************************************
*                                                GPIO_Configure()
*********************************************************************************************************
*/
static void GPIO_Configure(void){
	GPIO_InitTypeDef GPIO_InitStructure;

//for sensor
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);


//for LED and Heater, Pump
	GPIO_InitTypeDef GPIO_InitStructure2;
	GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14; 
    GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOD, &GPIO_InitStructure2);
	
	GPIO_ResetBits(GPIOD,GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);
	
	GPIO_InitTypeDef GPIO_InitStructure3;
	
	GPIO_InitStructure3.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11; 
    GPIO_InitStructure3.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure3.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOE, &GPIO_InitStructure3);
		
		//for button
		GPIO_InitTypeDef GPIO_InitStructure4;
		
		GPIO_InitStructure4.GPIO_Pin=GPIO_Pin_4;
		GPIO_InitStructure4.GPIO_Mode=GPIO_Mode_IPU;
		GPIO_InitStructure4.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOC,&GPIO_InitStructure4);
		
	GPIO_SetBits(GPIOD,GPIO_Pin_12);
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_10|GPIO_Pin_11);
}
/*
*********************************************************************************************************
*                                                RCC_Configure()
*********************************************************************************************************
*/
static void RCC_Configure(void){
	
	//BSP_PeriphEn(BSP_PERIPH_ID_USART2);
	//BSP_PeriphEn(BSP_PERIPH_ID_IOPC);
	//BSP_PeriphEn(BSP_PERIPH_ID_IOPD);
	//BSP_PeriphEn(BSP_PERIPH_ID_ADC1);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
}

/*
*********************************************************************************************************
*                                                Interrupt_Configure()
*********************************************************************************************************
*/

static void Interrupt_Configure(){
	//이게 안되면 평범하게 NVIC로
	//BSP_IntPrioSet(BSP_INT_ID_USART2,2);
	BSP_IntVectSet(BSP_INT_ID_ADC1_2, ADC_ISR_Handler);
  BSP_IntEn(BSP_INT_ID_ADC1_2);
	BSP_IntPrioSet(BSP_INT_ID_ADC1_2,1);
	BSP_IntVectSet(BSP_INT_ID_EXTI4,EXTI_ISR_Handler);
	//BSP_IntEn(BSP_INT_ID_EXTI4);
	//BSP_IntPrioSet(BSP_INT_ID_EXTI4,2);
	
	
}
/*
*********************************************************************************************************
*                                                EXTI_Configure()
*********************************************************************************************************
*/
static void EXTI_Configure(){
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

//https://copyprogramming.com/howto/c-convert-string-to-float-f-code-example
char * dtoa(char *s, double n) {
    // handle special cases
     if (n == 0.0) {
        Str_Copy(s, "0");
    } else {
        int digit, m, m1;
        char *c = s;
        int neg = (n < 0);
        if (neg)
            n = -n;
        // calculate magnitude
        m = log10(n);
        int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
        if (neg)
            *(c++) = '-';
        // set up for scientific notation
        if (useExp) {
            if (m < 0)
               m -= 1.0;
            n = n / pow(10.0, m);
            m1 = m;
            m = 0;
        }
        if (m < 1.0) {
            m = 0;
        }
        // convert the number
        while (n > PRECISION || m >= 0) {
            double weight = pow(10.0, m);
            if (weight > 0) {
                digit = floor(n / weight);
                n -= (digit * weight);
                *(c++) = '0' + digit;
            }
            if (m == 0 && n > 0)
                *(c++) = '.';
            m--;
        }
        if (useExp) {
            // convert the exponent
            int i, j;
            *(c++) = 'e';
            if (m1 > 0) {
                *(c++) = '+';
            } else {
                *(c++) = '-';
                m1 = -m1;
            }
            m = 0;
            while (m1 > 0) {
                *(c++) = '0' + m1 % 10;
                m1 /= 10;
                m++;
            }
            c -= m;
            for (i = 0, j = m-1; i<j; i++, j--) {
                // swap without temporary
                c[i] ^= c[j];
                c[j] ^= c[i];
                c[i] ^= c[j];
            }
            c += m;
        }
        *(c) = '\0';
    }
    return s;
}
//https://blog.naver.com/aul-_-/221485012562
static float map(float x, float input_min, float input_max, float output_min, float output_max){
	return (x-input_min)*(output_max-output_min)/(input_max-input_min)+output_min;
}
//https://gopae02.tistory.com/18
double MyAtof(char *p_pszStr)
	{
		double dTotal, dTmp;
		int i;
		for(i =0, dTotal =dTmp =0; p_pszStr[i] != '\0' ; ++i){
			if (!dTmp){
				dTotal *=10;
				dTotal +=p_pszStr[i] -'0';
					if (p_pszStr[i +1] =='.'){
						dTmp =0.1;
						++i;
					}
			}
			else{
				dTotal += dTmp* (p_pszStr[i] -'0');
				dTmp *=0.1;
			}
		}
		return dTotal;
	}