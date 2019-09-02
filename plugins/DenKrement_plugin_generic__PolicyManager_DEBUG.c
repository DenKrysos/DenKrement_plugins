/*
 * DenKrement_plugin_generic__PolicyManager.c
 *
 *  Created on: 13.06.2019
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
//#include <stdint.h>
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
#include "DenKr_essentials/Program_Files/control_cfg_file.h"
#define DenKr_HASHTAB__PRIME_NUM_SMALLER_THAN_TABLESIZE 37
#define DenKr_HASHTAB__INIT_SIZE 53
#include "DenKr_essentials/DataStructures/hash_table.h"
//#include "nl80211.h"
//---------------------------------------------------------------------------------
#include "plugins_export.h"
#include "headers/DenKrement_plugin_generic__PolicyManager.h"
//---------------------------------------------------------------------------------
//#define DENKR_SORT_TYPE topo_lID
//#define DENKR_SORT_NAME PM
//#define DENKR_SORT_SELECTIVE
//#define DENKR_SORT_BINARY
//#include "DenKr_essentials/sort.h"
#define DENKR_SEARCH_STATIC static
#define DENKR_SEARCH_TYPE topo_lID
#define DENKR_SEARCH_NAME PM
#define DENKR_SEARCH_SELECTIVE
#define DENKR_SEARCH_BINARY
#include "DenKr_essentials/search.h"
//==================================================================================================//
//==================================================================================================//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------//
//==================================================================================================//


//TODO:
/*
 * Currently no rollback in case of config-decision failure exists
 * For introducing this, one must:
 * - Backup the entry for the new arrived Topo-Msg before override and reinsert this in case of config-calculation failure
 *   or
 * - Override the HashTable Entry first after successful calculation
 * -->
 * - Then in case of a failure "calculate the old decision" again and roll this out
 * - Together with a "Failure&Rollback" notification
 *
 *
 * Glättungslogik einbauen, um ein "Springen des Output" zu verhindern
 * -> Zwar jede Nachricht abrufen und intern verarbeiten (d.h. in HashTab eintragen, aber keine neue Config berechnen
 * -> Config erst berechnen (und anschließend aussenden), wenn keine neuen Nachrichten vom Socket empfangbar sind.
 * -> D.h.: Es wird das interne Gedächtnis zwar immer brav aktualisiert. Neue Configs werden jedoch nur berechnet und ausgesendet, wenn sich keine neuen Nachrichten in der Queue befinden.
 * --->> Achtung!!! Könnte zu Live-Lock führen, wenn andauernd Nachrichten abrufbar sind.
 *   --> Mit einem zusätzlichen Timeout ausstatten, sodass neue configs zu allermindest alle par Sekunden berechnet werden.
 * --->> Das wiederum könnte zu einem Drift führen, wodurch der PM immer weiter ins Hintertreffen gerät.
 *   --> Hem, wenn das Ding schlichtweg zu langsam ist, ist nichts zu machen. Nachrichten dürfen ja nicht gedroppt werden; das kreierte eine inkonsistente Datenhaltung.
 */



#ifdef DEBUG
//	#define DEBUG_module_generic__PolicyManager
#endif

#define DISABLE_DEPR




// The "Content Prefix":
//    Put this in Front of every Function, Variable, simply every Symbol, that will
//    be visible outside, to make it unique and thus avoid clashes.
//  Set it identical to the File-Name.
#define C_PREF DenKrement_plugin_generic__PolicyManager



void CONCAT(C_PREF, _settings_init())
{

}


void CONCAT(C_PREF, _gui_window_init())
{

}







//static void* CAT(C_PREF,_listenThread)(void* arg)
//{
//	return NULL;
//}



#define IFDEB(Code) if(isDebug==1){Code}
#define PMprint(text) printfc(DENKR__COLOR_ANSI__THREAD_BASIC,"Module");printf(" PolicyManager: "text);
#define PMprintDEB(text) PMprint() printfc(gray, "(DEBUG) "text);

void* CAT(hook_role_thread_,C_PREF)(void* arg)
{
	DenKr_generic_module_thread__startPreamble
	int err;

	CREATE_PROGRAM_PATH_REUSE(*argvg);

	//==================================================================================================//
	//----  Context-Registrations (at least the initial ones...)  --------------------------------------//
	//----       Could, if desired, surely change over Runtime  ----------------------------------------//
	//==================================================================================================//
	((InfBrokerIface->FuncRegConsumer).regSocket)(InfBrokerIface,"topology","dabo_alt",NULL);
	((InfBrokerIface->FuncRegProducer).regSocket)(InfBrokerIface,"dashboard","dashboard_feedback","dashboard_async","config_decision",NULL);
	//==================================================================================================//
	/*Tell Main, that i am Ready and than wait for Main, to tell me, that everyone is Ready.*/
	DenKr_Thread_Ready_Init(shmem_headers, mainThreadID, ownID)

//==================================================================================================//
//----  Init finished, actual Module-Execution  ----------------------------------------------------//
//==================================================================================================//

	//------------------------------
	// Few Module specific Init
	// Ascertain, if the module should work in Debug-Mode, determined by its File-Name
	//   During this process, as a byproduct, we check also whether its File-Name is valid at all
	#define FILENAME_INVALID 9
	#define FILENAME_INVALID_SUBSTR_POSITION 10
	char isDebug = FILENAME_INVALID;//Initially we start with assuming "No valid File-Name was found". If one is found, this value is turned to
										// 0  -- No Debug-Mode. Valid "Plain File-Name" was found.
										// 1  -- Debug-Mode. Valid File-Name with "_DEBUG" in it was found.
											// File-Name  SHALL EQUAL to "#define C_PREF"
	//NOTE: NO PROPER "Prefix-Checking" is done so far. I.e. it is checked indeed, if C_PREF is at the Beginning or just somewhere in the File-Name, but no "proper handling" of this is done. Here it is just said, that anytime when it is encountered that C_PREF lays inside some File-Name but not at the Beginning everything is Invalid and we exit.
	{
		int i;
		int timesCPREFfound = 0;
		int plugDir_len=ProgPath_length+sizeof("plugins/");
		char plugDir[plugDir_len];
		snprintf(plugDir,plugDir_len,"%s%s",ProgPath,"plugins/");
//		printfc(green,"Dev-State-Debug: ");printf("Plugin-Path: \n%s\n",plugDir);

		DIR *dir=NULL;
		struct dirent *direntry;
		struct stat  ent_stat;
		if((dir = opendir(plugDir)) == NULL){
			printfc(DENKR__COLOR_ANSI__THREAD_BASIC,"Module");printfc(red," ERROR:");printf(" PolicyManager: Cannot open \"%s\". Exit...\n",plugDir);
			exit(1);
		}

//		char* strstart=NULL;
		while( (direntry = readdir(dir)) != NULL ){
			int ent_fullpath_len = plugDir_len+strlen(direntry->d_name);
			char ent_fullpath[ent_fullpath_len+1];
			snprintf(ent_fullpath,ent_fullpath_len,"%s%s",plugDir,direntry->d_name);
			ent_fullpath[ent_fullpath_len]='\0';
//			printfc(green,"Dev-State-Debug: ");printf("Current-Read Full-Path: \n%s\n",ent_fullpath);
		    if (stat(ent_fullpath, &ent_stat)) {
		    	printfc(DENKR__COLOR_ANSI__THREAD_BASIC,"Module");printfc(red," ERROR:");printf(" PolicyManager: Couldn't get Dir-Entry stats: %s.\n",ent_fullpath);
		        exit(8);
		    }
		    if(ent_stat.st_mode & S_IFREG){
		    	// Directory Entry is a File
		    	// Now check, if it has a .so File-Extension
				char* lastdot;
				lastdot=direntry->d_name;
				for(i=0;i<256;i++){
					if(*lastdot=='\0'){
						goto FileNameEndFound;
					}
					lastdot++;
				}
				goto SkipFile;
				FileNameEndFound:
				for(i=0;i<256;i++){
					lastdot--;
					if(lastdot<=(direntry->d_name)){//The '==' Case would represent a File with File-Extension but without Name, if than a dot where found. I.e. a File only named ".so". Won't accept such a File.
						goto SkipFile;
					}
					if(*lastdot=='.')
						break;
				}
				if(strcmp(lastdot, ".so") != 0){
					goto SkipFile;
				}
				int plug_name_len=lastdot-direntry->d_name;
				char plug_name[plug_name_len+1];
				memcpy(plug_name,direntry->d_name,plug_name_len);
				plug_name[plug_name_len]='\0';
//				printfc(green,"Dev-State-Debug: ");printf(" Tackling to compare File-Name %s.\n",direntry->d_name);
				//Here the real File-Name comparison happens. Here we have every "valid File" for this purpose and loop over them.
				char* strstart=NULL;
				if((strstart=strstr(direntry->d_name,STRINGIFY(C_PREF)))){
					//Now we have the C_PREF somewhere in the File-Name
			    	if(strstart!=direntry->d_name){
			    		//The C_PREF does not lay at the beginning of the File-Name. For now just considered as totally invalid: Nothing more we can do about it: Terminate immediately.
			    		isDebug=FILENAME_INVALID_SUBSTR_POSITION;
			    		goto TerminateFileSearch;
			    	}else{
//			    		printfc(green,"Dev-State-Debug: ");printf(" Comparing Substring %s. | Added: %lli\n",(direntry->d_name)+sizeof(STRINGIFY(C_PREF)-1),sizeof(STRINGIFY(C_PREF))-1);
			    		timesCPREFfound++;
			    		if(strcmp((direntry->d_name)+sizeof(STRINGIFY(C_PREF))-1,".so")==0){
			    			//Now we have just C_PREF.so
			    			isDebug=0;
			    		}else if(strcmp((direntry->d_name)+sizeof(STRINGIFY(C_PREF))-1,"_DEBUG.so")==0){
			    			isDebug=1;
			    		}else{
			    			printfc(DENKR__COLOR_ANSI__THREAD_BASIC,"Module");printfc(red," ERROR:");printf(" PolicyManager started, but some malformed File-Name detected. Did you maybe introduce some typo? ;o) Let's exit to give you the chance to fix this...\n");
			    			exit(1);
			    		}
			    	}
				}
		    }else if(ent_stat.st_mode & S_IFDIR){
		    	// Directory Entry is a Directory
		    	// Maybe add the option to go recursive over Subfolders.
		    }
		    SkipFile:
			;
		}
		TerminateFileSearch:

		if(timesCPREFfound<1){
			printfc(DENKR__COLOR_ANSI__THREAD_BASIC,"Module");printfc(red," ERROR:");printf(" PolicyManager started, but no valid File-Name found. Did you mess it up? ;o) Let's exit to give you the chance to fix this...\n");
			exit(1);
		}else if(timesCPREFfound>1){
			printfc(DENKR__COLOR_ANSI__THREAD_BASIC,"Module");printfc(red," ERROR:");printf(" PolicyManager started, but multiple Files with "STRINGIFY(C_PREF)" in it found. Did you screw something? ;o) Maybe File-Duplication? Let's exit to give you the chance to fix this...\n");
			exit(1);
		}

		if(isDebug==1){
			printfc(DENKR__COLOR_ANSI__THREAD_BASIC,"Module");printf(" PolicyManager, ");printfc(gray,"DEBUG");printf("-Mode: Enabled. Started in Debugging-Mode, i.e. WITH Debug Output on Console.\n");
			printf("\t(-> see \"<exe-Dir 'DenKrement'>/plugins/DenKrement_plugin_generic__PolicyManager_README_Instructions.txt\")\n");
		}else if(isDebug==0){
			printfc(DENKR__COLOR_ANSI__THREAD_BASIC,"Module");printf(" PolicyManager, ");printfc(gray,"DEBUG");printf("-Mode: Disabled. Started in Normal-Operation, i.e. NO Debug Output on Console.\n");
			printf("\t(-> see \"<exe-Dir 'DenKrement'>/plugins/DenKrement_plugin_generic__PolicyManager_README_Instructions.txt\")\n");
		}else{
			printfc(DENKR__COLOR_ANSI__THREAD_BASIC,"Module");printf(" PolicyManager, ");printfc(DENKR__COLOR_ANSI__DEBUG,"DEBUG");printf("Bug during Initialization Phase detected. Have to exit. Please contact your Developer at heart.\n");
			exit(1);
		}
	}
	#undef FILENAME_INVALID
	#undef FILENAME_INVALID_SUBSTR_POSITION

//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
//----------  Temporary Testing Section  --------------------------------------------------------------------------------------==========
//=======================================================================================================================================
//=======================================================================================================================================
#ifdef DEBUG //----------------------------------------------------------------------------------------------------------------==========
#ifndef RELEASE_VERSION //Just to get sure... ;o) that nothing got forgotten within the final Build  --------------------------==========
//=======================================================================================================================================


//exit(1);


//=======================================================================================================================================
#endif //----------------------------------------------------------------------------------------------------------------------==========
#endif //----------------------------------------------------------------------------------------------------------------------==========
//=======================================================================================================================================
//=======================================================================================================================================
//----------  End  -  Temporary Testing Section  ------------------------------------------------------------------------------==========
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================


	//NOTES:
	//Set "LinkNum" with first received msg and check with every following Topo-Msg. If different: Throw error: Physical Topology changed. Dynamicity not properly implemented yet
	//==================================================================================================//
	//----  Main Operation  ----------------------------------------------------//
	//==================================================================================================//
	#define STARTUPSTATE_DashboardAlreadyGot 1
	#define STARTUPSTATE_CNCAlreadyGot 2

	int readNum_fromStrCSV(DenKr_InfBroker_msg* brokmsg, char* key, int* res){
		int err=0;
		int csvidx;
		void* csvval;
		if((err=DenKr_ContextBroker_CSV_getNthIndex(brokmsg,key,1,&csvidx))){
			IFDEB(
				//PMprintDEB("Malformed \"topology\" msg. No Key \"");printf("%s\" found.\n",key);
				PMprintDEB("Malformed msg. No Key \"");printf("%s\" found.\n",key);
			)
			return err;
		}else{
			DenKr_ContextBroker_CSV_getValbyIndex(brokmsg,csvidx,&csvval,NULL);
			char* tailptr;
			*res=strtol(csvval,&tailptr,10);
		}
		return err;
	}
	void get_ent_from_HashTabNum(void** ht_ent, DenKr_HashTabNum_hash_table* hashtab, DenKr_HashTabNum_Key__type ID){
		DenKr_HashTabNum_ht_item* ht_it;
		ht_it = DenKr_HashTabNum_search(hashtab,ID);
		if(ht_it){
			(*ht_ent)=ht_it->value;
		}else{
			*ht_ent=NULL;
		}
	}

	DenKr_InfBroker_msg* brokmsg;

	DenKr_HashTabNum_hash_table* hashtab_topo;//Contains the physical Topology-Information. Gesendet von der "Bosch-CNC" Stored per Link (e.g.: "Dev1"=1234 & "Dev2"=5678 wird zur Link-ID 12345678, genutzt als Key)
	DenKr_HashTabNum_hash_table* hashtab_dabo;//Contains the selections from the Dashboard.

	DenKr_HashTabNum_htInit(&hashtab_topo);
	DenKr_HashTabNum_htInit(&hashtab_dabo);

	topo_lID* linkIDs=NULL;//An Array to keep track of the already written link-IDs
	int linkIDsC=0;
	dabo_ARID* ARIDs=NULL;//An Array to keep track of the already written link-IDs
	int ARIDsC=0;

	struct PM_hashtab_topology_entry_t topo_htentry;
	struct PM_hashtab_dashboard_entry_t dabo_htentry;
	uint32_t dev1, dev2;
//	uint64_t daboSeqN=0;
//	char seqNStr[20];
	char* daboSeqNp=NULL;
	int tmpint;
	int csvidx;
	void* csvval;

	#define RET_SKIPMSG 50
	int recv_topology(){
		switch((brokmsg->head)->subtype){
		case DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int:
			#if !defined(DISABLE_DEPR)
				depr(PM_recv_int_1)
			#endif
			if(DenKr_ContextBroker_CSV_getNthIndex(brokmsg,"Dev1",1,&csvidx)){
				IFDEB(
					PMprintDEB("Malformed \"topology\" msg. No Key \"Dev1\" found.\n");
				)
				return RET_SKIPMSG;
			}else{
				DenKr_ContextBroker_CSV_getValbyIndex(brokmsg,csvidx,(void**)(&csvval),NULL);
			}
			topo_htentry.Dev1 = *((uint16_t*)csvval);
			#if !defined(DISABLE_DEPR)
				depr(PM_recv_int_2)
			#endif
			if(DenKr_ContextBroker_CSV_getNthIndex(brokmsg,"Dev2",1,&csvidx)){
				IFDEB(
					PMprintDEB("Malformed \"topology\" msg. No Key \"Dev2\" found.\n");
				)
				return RET_SKIPMSG;
			}else{
				DenKr_ContextBroker_CSV_getValbyIndex(brokmsg,csvidx,(void**)(&csvval),NULL);
			}
			topo_htentry.Dev2 = *((uint16_t*)csvval);
			#if !defined(DISABLE_DEPR)
				depr(PM_recv_int_3)
			#endif
			topo_htentry.linkID = ((topo_htentry.Dev1)<<16) | topo_htentry.Dev2;

			if(DenKr_ContextBroker_CSV_getNthIndex(brokmsg,"Status",1,&csvidx)){
				IFDEB(
					PMprintDEB("Malformed \"topology\" msg. No Key \"Status\" found.\n");
				)
				return RET_SKIPMSG;
			}else{
				DenKr_ContextBroker_CSV_getValbyIndex(brokmsg,csvidx,(void**)(&csvval),NULL);
			}
			if(*((int*)csvval)==0){
				topo_htentry.status=phy_topo_link_status__inactive;
			}else if(*((int*)csvval)==1){
				topo_htentry.status=phy_topo_link_status__active;
			}else{
				IFDEB(
					PMprintDEB("Malformed \"topology\" msg. Status not of Value \"enabled\"/\"1\"  or  \"disabled\"/\"0\". Skipping msg.\n");
					return RET_SKIPMSG;
				)
			}
			#if !defined(DISABLE_DEPR)
				depr(PM_recv_int_4)
			#endif
			break;
		case DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str:
			#if !defined(DISABLE_DEPR)
				depr(PM_recv_str)
			#endif
			if(readNum_fromStrCSV(brokmsg, "Dev1", &tmpint)){
				return RET_SKIPMSG;
			}
			dev1=(uint32_t)tmpint;
			if(readNum_fromStrCSV(brokmsg, "Dev2", &tmpint)){
				return RET_SKIPMSG;
			}
			dev2=(uint32_t)tmpint;
			topo_htentry.Dev1 = dev1;
			topo_htentry.Dev2 = dev2;
			topo_htentry.linkID = (dev1<<16) | dev2;
			if(DenKr_ContextBroker_CSV_getNthIndex(brokmsg,"Status",1,&csvidx)){
				IFDEB(
					PMprintDEB("Malformed \"topology\" msg. No Key \"Status\" found.\n");
				)
				return RET_SKIPMSG;
			}else{
				DenKr_ContextBroker_CSV_getValbyIndex(brokmsg,csvidx,(void**)(&csvval),NULL);
			}
			if(strcmp(csvval, "enabled")==0 || strcmp(csvval, "1")==0){
				topo_htentry.status=phy_topo_link_status__active;
			}else if(strcmp(csvval, "disabled")==0 || strcmp(csvval, "0")==0){
				topo_htentry.status=phy_topo_link_status__inactive;
			}else{
				IFDEB(
					PMprintDEB("Malformed \"topology\" msg. Status not of Value \"enabled\"/\"1\"  or  \"disabled\"/\"0\". Skipping msg.\n");
					return RET_SKIPMSG;
				)
			}
			break;
		default:
			break;
		}
		#if !defined(DISABLE_DEPR)
			depr(PM_recv_int_preHTinsert)
		#endif
		DenKr_HashTabNum_insert(hashtab_topo,topo_htentry.linkID,&topo_htentry, sizeof(topo_htentry));
		#if !defined(DISABLE_DEPR)
			depr(PM_recv_int_postHTinsert)
		#endif
		return 0;
	}
	int recv_dashboard(){
		//Screw the numeric approach. Just directly bypass the string from the received msg into the preperation of the new send-msg
		//if(readNum_fromStrCSV(brokmsg, "SequenceN", &daboSeqN)){
		//	return RET_SKIPMSG;
		//}
		if(DenKr_ContextBroker_CSV_getNthIndex(brokmsg,"SequenceN",1,&csvidx)){
			IFDEB(
				PMprintDEB("Malformed \"dabo_alt\" msg. No Key \"SequenceN\" found.\n");
			)
			return RET_SKIPMSG;
		}else{
			DenKr_ContextBroker_CSV_getValbyIndex(brokmsg,csvidx,(void**)(&daboSeqNp),NULL);
		}

		if(readNum_fromStrCSV(brokmsg, "ARId", &tmpint)){
			return RET_SKIPMSG;
		}
		dabo_htentry.ARID=tmpint;

		if(DenKr_ContextBroker_CSV_getNthIndex(brokmsg,"ARState",1,&csvidx)){
			IFDEB(
				PMprintDEB("Malformed \"dabo_alt\" msg. No Key \"ARState\" found.\n");
			)
			return RET_SKIPMSG;
		}else{
			DenKr_ContextBroker_CSV_getValbyIndex(brokmsg,csvidx,(void**)(&csvval),NULL);
		}
		if(strcmp(csvval, "enabled")==0 || strcmp(csvval, "1")==0 || strcmp(csvval, "active")==0 || strcmp(csvval, "true")==0){
			dabo_htentry.ARstate=dabo_ar_state__active;
		}else if(strcmp(csvval, "disabled")==0 || strcmp(csvval, "0")==0 || strcmp(csvval, "inactive")==0 || strcmp(csvval, "false")==0){
			dabo_htentry.ARstate=dabo_ar_state__inactive;
		}else{
			IFDEB(
				PMprintDEB("Malformed \"dabo_alt\" msg. ARStatus not of Value \"enabled\"/\"1\"/\"active\"/\"true\"  or  \"disabled\"/\"0\"/\"inactive\"/\"false\". Skipping msg.\n");
				return RET_SKIPMSG;
			)
		}

		if(DenKr_ContextBroker_CSV_getNthIndex(brokmsg,"ARProfileType",1,&csvidx)){
			IFDEB(
				PMprintDEB("Malformed \"dabo_alt\" msg. No Key \"ARProfileType\" found.\n");
			)
			return RET_SKIPMSG;
		}else{
			DenKr_ContextBroker_CSV_getValbyIndex(brokmsg,csvidx,(void**)(&csvval),NULL);
		}
		//TODO: Maybe to be extended/changed
		if(strcmp(csvval, "CycleTime")==0){
			dabo_htentry.ARprofile.paramType=DABO_AR_Profile_Type__CycleTime;
		}else if(strcmp(csvval, "PacketSize")==0){
			dabo_htentry.ARprofile.paramType=DABO_AR_Profile_Type__PktSize;
		}else{
			IFDEB(
				PMprintDEB("Malformed \"dabo_alt\" msg. ARProfileType not of valid Value.\n");
				return RET_SKIPMSG;
			)
		}

		if(readNum_fromStrCSV(brokmsg, "ARProfileVal", &tmpint)){
			return RET_SKIPMSG;
		}
		dabo_htentry.ARprofile.val=tmpint;

		DenKr_HashTabNum_insert(hashtab_dabo,dabo_htentry.ARID,&dabo_htentry, sizeof(dabo_htentry));
		return 0;
	}

	//Start-Up Phase
	IFDEB(
		PMprintDEB("Start-Up / Init Phase begun.\n");
	)
	{//Scope to eliminate temporary variables from Start-Up Phase
		int state=0;
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//TODO !!!! Very temporary. DELETE THIS LINE !!!!!!!!
//		state=STARTUPSTATE_DashboardAlreadyGot;
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//- - - - - - - - - - - - - - - - - - - - - -
		StartUpPhase:
		//Blocking receive
		DenKr_ContextBroker_recvmsg_socket(InfBrokerIface, &brokmsg);
		//Parse msg
		//Don't forget to use "DenKr_ContextBroker_free_msg(brokmsg);" at the right places. Here not included because of readability.
		switch(brokmsg->head->type){
		case DenKr_InfBroker_Msg_Type__KeyEqualValue_CSV:
			DenKr_ContextBroker_parsemsg_CSV(brokmsg);
			//DenKr_ContextBroker_Debug_printmsg_CSV(brokmsg);
			break;
		default:
			PMprint();printfc(yellow,"WARNING: ");printf("Invalid Msg-Format received. Ignoring...\n");
			DenKr_ContextBroker_free_msg(brokmsg);
			goto StartUpPhase;
			break;
		}

		//Switch "Sender"
		if(strcmp(brokmsg->context, "topology")==0){//From CNC
			#if !defined(DISABLE_DEPR)
				depr(PM_topo_msg)
			#endif
			switch((brokmsg->head)->subtype){
			unsigned int sidx;
			case DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int:
				//Maybe check for completeness (i.e. is it only a single-link msg that indicates a change or is what we actually want here: The whole topology.
				//////『Throw into Memory』
				if((err=recv_topology())==RET_SKIPMSG){DenKr_ContextBroker_free_msg(brokmsg); goto StartUpPhase;}
				#if !defined(DISABLE_DEPR)
					depr(PM_post_recv_int)
				#endif
				//Initialize or update the linkIDs Array
				if(!linkIDs){
					if(DenKr_ContextBroker_CSV_getNthIndex(brokmsg,"TotalLinkNum",1,&csvidx)){
						IFDEB(
							PMprintDEB("Malformed \"topology\" msg. No Key \"TotalLinkNum\" found.\n");
						)
						DenKr_ContextBroker_free_msg(brokmsg); goto StartUpPhase;
					}else{
						DenKr_ContextBroker_CSV_getValbyIndex(brokmsg,csvidx,(void**)(&csvval),NULL);
					}
					linkIDsC=*((int*)csvval);
					linkIDs=malloc(linkIDsC*sizeof(topo_lID));
					memset(linkIDs,0xFF,linkIDsC*sizeof(topo_lID));//Either init with 0 and start filling from the back or init with MAX and start filling from the start
				}
				if(!PM_BinarySearch(linkIDs, linkIDsC, topo_htentry.linkID, &sidx)){
					if(sidx>=linkIDsC){
						//INFO: Watch here, if you change the init/indexing approach above
						PMprintDEB("Something went wrong during linkID indexing. Malformed Dev-IDs or wrong TotalLinkNum sent? Exiting...\n");
						exit(9000);
					}
					memmove(&linkIDs[sidx+1], &linkIDs[sidx], (linkIDsC-sidx-1)*sizeof(topo_lID));
					linkIDs[sidx]=topo_htentry.linkID;
				}
				break;
			case DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str:
				//Maybe check for completeness (i.e. is it only a single-link msg that indicates a change or is what we actually want here: The whole topology.
				//////『Throw into Memory』
				if((err=recv_topology())==RET_SKIPMSG){DenKr_ContextBroker_free_msg(brokmsg); goto StartUpPhase;}
				//Initialize or update the linkIDs Array
				if(!linkIDs){
					int totalLinkNum=0;
					if(readNum_fromStrCSV(brokmsg, "TotalLinkNum", &totalLinkNum)){
						DenKr_ContextBroker_free_msg(brokmsg); goto StartUpPhase;
					}
					linkIDsC=totalLinkNum;
					linkIDs=malloc(linkIDsC*sizeof(topo_lID));
					memset(linkIDs,0xFF,linkIDsC*sizeof(topo_lID));//Either init with 0 and start filling from the back or init with MAX and start filling from the start
				}
				if(!PM_BinarySearch(linkIDs, linkIDsC, topo_htentry.linkID, &sidx)){
					if(sidx>=linkIDsC){
						//INFO: Watch here, if you change the init/indexing approach above
						PMprintDEB("Something went wrong during linkID indexing. Malformed Dev-IDs sent? Exiting...\n");
						exit(9000);
					}
					memmove(&linkIDs[sidx+1], &linkIDs[sidx], (linkIDsC-sidx-1)*sizeof(topo_lID));
					linkIDs[sidx]=topo_htentry.linkID;
				}
				break;
			default:
				IFDEB(
					PMprintDEB("Error: Invalid Msg Subtype received.\n");
				)
				break;
			}
			IFDEB(
				if(linkIDs){
					PMprintDEB("Indexed LinkIDs: [ ");
					printf("0x%08X",linkIDs[0]);
					int it;
					for(it=1;it<linkIDsC;it++){
						printf(", 0x%08X",linkIDs[it]);
					}
					printf(" ]\n");
				}
			)
			//Little Debug Output
			IFDEB(
				PMprintDEB("Current number of links in memory: ");printf("%d\n",hashtab_topo->count);
				struct PM_hashtab_topology_entry_t* htent_deb;
				get_ent_from_HashTabNum((void**)&htent_deb, hashtab_topo, topo_htentry.linkID);
				PMprintDEB("Last entered: ");printf("LinkID: %d | Dev1: %d | Dev2: %d | status: %d     --  (Internal: LinkID=(dev1<<16)|dev2), status_active=%d, status_inactive=%d\n",
						htent_deb->linkID,
						(htent_deb->linkID&BITMASK_Dev1)>>16,
						htent_deb->linkID&BITMASK_Dev2,
						htent_deb->status,
						phy_topo_link_status__active,
						phy_topo_link_status__inactive
				);
			)
			//FSM Handling
			if(hashtab_topo->count >= linkIDsC){//There should be a more proper handling of different cases...
				IFDEB(
					PMprintDEB("Initialization of CNC related Topology Data finished.\n");
				)
				if(state==STARTUPSTATE_DashboardAlreadyGot){
					DenKr_ContextBroker_free_msg(brokmsg);
					IFDEB(
						PMprintDEB("Startup-Phase finished. Going over to permanent Operation.\n");
					)
					goto StartUpFinished;
				}
				state=STARTUPSTATE_CNCAlreadyGot;
			}
			//If implemented by producers: "Request Everything" from Dashboard & (if !(topology_complete)) from CNC. Vice-versa in the case below
			//goto StartUpPhase;
		}else if(strcmp(brokmsg->context, "dabo_alt")==0){//From Dashboard
			if(DenKr_ContextBroker_CSV_getNthIndex(brokmsg,"Content",1,&csvidx)){
				IFDEB(
					PMprintDEB("Malformed \"dabo_alt\" msg. No Key \"Content\" found.\n");
				)
				goto StartUpPhase;
			}else{
				DenKr_ContextBroker_CSV_getValbyIndex(brokmsg,csvidx,(void**)(&csvval),NULL);
			}
			if(strcmp(csvval, "DaboChange")==0){
				if(!ARIDsC){
					PMprint("");printfc(red,"ERROR: ");printf("No \"DaboInit\" received so far, but got a \"DaboChange\". Thus I am NOT initialized. Ich kann so nicht arbeiten! Exit, EXIT!\n");
					exit(9000);
				}
				//Same thing to check for completeness: We need state of all AR-Checkboxes and selection of varying dropdown-lists
				//////『Throw into Memory』
				if((err=recv_dashboard())==RET_SKIPMSG){DenKr_ContextBroker_free_msg(brokmsg); goto StartUpPhase;}
				//TODO WORKING STATE
				unsigned int sidx;
				if(!PM_BinarySearch(ARIDs, ARIDsC, dabo_htentry.ARID, &sidx)){
					if(sidx>=ARIDsC){
						//INFO: Watch here, if you change the init/indexing approach above
						PMprintDEB("Something went wrong during AR-ID indexing. Malformed AR-ID sent? Exiting...\n");
						exit(9000);
					}
					memmove(&ARIDs[sidx+1], &ARIDs[sidx], (ARIDsC-sidx-1)*sizeof(dabo_ARID));
					ARIDs[sidx]=dabo_htentry.ARID;
				}
				IFDEB(
					if(ARIDs){
						PMprintDEB("Indexed ARIDs: [ ");
						printf("0x%08X",ARIDs[0]);
						int it;
						for(it=1;it<ARIDsC;it++){
							printf(", 0x%08X",ARIDs[it]);
						}
						printf(" ]\n");
					}
				)
				//Little Debug Output
				IFDEB(
					PMprintDEB("Current number of AR-IDs in memory: ");printf("%d\n",hashtab_dabo->count);
					struct PM_hashtab_dashboard_entry_t* htent_deb;
					get_ent_from_HashTabNum((void**)&htent_deb, hashtab_dabo, dabo_htentry.ARID);
					PMprintDEB("Last entered: ");printf("AR-ID: %d | ARstate: %d | ARprofile.paramType: %d | ARprofile.val: %d     --  (Internal: status_active=%d, status_inactive=%d || ProfileType_CycleTime=%d, ProfileType_PktSize=%d\n",
							htent_deb->ARID,
							htent_deb->ARstate,
							htent_deb->ARprofile.paramType,
							htent_deb->ARprofile.val,
							dabo_ar_state__active,
							dabo_ar_state__inactive,
							DABO_AR_Profile_Type__CycleTime,
							DABO_AR_Profile_Type__PktSize
					);
				)
				//FSM Handling
				//TODO:Condition
				if(hashtab_dabo->count >= ARIDsC){//There should be a more proper handling of different cases...
					IFDEB(
						PMprintDEB("Initialization of Dashboard related Selection Data finished.\n");
					)
					if(state==STARTUPSTATE_CNCAlreadyGot){
						DenKr_ContextBroker_free_msg(brokmsg);
						IFDEB(
							PMprintDEB("Startup-Phase finished. Going over to permanent Operation.\n");
						)
						goto StartUpFinished;
					}
					state=STARTUPSTATE_DashboardAlreadyGot;
				}
			}else if(strcmp(csvval, "DaboInit")==0){
				if(readNum_fromStrCSV(brokmsg, "ARTotalNum", &ARIDsC)){
					PMprint(" Bad \"DaboInit\"-msg. Skipping.\n");
					goto StartUpPhase;
				}
				ARIDs=malloc(ARIDsC*sizeof(dabo_ARID));
				memset(ARIDs,0xFF,ARIDsC*sizeof(dabo_ARID));//Either init with 0 and start filling from the back or init with MAX and start filling from the start
			}else{
				PMprint("");printfc(red,"ERROR: ");printf("Invalid \"Content\" inside of \"dabo_alt msg\". Skipping\n");
			}
			//goto StartUpPhase;
		}else{
			IFDEB(
				PMprintDEB("Search for: euiva983.\n");
			)
			exit(1);
		}
		DenKr_ContextBroker_free_msg(brokmsg);
		goto StartUpPhase;
		StartUpFinished:
		;
	}
	#undef STARTUPSTATE_DashboardAlreadyGot
	#undef STARTUPSTATE_CNCAlreadyGot
	//When init messages of both sources are received: Go over to permanent operation

	//NOTE: I know, that is reeeaaally dirty. An inline Function using partly passed arguments and partly scope-grabbed variables... And also mixing the count value from the hashtab with the separately tracked array-size of IDs...
	int prepMsg_intoCNC(int* msgsiz, char** msg, DenKr_HashTabNum_hash_table* ht_topo, DenKr_HashTabNum_hash_table* ht_dabo){
		int err=0;
		int i;
		char* tmpp;
		struct PM_hashtab_topology_entry_t* topo_htentry;
		struct PM_hashtab_dashboard_entry_t* dabo_htentry;

		*msgsiz=(ht_topo->count * sizeof(pmToCNCEnt_Phy) + ht_dabo->count * sizeof(pmToCNCEnt_AR) + sizeof(pmToCNC_phyCount) + sizeof(pmToCNC_ARCount) +1);
		*msg=malloc(*msgsiz);

		**((pmToCNC_phyCount**)msg)=ht_topo->count;
		tmpp=*msg+sizeof(pmToCNC_phyCount);
		for(i=0;i<ht_topo->count;i++){
			get_ent_from_HashTabNum((void**)&topo_htentry, hashtab_topo, linkIDs[i]);
			if(!topo_htentry){
				PMprintDEB("");printfc(red,"BUG detected!");printf(" Couldn't read Link-ID \"0x%08X\" (Dev1: \"%d\", Dev2: \"%d\") out of Hash-Table (Key not existent).\n",linkIDs[i], (linkIDs[i] & BITMASK_Dev1)>>16, linkIDs[i] & BITMASK_Dev2);
				exit(1);
			}
			((pmToCNCEnt_Phy*)tmpp)->Dev1=(topo_htentry->linkID & BITMASK_Dev1)>>16;
			((pmToCNCEnt_Phy*)tmpp)->Dev2=topo_htentry->linkID & BITMASK_Dev2;
			((pmToCNCEnt_Phy*)tmpp)->status=topo_htentry->status;
			tmpp+=sizeof(pmToCNCEnt_Phy);
		}
		*((pmToCNC_ARCount*)tmpp)=ht_dabo->count;
		tmpp+=sizeof(pmToCNC_ARCount);
		for(i=0;i<ht_dabo->count;i++){
			get_ent_from_HashTabNum((void**)&dabo_htentry, hashtab_dabo, ARIDs[i]);
			if(!dabo_htentry){
				PMprintDEB("");printfc(red,"BUG detected!");printf(" Couldn't read AR-ID \"%d\" out of Hash-Table (Key not existent).\n",ARIDs[i]);
				exit(1);
			}
			memcpy(tmpp,dabo_htentry,sizeof(pmToCNCEnt_AR));
			tmpp+=sizeof(pmToCNCEnt_AR);
		}
		(*msg)[*msgsiz]='\0';

		return err;
	}
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

	//Prepare the Payload to send to the Dashboard in Case of "Full-Information to send"
	int prepMsg_toDaBo(int* msgsiz, char** msg, DenKr_HashTabNum_hash_table* ht_topo, DenKr_HashTabNum_hash_table* ht_dabo, char* contentStr){
		int err=0;
		int i;
		#define TMPSIZ 256
		char tmpStr[TMPSIZ];
		char* tmpp;
		struct PM_hashtab_topology_entry_t* topo_htentry;
		//struct PM_hashtab_dashboard_entry_t* dabo_htentry;

		*msgsiz=0;
		*msgsiz+=strlen("Content")+strlen(contentStr)+strlen("ActiveConfig")+strlen("NEW")+4;

		for(i=0;i<ht_topo->count;i++){
			get_ent_from_HashTabNum((void**)&topo_htentry, hashtab_topo, linkIDs[i]);
			if(!topo_htentry){
				PMprintDEB("");printfc(red,"BUG detected!");printf(" Couldn't read Link-ID \"0x%08X\" (Dev1: \"%d\", Dev2: \"%d\") out of Hash-Table (Key not existent).\n",linkIDs[i], (linkIDs[i] & BITMASK_Dev1)>>16, linkIDs[i] & BITMASK_Dev2);
				exit(1);
			}
			snprintf(tmpStr,TMPSIZ,"%d",topo_htentry->Dev1);
			*msgsiz+=strlen("Dev1")+strlen(tmpStr)+2;
			snprintf(tmpStr,TMPSIZ,"%d",topo_htentry->Dev2);
			*msgsiz+=strlen("Dev2")+strlen(tmpStr)+2;
			*msgsiz+=strlen("Status")+1+2;
		}
		(*msgsiz)++;//I want one sign more, because at the very last i would like to end the message with "KEYn=VALUEn,\n"

		*msg=malloc(*msgsiz);
		tmpp=*msg;
		tmpp+=snprintf(tmpp,*msgsiz,"Content=%s,ActiveConfig=New,",contentStr);

		for(i=0;i<ht_topo->count;i++){
			get_ent_from_HashTabNum((void**)&topo_htentry, hashtab_topo, linkIDs[i]);
//			if(!topo_htentry){
//				PMprintDEB("");printfc(red,"BUG detected!");printf(" Couldn't read Link-ID \"0x%08X\" (Dev1: \"%d\", Dev2: \"%d\") out of Hash-Table (Key not existent).\n",linkIDs[i], (linkIDs[i] & BITMASK_Dev1)>>16, linkIDs[i] & BITMASK_Dev2);
//				exit(1);
//			}
			tmpp+=snprintf(tmpp,strlen("Dev1=,")+getDigitCountofInt(topo_htentry->Dev1)+1,"Dev1=%d,",topo_htentry->Dev1);
			tmpp+=snprintf(tmpp,strlen("Dev2=,")+getDigitCountofInt(topo_htentry->Dev2)+1,"Dev2=%d,",topo_htentry->Dev2);
			tmpp+=snprintf(tmpp,strlen("Status=,")+1+1,"Status=%d,",(topo_htentry->status == phy_topo_link_status__inactive) ? 0 : ((topo_htentry->status == phy_topo_link_status__active) ? 1 : -1 ) );
		}

		(*msg)[*msgsiz]='\0';

		return err;
		#undef TMPSIZ
	}

	//====================================================================================
	//====================================================================================
	//==  Temporary for first Compatibility Testing / Debugging
	//====  Remove later on
	//====================================================================================
					//	ARIDsC=2;
					//	ARIDs=malloc(ARIDsC*sizeof(dabo_ARID));
					//	ARIDs[0]=5000;
					//	ARIDs[1]=5010;
					//	dabo_htentry.ARID=5000;
					//	dabo_htentry.ARstate=dabo_ar_state__active;
					//	dabo_htentry.ARprofile.paramType=DABO_AR_Profile_Type__CycleTime;
					//	dabo_htentry.ARprofile.val=300;
					//	DenKr_HashTabNum_insert(hashtab_dabo,dabo_htentry.ARID,&dabo_htentry, sizeof(dabo_htentry));
					//	// - - - - - -
					//	dabo_htentry.ARID=5010;
					//	dabo_htentry.ARstate=dabo_ar_state__inactive;
					//	dabo_htentry.ARprofile.paramType=DABO_AR_Profile_Type__PktSize;
					//	dabo_htentry.ARprofile.val=8192;
					//	DenKr_HashTabNum_insert(hashtab_dabo,dabo_htentry.ARID,&dabo_htentry, sizeof(dabo_htentry));
	//====================================================================================
	//====================================================================================


	//====================================================================================
	//====================================================================================
	// For msg to CNC respectively DaBo
	char* sendmsgCNC;
	int sendmsgsizCNC;
	char* sendmsgDabo;
	int sendmsgsizDabo;
	//== After Init, first msg to CNC
	IFDEB(
		PMprintDEB("Sending to CNC (Context \"config_decision\").\n");
	)
	prepMsg_intoCNC(&sendmsgsizCNC,&sendmsgCNC,hashtab_topo,hashtab_dabo);
	DenKr_ContextBroker_sendInfo_withSize(InfBrokerIface, DenKr_InfBroker_Msg_Type__Raw, DenKr_InfBroker_Msg_SubType__Raw__Plugin_IntoCNC, 0, "config_decision", (void*)sendmsgCNC, sendmsgsizCNC);
	free(sendmsgCNC);
	//== After Init, first msg to DaBo
	IFDEB(
		PMprintDEB("Sending to Dashboard (Context \"dashboard_async\" | Content \"InitDone\").\n");
	)
	prepMsg_toDaBo(&sendmsgsizDabo,&sendmsgDabo,hashtab_topo,hashtab_dabo,"InitDone");
	DenKr_ContextBroker_sendInfo_withSize(InfBrokerIface, DenKr_InfBroker_Msg_Type__KeyEqualValue_CSV, DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str, 0, "dashboard_async", (void*)sendmsgDabo, sendmsgsizDabo);
	free(sendmsgDabo);
	//====================================================================================
	//====================================================================================
	//TODO:
	int calcConfig(){
		int err=0;
		int i;
		struct PM_hashtab_topology_entry_t* topo_htentry;
		struct PM_hashtab_dashboard_entry_t* dabo_htentry;

		IFDEB(
			PMprintDEB("Info, Current physical Link Status: ");
		)
		for(i=0;i<hashtab_topo->count;i++){
			get_ent_from_HashTabNum((void**)&topo_htentry, hashtab_topo, linkIDs[i]);
			if(topo_htentry->status == phy_topo_link_status__inactive){
				IFDEB(printf("Dev1:%d|Dev2:%d|status:%d | ",(topo_htentry->linkID&BITMASK_Dev1)>>16,topo_htentry->linkID&BITMASK_Dev2,topo_htentry->status);)
//				return 1;
			}
		}
		IFDEB(puts("");)

//		for(i=0;i<hashtab_dabo->count;i++){
//			get_ent_from_HashTabNum((void**)&dabo_htentry, hashtab_dabo, ARIDs[i]);
//			if(dabo_htentry->ARID == 5010){
//				if(dabo_htentry->ARprofile.val > 12000){
//					return 2;
//				}
//			}
//		}

		return err;
	}
	//====================================================================================
	//====================================================================================


	//Permanent Operation:
	char helpstr1[16], helpstr2[16], helpstr3[16];
	SkipOpMsg:
	while(1){
		//Blocking receive
		DenKr_ContextBroker_recvmsg_socket(InfBrokerIface, &brokmsg);
		IFDEB(
			PMprintDEB("Message received. Context: ")printf("%s\n", brokmsg->context);
		)
		//Parse msg
		switch(brokmsg->head->type){
		case DenKr_InfBroker_Msg_Type__KeyEqualValue_CSV:
			DenKr_ContextBroker_parsemsg_CSV(brokmsg);
			//DenKr_ContextBroker_Debug_printmsg_CSV(brokmsg);
			break;
		default:
			IFDEB(
				PMprint();printfc(yellow,"WARNING: ");printf("Invalid Msg-Format received. Ignoring...\n");
			)
			DenKr_ContextBroker_free_msg(brokmsg);
			goto SkipOpMsg;
			break;
		}

		//『Update Memory with new content』
		//Analyse Memory and calc decision / send feedback to Dashboard
		//Switch "Sender"
		#define RET_SKIPMSG 50
		if(strcmp(brokmsg->context, "topology")==0){//From CNC
			IFDEB(
				PMprintDEB("Parse Msg with Context \"topology\".\n");
			)
			//『Decode Msg and Throw into Memory』
			if((err=recv_topology())==RET_SKIPMSG){DenKr_ContextBroker_free_msg(brokmsg); break;}//goto SkipOpMsg;
		}else if(strcmp(brokmsg->context, "dabo_alt")==0){//From Dashboard
			IFDEB(
				PMprintDEB("Parse Msg with Context \"dabo_alt\".\n");
			)
			//『Decode Msg and Throw into Memory』
			if((err=recv_dashboard())==RET_SKIPMSG){DenKr_ContextBroker_free_msg(brokmsg); break;}
		}else{
			IFDEB(
				PMprintDEB("Error. Search for: q234598gfds.\n");
			)
			exit(1);
		}
		//Fuck this: (Just directly link the value from the received msg)
		//snprintf(seqNStr,getDigitCountofInt(daboSeqN)+1,"%llu",daboSeqN);

		//err=『Calc new Config』
		IFDEB(
			PMprintDEB("Start Config-calculation Pre-Step.\n");
		)
		err=calcConfig();
		IFDEB(
			PMprintDEB("Finished Config-calculation Pre-Step. Result: ")printf("%d.\n",err);
		)
		if(err){ //No new config could be found. Hardcoded for some combinations?
			if(strcmp(brokmsg->context, "topology")==0){//Reason for new Calculation: Physically Link up or down. From CNC
					//IMPORTANT: From such a msg, DaBo needs to mark the broken Link and Display some Kind of an error (Overlay with "No Config possible", mark AR-Selection with one of these error-states)
				//////『Send to DaBo: ["Content"="TopoChange", "ActiveConfig"="NONE", "Dev1"="StrID", "Dev2"="StrID", "Status"="0-for-down/1-for-up"]』 //Careful: Everything as String  //Link-Status will most probably be down ;oP
				IFDEB(
					PMprintDEB("Sending to DaBo (Context \"dashboard_async\"). Case: No-config-found. Cause: Change in Topology (Physical).\n");
				)
				snprintf(helpstr1,getDigitCountofInt(topo_htentry.Dev1)+1,"%d",topo_htentry.Dev1);
				snprintf(helpstr2,getDigitCountofInt(topo_htentry.Dev2)+1,"%d",topo_htentry.Dev2);
				snprintf(helpstr3,3,"%d", (topo_htentry.status == phy_topo_link_status__inactive) ? 0 : ((topo_htentry.status == phy_topo_link_status__active) ? 1 : -1 ) );//-1 indicates some error. Shouldn't be possible to occur
				DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str,0,"dashboard_async","Content","TopoChange","ActiveConfig","None","Dev1",helpstr1,"Dev2",helpstr2,"Status",helpstr3,NULL);
				//////『Send to CNC: ["ID"=0]』 //ID=0 for "No working config found"?
				IFDEB(
					PMprintDEB("Sending nothing to CNC. No need for this here (Context \"config_decision_failure\"). Case: No-config-found. Cause: Change in Topology (Physical).\n");
				)
			}else if(strcmp(brokmsg->context, "dabo_alt")==0){//Reason for new Calculation: In Dashboard Checkbox or Dropdown-Attribute change. From Dashboard
					//In this case, DaBo awaits some feedback. Gets back that sent request is not feasible. DaBo can accordingly display error-state at selection
				//////『Send to DaBo: ["Content"="DaBoChange", "SequenceN"="WhatEver", "ActiveConfig"="NONE"]』 // Attach whole received DaBo msg? Attach some Sequence-Num between DaBo and PM for easier assignment of feedback?
				IFDEB(
					PMprintDEB("Sending to DaBo (Context \"dashboard_feedback\"). Case: No-config-found. Cause: Change in Dashboard (Selection).\n");
				)
				DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str,0,"dashboard_feedback","Content","DaboChange","SequenceN",daboSeqNp,"ActiveConfig","None",NULL);
				//////『Send to CNC: ["ID"=0]』 //ID=0 for "No working config found"?
				IFDEB(
					PMprintDEB("Sending nothing to CNC. No need for this here (Context \"config_decision_failure\"). Case: No-config-found. Cause: Change in Dashbaord (Selection).\n");
				)
			}
		}else{
			if(strcmp(brokmsg->context, "topology")==0){//Reason for new Calculation: Physically Link up or down. From CNC
					//Oh noes, such a message is a lot hustle for the DaBo. It should receive the new allocated config to display properly...
				//////『Send to DaBo: ["Content"="TopoChange", "ActiveConfig"="NEW", "Dev1"="StrID", "Dev2"="StrID", "Status"="0-for-down/1-for-up", "AR1"="enabled", "AR1Dropdown"="2", "AR2"="disable", "AR2Dropdown"="static", ...]』 //Careful: Everything as String  //Status will most probably be down ;oP
				IFDEB(
					PMprintDEB("Sending to DaBo (Context \"dashboard_async\"). Case: Config successfully calculated. Cause: Change in Topology (Physical).\n");
				)
				prepMsg_toDaBo(&sendmsgsizDabo,&sendmsgDabo,hashtab_topo,hashtab_dabo,"TopoChange");
				DenKr_ContextBroker_sendInfo_withSize(InfBrokerIface, DenKr_InfBroker_Msg_Type__KeyEqualValue_CSV, DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str, 0, "dashboard_async", (void*)sendmsgDabo, sendmsgsizDabo);
				free(sendmsgDabo);
				//////『Send to CNC: ["ID"=Int_WhatEverItMightBe]』
				IFDEB(
					PMprintDEB("Sending to CNC (Context \"config_decision\"). Case: Config successfully calculated. Cause: Change in Topology (Physical).\n");
				)
				prepMsg_intoCNC(&sendmsgsizCNC,&sendmsgCNC,hashtab_topo,hashtab_dabo);
				DenKr_ContextBroker_sendInfo_withSize(InfBrokerIface, DenKr_InfBroker_Msg_Type__Raw, DenKr_InfBroker_Msg_SubType__Raw__Plugin_IntoCNC, 0, "config_decision", (void*)sendmsgCNC, sendmsgsizCNC);
				free(sendmsgCNC);
			}else if(strcmp(brokmsg->context, "dabo_alt")==0){//Reason for new Calculation: In Dashboard Checkbox or Dropdown-Attribute change. From Dashboard
					//Should be easy, if I'm not mistaken. Just send "success"
				//////『Send to DaBo: ["Content"="DaBoChange", "SequenceN"="WhatEver", "ActiveConfig"="Success"]』 // Again: Attach whole recv-message? Attach Sequence-Num?
				IFDEB(
					PMprintDEB("Sending to DaBo (Context \"dashboard_feedback\"). Case: Config successfully calculated. Cause: Change in Dashboard (Selection).\n");
				)
				DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__str,0,"dashboard_feedback","Content","DaboChange","SequenceN",daboSeqNp,"ActiveConfig","Success",NULL);
				//////『Send to CNC: ["ID"=Int_WhatEverItMightBe]』 //ID=0 for "No working config found"?
				IFDEB(
					PMprintDEB("Sending to CNC (Context \"config_decision\"). Case: Config successfully calculated. Cause: Change in Dashboard (Selection).\n");
				)
				prepMsg_intoCNC(&sendmsgsizCNC,&sendmsgCNC,hashtab_topo,hashtab_dabo);
				DenKr_ContextBroker_sendInfo_withSize(InfBrokerIface, DenKr_InfBroker_Msg_Type__Raw, DenKr_InfBroker_Msg_SubType__Raw__Plugin_IntoCNC, 0, "config_decision", (void*)sendmsgCNC, sendmsgsizCNC);
				free(sendmsgCNC);
			}
		}

		DenKr_ContextBroker_free_msg(brokmsg);
		IFDEB(
			PMprintDEB("Iteration done. Waiting for new message.\n");
		)
	}


					/*
					//Blocking receive
					DenKr_ContextBroker_recvmsg_socket(InfBrokerIface, &brokmsg);
					//Parse msg
					//Don't forget to use "DenKr_ContextBroker_free_msg(brokmsg);" at the right places. Here not included because of readability.
					switch(brokmsg->head->type){
					case DenKr_InfBroker_Msg_Type__KeyEqualValue_CSV:
						DenKr_ContextBroker_parsemsg_CSV(brokmsg);
						//DenKr_ContextBroker_Debug_printmsg_CSV(brokmsg);
						break;
					default:
						break;
					}
					//////『Update Memory with new content』
					//Analyse Memory and calc decision / send feedback to Dashboard
					//How to analyse stuff? Fällt dir ein krass geiler Move ein, den ich übersehe?
					// - Permutations-LUT
					// - Single-Check-If-Ladder
					// - Pattern-Matching
					// - Hm, ne kombinatorisch, analytische Funktion wär natürlich nice aber erneut: Who cares here...
					//   -> Also wieder, keep it simple: Wills noch nicht mal Pattern Matching nennen. Ganz low level: Die einzelnen Zustände binär zu nem Zahlwert kombinieren und in eine switch-LUT jagen?
					//////err=『Calc new Config』
					if(err){ //No new config could be found. Hardcoded for some combinations?
						switch(msgType/Src){
						case TopoLinkChange: //Physically Link up or down
								//IMPORTANT: From such a msg, DaBo needs to mark the broken Link and Display some Kind of an error (Overlay with "No Config possible", mark AR-Selection with one of these error-states)
							//////『Send to DaBo: ["Content"="TopoChange", "ActiveConfig"="NONE", "Dev1"="StrID", "Dev2"="StrID", "Status"="0-for-down/1-for-up"]』 //Careful: Everything as String  //Link-Status will most probably be down ;oP
							//////『Send to CNC: ["ID"=0]』 //ID=0 for "No working config found"?
							break;
						case DaBoChange: //In Dashboard Checkbox or Dropdown-Attribute change
								//In this case, DaBo awaits some feedback. Gets back that sent request is not feasible. DaBo can accordingly display error-state at selection
							//////『Send to DaBo: ["Content"="DaBoChange", "SequenceN"="WhatEver", "ActiveConfig"="NONE"]』 // Attach whole received DaBo msg? Attach some Sequence-Num between DaBo and PM for easier assignment of feedback?
							//////『Send to CNC: ["ID"=0]』 //ID=0 for "No working config found"?
							break;
						}
					}else{ //Combination found
						switch(msgType/Src){
						case TopoLinkChange: //Physically Link up or down
								//Oh noes, such a message is a lot hustle for the DaBo. It should receive the new allocated config to display properly...
							//////『Send to DaBo: ["Content"="TopoChange", "ActiveConfig"="NEW", "Dev1"="StrID", "Dev2"="StrID", "Status"="0-for-down/1-for-up", "AR1"="enabled", "AR1Dropdown"="2", "AR2"="disable", "AR2Dropdown"="static", ...]』 //Careful: Everything as String  //Status will most probably be down ;oP
							//////『Send to CNC: ["ID"=Int_WhatEverItMightBe]』
							break;
						case DaBoChange: //In Dashboard Checkbox or Dropdown-Attribute change
								//Should be easy, if I'm not mistaken. Just send "success"
							//////『Send to DaBo: ["Content"="DaBoChange", "SequenceN"="WhatEver", "ActiveConfig"="Success"]』 // Again: Attach whole recv-message? Attach Sequence-Num?
							//////『Send to CNC: ["ID"=Int_WhatEverItMightBe]』 //ID=0 for "No working config found"?
							break;
						}
					}
					//Gut, jetzt könnte man noch eine Glättungslogik einbauen, um ein config-Zittern bei zeitlich schnell folgenden Änderungen zu verhindern...
					*/


    //==================================================================================================//
    //----  Module-Termination  ------------------------------------------------------------------------//
    //==================================================================================================//
	//ModuleTermination:

	if(linkIDs)
		free(linkIDs);
	if(ARIDs)
		free(ARIDs);

	printfc(yellow,"IMPORTANT:");
	printfc(DENKR__COLOR_ANSI__THREAD_BASIC," Thread ");
	printf("is closing: ");
	printfc(DENKR__COLOR_ANSI__DB_CONNECTOR,STRINGIFY(C_PREF)"(FIND-CoC-PolicyManager).\n");
	return NULL;
}


int CAT(DENKR_PLUGIN_DISC_INIT_FUNC_PREFIX,C_PREF)(PluginManager *plugman)
{
//	int err;
	int roletype=DenKr_plugin_role__generic;
	char role[]="FIND_CoC_PolicyManager";

	PluginManager_register_role_generic(plugman, roletype, role, strlen(role)+1, DenKr_plugin_working_type__thread, NULL, DenKr_plugin_interComm_method__infBroker, CAT(hook_role_thread_,C_PREF));

	return 0;//return DenKr_plugin_role__generic;//This return isn't really used anymore. This is intrinsically done by registration-function
}

int CAT(CAT(DENKR_PLUGIN_DISC_INIT_FUNC_PREFIX,C_PREF),_DEBUG)(PluginManager *plugman)
{
	int err;
	err = CAT(DENKR_PLUGIN_DISC_INIT_FUNC_PREFIX,C_PREF)(plugman);
	return err;
}



#undef C_PREF


/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * "Notes - DenKrement, plugin, Core, PolicyManager - PseudoCode.c"

General Thoughts
	Send Key-Value-Pairs (Using the CSV functions in C. Comfortable handling in Python is currently under work (Janis/Dennis) )
	For sending to CNC: Reserve ID=0 for "No working config found"
	Attach some Sequence-Num between DaBo and PM for easier assignment of feedback?

Register for Contexts
	"topology"  – receives Information about the Topology
	"dashboard"  – receives Information conceived directly for the Dashboard
	Split into "dashboard_feedback", "dashboard_async" for the two different triggers of messages to the Dashboard?
	"config_decision"  – receives the decisions from the PolicyManager
	"dabo_alt"  – Dashboard_Alteration. Sent by the Dashboard on change of UI-Selection




//Message Format v2:
//Topology:
//   Key-Value-Pair subsequent Tupels
//   One single Msg per Link
//Example:
//   Link 1, Up | Link 2, Down
ExampleMsg1 = [
	"Dev1" = 3566,
	"Dev2" = 3567,
	"Status" = 1,
	"TotalLinkNum" = 26
]
ExampleMsg2 = [
	"Dev1" = 3742,
	"Dev2" = 3892,
	"Status" = 0,
	"TotalLinkNum" = 26
]
//Function-Call:
DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"topology","Dev1",3566,"Dev2",3567,"Status",1,"TotalLinkNum",26,NULL);
DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"topology","Dev1",3742,"Dev2",3892,"Status",1,"TotalLinkNum",26,,NULL);

//Config-Decision (down to CNC)
Example = [
	"ID" = 15,
]
DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"config-rollout","ID",(int)(WhateverItMightBe),NULL);

//Feedback to Dashboard
//Needs to get:
// - Link is physically broken
// - Requested AR/Dropdown combination not feasible
//See below at the Pseudo-Code PM function for some thoughts
//Multiple Cases:
//1) DaBo requested, awaits feedback
[
	"Content"="DaBoChange",
	"SequenceN"="WhatEver",
	"ActiveConfig"="Success",
]
//Or on failure
[
	"Content"="DaBoChange",
	"SequenceN"="WhatEver",
	"ActiveConfig"="None",
]
//2)Physically Topology changed, config calc was a success
[
	"Content"="TopoChange",
	"ActiveConfig"="NEW",
	"Dev1"="StrID",
	"Dev2"="StrID",
	"Status"="0-for-down/1-for-up",
	"AR1"="enabled",
	"AR1Dropdown"="2",
	"AR2"="disable",
	"AR2Dropdown"="static",
	...
]
//Or on calc-failure
[
	"Content"="TopoChange",
	"ActiveConfig"="NONE",
	"Dev1"="StrID",
	"Dev2"="StrID",
	"Status"="0-for-down/1-for-up"
]


// Selection change from Dash-Board
//  Always send the whole list of Input-Parameters
[
	"Content"="TopoChange",
	"SequenceN"="WhatEver",
	"AR1"="enabled",
	"AR1Dropdown"="2",
	"AR2"="disable",
	"AR2Dropdown"="static",
	...
]

//====================================================================================
//====================================================================================
//====================================================================================


//Message Format v1:
//Topology:
//   Key-Value-Pair subsequent Tupels
//Example:
//   Link 1, Up | Link 2, Down
Example = [
	"Dev1" = 3566,
	"Dev2" = 3567,
	"Status" = 1,
	"Dev1" = 3742,
	"Dev2" = 3892,
	"Status" = 0,
]
//Function-Call:
DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"topology","Dev1",3566,"Dev2",3567,"Status",1,"Dev1",3742,"Dev2",3892,"Status",0,NULL);

//Config-Decision (down to CNC)
//Ehh, we could do some RAW_SEND and fill in only a uint32_t with the config-ID. But therefor no Parse-func is defined yet.
//Soooo, keep it simple (and inefficient, but honestly who cares here...) and just also send a Key value Pair.
Example = [
	"ID" = 15,
]
DenKr_ContextBroker_sendInfo_CSV(InfBrokerIface,DenKr_InfBroker_Msg_SubType__KeyEqualValue_CSV__int,0,"config-rollout","ID",(int)(WhateverItMightBe),NULL);

//Feedback to Dashboard
//Needs to get:
// - Link is physically broken
// - Requested AR/Dropdown combination not feasible
//See below at the Pseudo-Code PM function for some thoughts
//Multiple Cases:
//1) DaBo requested, awaits feedback
[
	"Content"="DaBoChange",
	"SequenceN"="WhatEver",
	"ActiveConfig"="Success",
]
//Or on failure
[
	"Content"="DaBoChange",
	"SequenceN"="WhatEver",
	"ActiveConfig"="None",
]
//2)Physically Topology changed, config calc was a success
[
	"Content"="TopoChange",
	"ActiveConfig"="NEW",
	"Dev1"="StrID",
	"Dev2"="StrID",
	"Status"="0-for-down/1-for-up",
	"AR1"="enabled",
	"AR1Dropdown"="2",
	"AR2"="disable",
	"AR2Dropdown"="static",
	...
]
//Or on calc-failure
[
	"Content"="TopoChange",
	"ActiveConfig"="NONE",
	"Dev1"="StrID",
	"Dev2"="StrID",
	"Status"="0-for-down/1-for-up"
]

//====================================================================================
//====================================================================================
//====================================================================================

//Obviously, this whole information processing and communication sequence can cause race-conditions, that actually should be handled, IF this would become some product of worth. But like always, our credo here: Wayne...


//Problem: Pair Permutation
//   - What, if one plugin swaps the Device-IDs, compared to another plugin? -> Makes checks more complicated.
//Solution: Convention: Assign Lower ID to "Dev1", Higher ID to "Dev2"
//Can we ensure that all Developers abide by the convention? Oooobviously not... -.-
//Could we provide a "Sending-Function" that all plugins have to use? Sure, we could... but...
//  -> Can we assume that all Developers actually use this function to pack their messages? Agaaaaain not... -.-
//So fuck it. Always expect the DAU. Some Kind of a MIN-Sort.
//
// Approach to work with the two Dev-IDs the easy way:
//   Make one value out of two:
//   - MIN-Sort the two Dev-IDs
//   - Create a binary-concatenated value
//      -> (binary) left-shift Dev1
//      -> Logical-Bitwise-Or Dev1 & Dev2







// "Stateless FSM", Initialization ("Heat-Up Phase") through Flow-Control.
//Blocking Reception -> Event-driven calculation
//Register via socket => receive on socket -> do magic -> receive on socket -> ...
//NOTE: One could insert some additional logic during Start-Up-Phase to ensure that the first All-Topology-Complete-Message from the CNC was received; aka to ensure that none of the first messages during start-up are lost. But this shouldn't the possible anyway, due to my infamous program structure ;oP
//      Important to do for the pluging Developers: Use the DenKr_Thread_Ready_Init() Function, register for contexts beforehand and "really operate" afterwards.
//WARUM schreib ich den Stuss eigentlich die ganze Zeit auf Englisch? Code Kommentare schreib ich zwar immer auf English, das hier geht aber nur an dich, Julian...
//Hem, als Gedächtnis für den Policy Manager: Einfach stupide alles in ein vordefiniertes Array/Struct werfen und damit alles quick, dirty & statisch aufbauen oder vlt. bissel was Eleganteres wie ne Hash-Table? Oder fällt dir was anderes Schönes ein?

//"Vorverarbeitung" von Topo-msgs, d.h. nach Erhalt der msg und vor Eintragen in memory:
// Macht einen singulären Wert daraus.
uint16_t dev1;
uint16_t dev2;
uint32_t link = (dev1<<16) | dev2;

void* PolicyManager(void* arg){
	//Some init trash
	DenKr_generic_module_thread__startPreamble
	//Register Context to listen on
	((InfBrokerIface->FuncRegProducer).regSocket)(InfBrokerIface,"topology",NULL);
	((InfBrokerIface->FuncRegProducer).regSocket)(InfBrokerIface,"dashboard_changes",NULL);
	//Tell Main, that i am Ready and than wait for Main, to tell me, that everyone is Ready.
	DenKr_Thread_Ready_Init(shmem_headers, mainThreadID, ownID)
	//==================================================================================================//
	//----  Init finished, actual Module-Execution  ----------------------------------------------------//
	//==================================================================================================//

	DenKr_InfBroker_msg* brokmsg;

	//Start-Up Phase
	{//Scope to eliminate temporary variables from Start-Up Phase
		StartUpPhase:
		int state=0;
		//Blocking receive
		DenKr_ContextBroker_recvmsg_socket(InfBrokerIface, &brokmsg);
		//Parse msg
		//Don't forget to use "DenKr_ContextBroker_free_msg(brokmsg);" at the right places. Here not included because of readability.
		switch(brokmsg->head->type){
		case DenKr_InfBroker_Msg_Type__KeyEqualValue_CSV:
			DenKr_ContextBroker_parsemsg_CSV(brokmsg);
			//DenKr_ContextBroker_Debug_printmsg_CSV(brokmsg);
			break;
		default:
			break;
		}

		switch(Sender){
		case CNC:
			//Maybe check for completeness (i.e. is it only a single-link msg that indicates a change or is what we actually want here: The whole topology.
			『Throw into Memory』
			if(state == DashboardAlreadyGot){
				break;
			}else{
				state=CNCAlreadyGot;
				goto StartUpPhase;
			}
			break;
		case Dashboard:
			//Same thing to check for completeness: We need state of all AR-Checkboxes and selection of varying dropdown-lists
			『Throw into Memory』
			if(state == CNCAlreadyGot){
				break;
			}else{
				state=DashboardAlreadyGot;
				goto StartUpPhase;
			}
			break;
		}
	}
	//When init messages of both sources are received: Go over to permanent operation

	//Permanent Operation:
	while(1){
		//Blocking receive
		DenKr_ContextBroker_recvmsg_socket(InfBrokerIface, &brokmsg);
		//Parse msg
		//Don't forget to use "DenKr_ContextBroker_free_msg(brokmsg);" at the right places. Here not included because of readability.
		switch(brokmsg->head->type){
		case DenKr_InfBroker_Msg_Type__KeyEqualValue_CSV:
			DenKr_ContextBroker_parsemsg_CSV(brokmsg);
			//DenKr_ContextBroker_Debug_printmsg_CSV(brokmsg);
			break;
		default:
			break;
		}
		『Update Memory with new content』
		//Analyse Memory and calc decision / send feedback to Dashboard
		//How to analyse stuff? Fällt dir ein krass geiler Move ein, den ich übersehe?
		// - Permutations-LUT
		// - Single-Check-If-Ladder
		// - Pattern-Matching
		// - Hm, ne kombinatorisch, analytische Funktion wär natürlich nice aber erneut: Who cares here...
		//   -> Also wieder, keep it simple: Wills noch nicht mal Pattern Matching nennen. Ganz low level: Die einzelnen Zustände binär zu nem Zahlwert kombinieren und in eine switch-LUT jagen?
		err=『Calc new Config』
		if(err){ //No new config could be found. Hardcoded for some combinations?
			switch(msgType/Src){
			case TopoLinkChange: //Physically Link up or down
					//IMPORTANT: From such a msg, DaBo needs to mark the broken Link and Display some Kind of an error (Overlay with "No Config possible", mark AR-Selection with one of these error-states)
				『Send to DaBo: ["Content"="TopoChange", "ActiveConfig"="NONE", "Dev1"="StrID", "Dev2"="StrID", "Status"="0-for-down/1-for-up"]』 //Careful: Everything as String  //Link-Status will most probably be down ;oP
				『Send to CNC: ["ID"=0]』 //ID=0 for "No working config found"?
				break;
			case DaBoChange: //In Dashboard Checkbox or Dropdown-Attribute change
					//In this case, DaBo awaits some feedback. Gets back that sent request is not feasible. DaBo can accordingly display error-state at selection
				『Send to DaBo: ["Content"="DaBoChange", "SequenceN"="WhatEver", "ActiveConfig"="NONE"]』 // Attach whole received DaBo msg? Attach some Sequence-Num between DaBo and PM for easier assignment of feedback?
				『Send to CNC: ["ID"=0]』 //ID=0 for "No working config found"?
				break;
			}
		}else{ //Combination found
			switch(msgType/Src){
			case TopoLinkChange: //Physically Link up or down
					//Oh noes, such a message is a lot hustle for the DaBo. It should receive the new allocated config to display properly...
				『Send to DaBo: ["Content"="TopoChange", "ActiveConfig"="NEW", "Dev1"="StrID", "Dev2"="StrID", "Status"="0-for-down/1-for-up", "AR1"="enabled", "AR1Dropdown"="2", "AR2"="disable", "AR2Dropdown"="static", ...]』 //Careful: Everything as String  //Status will most probably be down ;oP
				『Send to CNC: ["ID"=Int_WhatEverItMightBe]』
				break;
			case DaBoChange: //In Dashboard Checkbox or Dropdown-Attribute change
					//Should be easy, if I'm not mistaken. Just send "success"
				『Send to DaBo: ["Content"="DaBoChange", "SequenceN"="WhatEver", "ActiveConfig"="Success"]』 // Again: Attach whole recv-message? Attach Sequence-Num?
				『Send to CNC: ["ID"=Int_WhatEverItMightBe]』 //ID=0 for "No working config found"?
				break;
			}
		}
		//Gut, jetzt könnte man noch eine Glättungslogik einbauen, um ein config-Zittern bei zeitlich schnell folgenden Änderungen zu verhindern...
	}
}

 */
