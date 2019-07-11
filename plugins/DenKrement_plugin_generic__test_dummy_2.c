/*
 * DenKrement_plugin_generic__test_dummy_2.c
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
//#include "plugins_export.h"
#include "headers/DenKrement_plugin_generic__test_dummy_2.h"
//==================================================================================================//
//==================================================================================================//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------//
//==================================================================================================//

#ifdef DEBUG
//	#define DEBUG_module_generic__test_dummy_2
#endif




// The "Content Prefix":
//    Put this in Front of every Function, Variable, simply every Symbol, that will
//    be visible outside, to make it unique and thus avoid clashes.
//  Set it identical to the File-Name.
#define C_PREF DenKrement_plugin_generic__test_dummy_2



void CONCAT(C_PREF, _settings_init())
{

}


void CONCAT(C_PREF, _gui_window_init())
{

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
	((InfBrokerIface->FuncRegConsumer).regSocket)(InfBrokerIface,"test_consumer_socket","cake",NULL);
	((InfBrokerIface->FuncRegProducer).regSocket)(InfBrokerIface,"test_consumer_socket","cake",NULL);
	//==================================================================================================//
	/*Tell Main, that i am Ready and than wait for Main, to tell me, that everyone is Ready.*/
	DenKr_Thread_Ready_Init(shmem_headers, mainThreadID, ownID)

	//==================================================================================================//
	//----  Init finished, actual Module-Execution  ----------------------------------------------------//
	//==================================================================================================//


	DenKr_InfBroker_msg* brokmsg;

	DenKr_ContextBroker_sendInfo_Str(InfBrokerIface,DenKr_InfBroker_Msg_Type__Raw,0,0,"cake","Sugar from test dummy 2!");
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str,0,"test_CSV","Key1","Value1","Key2","Value 2","User","admin","PW","0000","Empty-Value","","SpecialSigns","1/,=5","SpecialSignsOnly",",,=,=",NULL);
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"test_CSV","txpower",8,"PowerLevel",9000,"RandomValue",2402,"DefaultPW",1234,NULL);
	DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"test_CSV","SameKey",1,"SameKey",2,"OtherKey",10,"SameKey",3,NULL);


	while(1){
		DenKr_ContextBroker_recvmsg_socket(InfBrokerIface, &brokmsg);

		#ifdef DEBUG_module_generic__test_dummy_2
			puts("");
			printfc(gray,"DEBUG");printfc(DENKR__COLOR_ANSI__DL_LIBS," (Plugin)");printfc(gray,":");printfc(gray,": Socket msg from InfBrok:");printf(" recvd inside %s-Module\n",STRINGIFY(C_PREF));
			DenKr_ContextBroker_Debug_printmsg(brokmsg);
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
