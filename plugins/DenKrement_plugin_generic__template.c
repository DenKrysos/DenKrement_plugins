/*
 * DenKrement_plugin_generic__template.c
 *
 *  Created on: 24.07.2018
 *      Author: Dennis Krummacker
 */



//==================================================================================================//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------//
//--------  Preamble, Inclusions  ------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------//
//==================================================================================================//
//==================================================================================================//
// At first include this ----------------------------------------------------------
//---------------------------------------------------------------------------------
#include "./global/global_settings.h"
#include "./global/global_variables.h"
// Need this for e.g. some really fundamental Program Basics (if any)  ------------
//---------------------------------------------------------------------------------
//#include "DenKrement_plugins_base.h"
// Then better start with this  ---------------------------------------------------
//---------------------------------------------------------------------------------
//#include "DenKrement_plugins.h"
//==================================================================================================//
//==================================================================================================//
//==================================================================================================//
//Just to nicely keep order:  -----------------------------------------------------
//   First include the System / Third-Party Headers  ------------------------------
//   Format: Use <NAME> for them  -------------------------------------------------
//---------------------------------------------------------------------------------
//#include <limits.h>
//#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h>
//#include <pcap.h>
//#include <malloc.h>
//==================================================================================================//
//==================================================================================================//
//==================================================================================================//
//Then include own Headers  -------------------------------------------------------
//   Format: Use "NAME" for them  -------------------------------------------------
//---------------------------------------------------------------------------------
#include "DenKr_essentials/PreC/DenKr_PreC.h"
#include "DenKr_essentials/auxiliary.h"
//#include "DenKr_essentials/DenKr_errno.h"
#include "DenKr_essentials/Plugins/DL_Libs.h"
#include "DenKr_essentials/multi_threading.h"
//#include "nl80211.h"
//---------------------------------------------------------------------------------
#include "plugins_export.h"
#include "headers/DenKrement_plugin_generic__template.h"
#include "headers/DenKrement_plugin_generic__PolicyManager.h"
//==================================================================================================//
//==================================================================================================//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------//
//==================================================================================================//


#ifdef DEBUG
	#define DEBUG_module_generic__template
#endif


#define DISABLE_DEPR




// The "Content Prefix":
//    Put this in Front of every Function, Variable, simply every Symbol, that will
//    be visible outside, to make it unique and thus avoid clashes.
//  Set it identical to the File-Name.
#define C_PREF DenKrement_plugin_generic__template



void CONCAT(C_PREF, _settings_init())
{

}


void CONCAT(C_PREF, _gui_window_init())
{

}







//Example: Callback, general one, gets Data (registered as Consumer)
void* CAT(C_PREF,_InfBrok_cb_gear)(void* arg)
{
	#ifdef DEBUG_module_generic__template
		puts("");
		printfc(gray,"DEBUG");printfc(DENKR__COLOR_ANSI__DL_LIBS," (Plugin)");printfc(gray,":");printfc(gray," Callback called:");printf(" %s_InfBrok_cb_gear\n",STRINGIFY(C_PREF));
	#endif
	DenKr_InfBroker_msg* msg;
	DenKr_ContextBroker_parsemsg_callback(arg,&msg);
	#ifdef DEBUG_module_generic__template
		DenKr_ContextBroker_Debug_printmsg(msg);
	#endif
	DenKr_ContextBroker_free_msg(msg);
	return NULL;
}

//Example: Callback, reply to a request. (Registered as Producer)
	//I know... that is dirty...
	//  But to force the InfBroker to be able to handle variadic Callback Arguments isn't nice either...
	DenKr_InfBroker_Iface_Client* CAT(C_PREF,_InfBrok_cb_request_InfBrokerIface);
void* CAT(C_PREF,_InfBrok_cb_request)(void* arg)
{
	#ifdef DEBUG_module_generic__template
		puts("");
		printfc(gray,"DEBUG");printfc(DENKR__COLOR_ANSI__DL_LIBS," (Plugin)");printfc(gray,":");printfc(gray," Callback called:");printf(" (Information-Request) %s_InfBrok_cb_request\n",STRINGIFY(C_PREF));
	#endif
	DenKr_InfBroker_msg* msg;
	DenKr_ContextBroker_parsemsg_callback(arg,&msg);
	#ifdef DEBUG_module_generic__template
		DenKr_ContextBroker_Debug_printmsg(msg);
	#endif
	DenKr_ContextBroker_free_msg(msg);
	DenKr_ContextBroker_sendInfo_CSV(CAT(C_PREF,_InfBrok_cb_request_InfBrokerIface),DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str,0,"cake","Täter","Homer",NULL);
	return NULL;
}

//Example: Callback, KeyValue_CSV Msg Type, receive
void* CAT(C_PREF,_InfBrok_cb_csv)(void* arg)
{
	#ifdef DEBUG_module_generic__template
		puts("");
		printfc(gray,"DEBUG");printfc(DENKR__COLOR_ANSI__DL_LIBS," (Plugin)");printfc(gray,":");printfc(gray," Callback called:");printf(" (CSV-Message) %s_InfBrok_cb_csv\n",STRINGIFY(C_PREF));
	#endif
	DenKr_InfBroker_msg* msg;
	DenKr_ContextBroker_parsemsg_callback(arg,&msg);
	DenKr_ContextBroker_parsemsg_CSV(msg);
	#ifdef DEBUG_module_generic__template
		DenKr_ContextBroker_Debug_printmsg_CSV(msg);
		printfc(magenta,"Example-Test:");printf(" Manually printing the CSV Values using \"DenKr_ContextBroker_CSV_countKeyTimes\", \"DenKr_ContextBroker_CSV_getNthIndex\", \"DenKr_ContextBroker_CSV_getValue\"\n");
		int keyCount;
		DenKr_ContextBroker_CSV_countKeyTimes(msg, "SameKey", &keyCount);
		printfc(magenta,"Example-Test:");printf(" Count Key \"SameKey\": %d\n",keyCount);
		int keyIndex;
		void* value;
		char* returnedKey;
		if(!DenKr_ContextBroker_CSV_getNthIndex(msg, "SameKey", 2, &keyIndex)){
			printfc(magenta,"Example-Test:");printf(" Index of 2nd occurrence of Key \"SameKey\": %d\n",keyIndex);
			printf("\tValue (of Index %d): ", keyIndex);
			switch(DenKr_ContextBroker_CSV_getValbyIndex(msg, keyIndex, &value, &returnedKey)){
			case 0:
				printf("!! requested Index too high !!");
				break;
			case 1:
				printf("%d  |  returned Key of this Index (just for curiosity) %s",*((int*)value), returnedKey);
				break;
			case 2:
				printf("%s  |  returned Key of this Index (just for curiosity) %s",(char*)value, returnedKey);
				break;
			}
			puts("");
		}
		if(!DenKr_ContextBroker_CSV_getNthIndex(msg, "SameKey", 3, &keyIndex)){
			printfc(magenta,"Example-Test:");printf(" Index of 3rd occurrence of Key \"SameKey\": %d\n",keyIndex);
			printf("\tValue (of Index %d): ", keyIndex);
			switch(DenKr_ContextBroker_CSV_getValbyIndex(msg, keyIndex, &value, &returnedKey)){
			case 0:
				printf("!! requested Index too high !!");
				break;
			case 1:
				printf("%d  |  returned Key of this Index (just for curiosity) %s",*((int*)value), returnedKey);
				break;
			case 2:
				printf("%s  |  returned Key of this Index (just for curiosity) %s",(char*)value, returnedKey);
				break;
			}
			puts("");
		}
	#endif
	DenKr_ContextBroker_free_msg(msg);
	return NULL;
}


//TODO:
static void* CAT(C_PREF,_listenThread)(void* arg)
{
	return NULL;
}


//TODO:
void* CAT(hook_role_thread_,C_PREF)(void* arg)
{
	DenKr_generic_module_thread__startPreamble
	int err;

	//CREATE_PROGRAM_PATH_REUSE(*argvg);

	//==================================================================================================//
	//----  Context-Registrations (at least the initial ones...)  --------------------------------------//
	//----       Could, if desired, surely change over Runtime  ----------------------------------------//
	//==================================================================================================//
	((InfBrokerIface->FuncRegConsumer).regSocket)(InfBrokerIface,"test_consumer_socket","cake","SDN","Schedule",NULL);
	((InfBrokerIface->FuncRegConsumer).regCallback)(InfBrokerIface, CAT(C_PREF,_InfBrok_cb_gear), "test_consumer_callback", NULL);
		CAT(C_PREF,_InfBrok_cb_request_InfBrokerIface) = InfBrokerIface;
	((InfBrokerIface->FuncRegProducer).regCallback)(InfBrokerIface, CAT(C_PREF,_InfBrok_cb_request), "cake", "gear", NULL);
	((InfBrokerIface->FuncRegConsumer).regCallback)(InfBrokerIface, CAT(C_PREF,_InfBrok_cb_csv), "test_CSV", NULL);
	//To test the PolicyManager without existing Bosch-CNC. Delete/Outcomment someday
	((InfBrokerIface->FuncRegConsumer).regSocket)(InfBrokerIface, "config_decision", "dashboard_async", "dashboard_feedback", NULL);
	//==================================================================================================//
	/*Tell Main, that i am Ready and than wait for Main, to tell me, that everyone is Ready.*/
	DenKr_Thread_Ready_Init(shmem_headers, mainThreadID, ownID)

	//==================================================================================================//
	//----  Init finished, actual Module-Execution  ----------------------------------------------------//
	//==================================================================================================//


	DenKr_InfBroker_msg* brokmsg;

	//To test the PolicyManager without existing Bosch-CNC. Delete/Outcomment someday
	//!!!! free() the passed "arrPhyLink" and "arrAR" after use !!!!
	int parseMsg_intoCNC(DenKr_InfBroker_msg* msg, int* numPhyLink, pmToCNCEnt_Phy** arrPhyLink, int* numAR, pmToCNCEnt_AR** arrAR){
		int err=0;
		int i;
		char* tmpp;
		tmpp=(msg->msg);
		*numPhyLink=*((pmToCNC_phyCount*)tmpp);
		*arrPhyLink=malloc(*numPhyLink*sizeof(pmToCNCEnt_Phy));
		tmpp+=sizeof(pmToCNC_phyCount);
		for(i=0;i<*numPhyLink;i++){
			memcpy(&((*arrPhyLink)[i]),tmpp,sizeof(pmToCNCEnt_Phy));
			tmpp+=sizeof(pmToCNCEnt_Phy);
		}
		*numAR=*((pmToCNC_ARCount*)tmpp);
		*arrAR=malloc(*numAR*sizeof(pmToCNCEnt_AR));
		tmpp+=sizeof(pmToCNC_ARCount);
		for(i=0;i<*numAR;i++){
			memcpy(&((*arrAR)[i]),tmpp,sizeof(pmToCNCEnt_AR));
			tmpp+=sizeof(pmToCNCEnt_AR);
		}
		return err;
	}
	//- - - - - - - - - -

	#define TEST_TOTALLINKNUM 5
	#if !defined(DISABLE_DEPR)
		depr(template_1)
	#endif
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"topology","Dev1",3011,"Dev2",3012,"Status",1,"TotalLinkNum",TEST_TOTALLINKNUM,NULL);
	#if !defined(DISABLE_DEPR)
		depr(template_2)
	#endif
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str,0,"topology","Dev1","3011","Dev2","3130","Status","1","TotalLinkNum",STRINGIFY(TEST_TOTALLINKNUM),NULL);
	#if !defined(DISABLE_DEPR)
		depr(template_3)
	#endif
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"topology","Dev1",3011,"Dev2",3151,"Status",1,"TotalLinkNum",TEST_TOTALLINKNUM,NULL);
	#if !defined(DISABLE_DEPR)
		depr(template_4)
	#endif
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"topology","Dev1",3011,"Dev2",3170,"Status",1,"TotalLinkNum",TEST_TOTALLINKNUM,NULL);
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"topology","Dev1",3011,"Dev2",3140,"Status",1,"TotalLinkNum",TEST_TOTALLINKNUM,NULL);
	//- - - - - - - - - -
	//To test the PolicyManager without existing Dashboard
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str,0,"dabo_alt","Content","DaboInit","ARTotalNum","2",NULL);
	#if !defined(DISABLE_DEPR)
		depr(template_5)
	#endif
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str,0,"dabo_alt","Content","DaboChange","SequenceN","1","ARId","5000","ARState","true","ARProfileType","CycleTime","ARProfileVal","500",NULL);
	#if !defined(DISABLE_DEPR)
		depr(template_6)
	#endif
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str,0,"dabo_alt","Content","DaboChange","SequenceN","2","ARId","5020","ARState","false","ARProfileType","PacketSize","ARProfileVal","8192",NULL);
	#if !defined(DISABLE_DEPR)
		depr(template_7)
	#endif
	//- - - - - - - - - -
	WAITNS(1,0)
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"topology","Dev1",3011,"Dev2",3140,"Status",1,"TotalLinkNum",TEST_TOTALLINKNUM,NULL);
	WAITNS(0,500000000)
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str,0,"dabo_alt","Content","DaboChange","SequenceN","3","ARId","5000","ARState","false","ARProfileType","CycleTime","ARProfileVal","750",NULL);
	WAITNS(0,500000000)
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"topology","Dev1",3011,"Dev2",3151,"Status",1,"TotalLinkNum",TEST_TOTALLINKNUM,NULL);
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"topology","Dev1",3011,"Dev2",3170,"Status",0,"TotalLinkNum",TEST_TOTALLINKNUM,NULL);
	WAITNS(0,500000000)
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"topology","Dev1",3011,"Dev2",3170,"Status",1,"TotalLinkNum",TEST_TOTALLINKNUM,NULL);


	int i;
	while(1){
		DenKr_ContextBroker_recvmsg_socket(InfBrokerIface, &brokmsg);

		#ifdef DEBUG_module_generic__template
			puts("");
			printfc(gray,"DEBUG");printfc(DENKR__COLOR_ANSI__DL_LIBS," (Plugin)");printfc(gray,":");printfc(gray,": Socket msg from InfBrok:");printf(" recvd inside %s-Module\n",STRINGIFY(C_PREF));
			DenKr_ContextBroker_Debug_printmsg(brokmsg);
			//!!!! free() the passed "arrPhyLink" and "arrAR" after use !!!!
			if(strcmp(brokmsg->context, "config_decision")==0){
				printfc(gray,"DEBUG");printfc(DENKR__COLOR_ANSI__DL_LIBS," (Plugin)");printfc(gray,":");printfc(gray,": Socket msg from Policy-Manager:");printf(" recvd inside %s-Module\n",STRINGIFY(C_PREF));
				int numPhyLink, numAR;
				pmToCNCEnt_Phy* arrPhyLink=NULL;
				pmToCNCEnt_AR* arrAR=NULL;
				parseMsg_intoCNC(brokmsg, &numPhyLink, &arrPhyLink, &numAR, &arrAR);
				printf("Physical-Link Information (%d Entries): [\n", numPhyLink);
				for(i=0;i<numPhyLink;i++){
					printf("   Device-1 ID: %d | Device-2 ID: %d | Link-Status: %d\n", arrPhyLink[i].Dev1, arrPhyLink[i].Dev2, arrPhyLink[i].status);
				}
				printf("]\n");
				printf("Application-Relation Information (From Dashboard) (%d Entries): [\n", numAR);
				for(i=0;i<numAR;i++){
					printf("   AR-ID: %d | AR-State: %d | AR-Profile: { Parameter-Typ: %d | Value: %d }\n",arrAR[i].ARID,arrAR[i].ARstate,arrAR[i].ARprofile.paramType,arrAR[i].ARprofile.val);
				}
				printf("]\n");
				free(arrPhyLink);
				free(arrAR);
			}else if(strcmp(brokmsg->context, "dashboard_async")==0 || strcmp(brokmsg->context, "dashboard_feedback")==0){
				printf("Msg intended to go to the Dashboard:\n");
				DenKr_ContextBroker_parsemsg_CSV(brokmsg);
				#ifdef DEBUG_module_generic__template
					DenKr_ContextBroker_Debug_printmsg_CSV(brokmsg);
				#endif
			}
		#endif

		DenKr_ContextBroker_free_msg(brokmsg);
	}


    //==================================================================================================//
    //----  Module-Termination  ------------------------------------------------------------------------//
    //==================================================================================================//

	printfc(yellow,"IMPORTANT:");
	printfc(DENKR__COLOR_ANSI__THREAD_BASIC," Thread ");
	printf("is closing: "STRINGIFY(C_PREF)".\n");
	return NULL;
}


int CAT(DENKR_PLUGIN_DISC_INIT_FUNC_PREFIX,C_PREF)(PluginManager *plugman)
{
//	int err;
	char role[]="template_test";

	PluginManager_register_role_generic(plugman, role, strlen(role)+1, DenKr_plugin_working_type__thread, NULL, DenKr_plugin_interComm_method__infBroker, CAT(hook_role_thread_,C_PREF));

	return DenKr_plugin_role__generic;
}




#undef C_PREF
