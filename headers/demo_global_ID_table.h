#ifndef DEMO_GLOBAL_ID_TABLE__H
#define DEMO_GLOBAL_ID_TABLE__H


enum demoID_devPhy{
	FINDCoC=2001,
	TSNSw1=2011,
	TSNSw2=2012,
	TSNSw3=2013,
	TSNSw4=2014,
	NUC1=2050,
	NUC2=2051,
	cam=2060,
	VScr=2061,
	//---------------
	DFKIEng=2129,
	//---------------
	BoschPLC1=2130,
	BoschPLC2=2131,
	SercosDr1=2140,
	SercorDr2=2141,
	BoschiPC1=2150,
	BoschiPC2=2154,
	BoschEng1=2158,
	BoschEng2=2159,
	//---------------
	FestoCECCX=2160,
	FestoCTEU_A=2170,
	FestoCTEU_B=2171,
	//---------------
	FestoEng=2189,
	HMSGW1=2190,
	HMSGW2=2191,
	HMSEng=2219,
	EtherCATPLC=2400,
	EtherCATIO_dev=2402,
	EtherNetIPPLC=2500,
	CANo_dev1=2602,
	CANo_dev2=2603,
	CANo_dev3=2604
};

enum demoID_devSubnet{
	FINDCoC=3001,
	TSNSw1=3011,
	TSNSw2=3012,
	TSNSw3=3013,
	TSNSw4=3014,
	NUC1=3050	,
	NUC2=3051,
	cam=3060,
	VScr=3061,
	//---------------
	DFKIEng=3129,
	//---------------
	BoschPLC1_1=3130,
		BoschPLC1_S3=3300,
	BoschPLC2_1=3131,
		BoschPLC2_S3=3301,
	SercosDr1_1=3140,
		SercorDr1_S3=3310,
	SercosDr2_1=3141,
		SercorDr2_S3=3311,
	BoschiPC1=3150,
	BoschiPC2=3154,
	BoschEng1=3158,
	BoschEng2=3159,
	//---------------
	FestoCECCX=3160,
	FestoCTEU_A=3170,
	FestoCTEU_B=3171,
	//---------------
	FestoEng=3189,
	HMSGW1_TSN=3190,
		HMSGW1_EC=3401,
		HMSGW1_CO=3600,
	HMSGW2_TSN=3191,
		HMSGW2_EIP=3501,
		HMSGW2_CO=3601,
	HMSEng=3219,
	EtherCATPLC=3400,
	EtherCATIO_dev=3402,
	EtherNetIPPLC=3500,
	CANo_dev1=3602,
	CANo_dev2=3603,
	CANo_dev3=3604
};

enum demoID_AR{
	Bosch_SercosOverTSN1=5000,
	Bosch_SercosOverTSN2=5010,
	Bosch_C2C1=5020,
	Bosch_C2C2=5030,
	Festo_AR1=5100,
	Festo_AR2=5110,
	HMS_Anybus=5200,
	HMS_CANopen=5210
};

enum demoID_CR{
	Bosch_PLC1_Drive1=6000,
	Bosch_Drive1_PLC1=6001,
	Bosch_PLC2_Drive2=6010,
	Bosch_Drive2_PLC2=6011,
	Bosch_PLC1_PLC2=6020,
	Bosch_PLC2_PLC1=6030,
	Festo_CR1=6100,
	Festo_CR2=6110,
	HMS_EC_EIP=6200,
	HMS_EIP_EC=6201,
	HMS_CANo_Sync=6210
};

enum demoID_Subnet{
	TSN=1000,
	Sercos_1=1300,
	Sercos_2=1301,
	EtherCAT=1400,
	EtherNetIP=1500,
	CANopen=1600
};

enum demoID_SubnetStream{
	s3130_3140=7000,
	s3140_3130=7001,
	s3131_3141=7010,
	s3141_3131=7011,
	s3130_3131=7020,
	s3131_3130=7030,
	s3160_3170=7100,
	s3160_3171=7110,
	s3190_3191=7200,
	s3191_3190=7201
};


#endif /* DEMO_GLOBAL_ID_TABLE__H */
