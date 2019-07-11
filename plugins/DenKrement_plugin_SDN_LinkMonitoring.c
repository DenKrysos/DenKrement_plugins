/*
 * DenKrement_plugin_LinkMonitoring.c
 *
 *  Created on: 24.07.2017
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
//#include "./global/global_settings.h"
//#include "./global/global_variables.h"
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
#include <pthread.h>
//#include <string.h>
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
//==================================================================================================//
//==================================================================================================//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------//
//==================================================================================================//




// The "Content Prefix":
//    Put this in Front of every Function, Variable, simply every Symbol, that will
//    be visible outside, to make it unique and thus avoid clashes.
//  Set it identical to the File-Name.
#define C_PREF DenKrement_plugin_SDN_LinkMonitoring



void CONCAT(C_PREF, _settings_init())
{

}


void CONCAT(C_PREF, _gui_window_init())
{

}






void* CAT(hook_role_thread_,C_PREF)(void* arg)
{
	ThreadManager* thrall = (((struct SDN_link_monitoring_ThreadArgPassing *)arg)->thrall);
	struct ShMemHeader *shmem_headers = (((struct SDN_link_monitoring_ThreadArgPassing *)arg)->shmem_headers);
	DenKr_essentials_ThreadID ownID = (((struct SDN_link_monitoring_ThreadArgPassing *)arg)->ownID);
	DenKr_essentials_ThreadID mainThreadID = (((struct SDN_link_monitoring_ThreadArgPassing *)arg)->mainThreadID);
	free(arg);
	int err;
	#define shmem_recv_start_self ShMem_recv_start(&(shmem_headers[ownID]))
	#define shmem_recv_finish_self ShMem_recv_finish(&(shmem_headers[ownID]))
	#define shmem_self (shmem_headers[ownID])

//	CREATE_PROGRAM_PATH_REUSE(*argvg);

	printfc(DENKR__COLOR_ANSI__THREAD_BASIC,"Thread:");printf(" SDN based Link-Monitoring started.\n");
	printf("\t\tJust a testing-dummy...\n");

	//Tell Main, that i am Ready and than wait for Main, to tell me, that everyone is Ready.
	DenKr_Thread_Ready_Init(shmem_headers, mainThreadID, ownID)




	printfc(yellow,"IMPORTANT:");
	printfc(DENKR__COLOR_ANSI__THREAD_BASIC," Thread ");
	printf("is closing: DenKrement_plugin_SDN_LinkMonitoring.\n");
	#undef shmem_recv_start_self
	#undef shmem_recv_finish_self
	#undef shmem_self
	return NULL;
}


int CAT(DENKR_PLUGIN_DISC_INIT_FUNC_PREFIX,C_PREF)(PluginManager *plugman)
{
	int err;
	int role=DenKr_plugin_role__SDN_link_monitoring;

	PluginManager_register_role_predefined(plugman, role, DenKr_plugin_working_type__thread, CAT(hook_role_thread_,C_PREF));

	return role;
}




#undef C_PREF
