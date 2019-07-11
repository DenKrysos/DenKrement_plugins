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
#include <pthread.h>
//#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
//#include <sys/stat.h>
//#include <arpa/inet.h>
//#include <resolv.h>
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
#include "DenKr_essentials/InterProgram_Communication/InterProgram_Communication.h"
//#include "nl80211.h"
//---------------------------------------------------------------------------------
#include "plugins_export.h"
#include "headers/DenKrement_plugin_predefined__connect_external_Python.h"
//==================================================================================================//
//==================================================================================================//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------//
//==================================================================================================//


#ifdef DEBUG
//	#define DEBUG_module_ext_py_connect
#endif



// The "Content Prefix":
//    Put this in Front of every Function, Variable, simply every Symbol, that will
//    be visible outside, to make it unique and thus avoid clashes.
//  Set it identical to the File-Name.
#define C_PREF DenKrement_plugin_predefined__connect_external_Python

#define DENKREMENT_SOCKET_C_PYTHON "\0DenKrement_SOCKET_Python_C"



void CONCAT(C_PREF, _settings_init())
{

}


void CONCAT(C_PREF, _gui_window_init())
{

}




static int CAT(C_PREF,_recv_head_from_Py)(int sock_Py, struct DenKr_InfBroker_Msg_Header* msgh){
	int err;
	int b_recvd;
	err=recvdetermined(&b_recvd,sock_Py,&(msgh->type),member_size(struct DenKr_InfBroker_Msg_Header,type),0);
	switch(err){
	case 1://proper operation
		break;
	case 0://socket shutdown at counterside
		return 0;
	case -1://recv error, check ERRNO
		return -1;
	}
	err=recvdetermined(&b_recvd,sock_Py,&(msgh->subtype),member_size(struct DenKr_InfBroker_Msg_Header,subtype),0);
	switch(err){
	case 1://proper operation
		break;
	case 0://socket shutdown at counterside
		return 0;
	case -1://recv error, check ERRNO
		return -1;
	}
	err=recvdetermined(&b_recvd,sock_Py,&(msgh->src),member_size(struct DenKr_InfBroker_Msg_Header,src),0);
	switch(err){
	case 1://proper operation
		break;
	case 0://socket shutdown at counterside
		return 0;
	case -1://recv error, check ERRNO
		return -1;
	}
	err=recvdetermined(&b_recvd,sock_Py,&(msgh->len),member_size(struct DenKr_InfBroker_Msg_Header,len),0);
	switch(err){
	case 1://proper operation
		break;
	case 0://socket shutdown at counterside
		return 0;
	case -1://recv error, check ERRNO
		return -1;
	}
	err=recvdetermined(&b_recvd,sock_Py,&(msgh->flags),member_size(struct DenKr_InfBroker_Msg_Header,flags),0);
	switch(err){
	case 1://proper operation
		break;
	case 0://socket shutdown at counterside
		return 0;
	case -1://recv error, check ERRNO
		return -1;
	}
	return 1;
}

static int CAT(C_PREF,_send_msg_to_Py)(int sock_Py, struct DenKr_InfBroker_Msg_Header* msgh, void* msg){
	//	printf("size %d\naddr1 %d\naddr2 %d\naddr3 %d\naddr4 %d\naddr5 %d\n\n",sizeof(struct DenKr_InfBroker_Msg_Header),&(((struct DenKr_InfBroker_Msg_Header*)test_msg)->type),&(((struct DenKr_InfBroker_Msg_Header*)test_msg)->subtype),&(((struct DenKr_InfBroker_Msg_Header*)test_msg)->src),&(((struct DenKr_InfBroker_Msg_Header*)test_msg)->len),&(((struct DenKr_InfBroker_Msg_Header*)test_msg)->flags));
		int headsiz=member_size(struct DenKr_InfBroker_Msg_Header,type)+member_size(struct DenKr_InfBroker_Msg_Header,subtype)+member_size(struct DenKr_InfBroker_Msg_Header,src)+member_size(struct DenKr_InfBroker_Msg_Header,len)+member_size(struct DenKr_InfBroker_Msg_Header,flags);
		char* send_msg=malloc(headsiz+msgh->len);
		char* send_msg_wr=send_msg;
		*((DenKr_InfBroker_Msg_Header__type*)send_msg_wr)=msgh->type;
		send_msg_wr+=member_size(struct DenKr_InfBroker_Msg_Header,type);
		*((DenKr_InfBroker_Msg_Header__subtype*)send_msg_wr)=msgh->subtype;
		send_msg_wr+=member_size(struct DenKr_InfBroker_Msg_Header,subtype);
		*((DenKr_essentials_ThreadID*)send_msg_wr)=msgh->src;
		send_msg_wr+=member_size(struct DenKr_InfBroker_Msg_Header,src);
		*((DenKr_InfBroker_Msg_Header__msglen*)send_msg_wr)=msgh->len;
		send_msg_wr+=member_size(struct DenKr_InfBroker_Msg_Header,len);
		*((DenKr_InfBroker_Msg_Header__flags*)send_msg_wr)=msgh->flags;
		send_msg_wr+=member_size(struct DenKr_InfBroker_Msg_Header,flags);
		memcpy(send_msg_wr,(char*)msg,msgh->len);
		senddetermined(sock_Py,send_msg,headsiz+msgh->len);
		if(send_msg)
			free(send_msg);
		return 0;
}






//static void* CAT(C_PREF,_from_InfBrok_passthrough_cb)(void* arg)
//{
//
//}



//TODO: When disconnect:
//   - destroy socket
//   - Remove Registration from Context-Broker
//   - Remove registered Generic Plugin
//   - Terminate Thread
//TODO: VERY IMPORTANT!!
// Clear the Callback-Registrations (Remove Producer/Consumer Functions), before exiting the Thread.
//   Otherwise the Broker trys to call the nested Functions (which then do not exist...)
//   and everything breaks down to Hell...
//  -> Some kind of own tracking about which contextes are registered and then finally clear them.
//
//For this purpose here just use the exact same Messages / Message-Format as for the Context-Broker
static void* CAT(C_PREF,_connect_translate__Thread)(void* arg)
{
	#define CONTEXT_TRACK_SIZE_ADDITION 10
	char** contextTrackCons = malloc(CONTEXT_TRACK_SIZE_ADDITION);
	int contextTrackConsArrSize = CONTEXT_TRACK_SIZE_ADDITION;
	memset(contextTrackCons,0,contextTrackConsArrSize);
	int contextTrackConsCurr = 0;//Used NOT as index, but actual Counter for entries. I.e: This -1 is the number of the last valid index
	void** callbackTrackCons = malloc(CONTEXT_TRACK_SIZE_ADDITION);
	memset(callbackTrackCons,0,contextTrackConsArrSize);
	// - - - - - - - -
	char** contextTrackProd = malloc(CONTEXT_TRACK_SIZE_ADDITION);
	int contextTrackProdArrSize = CONTEXT_TRACK_SIZE_ADDITION;
	memset(contextTrackProd,0,contextTrackProdArrSize);
	int contextTrackProdCurr = 0;
	void** callbackTrackProd = malloc(CONTEXT_TRACK_SIZE_ADDITION);
	memset(callbackTrackProd,0,contextTrackProdArrSize);
	// - - - - - - - -
	int sock_Py;
	sock_Py=*((int*)(((struct thread_generic_module_start_ThreadArgPassing *)arg)->additional));
	DenKr_generic_module_thread__startPreamble_noPrint
	int err;

	//CREATE_PROGRAM_PATH_REUSE(*argvg);

	//==================================================================================================//
	//----  Context-Registrations (at least the initial ones...)  --------------------------------------//
	//----       Could, if desired, surely change over Runtime  ----------------------------------------//
	//==================================================================================================//
	//	((InfBrokerIface->FuncRegConsumer).regSocket)(InfBrokerIface,"test_consumer_socket","cake","SDN","Schedule",NULL);
	//==================================================================================================//

	printfc(DENKR__COLOR_ANSI__THREAD_BASIC,"Thread:");printf("Python-Module-Connector: Python-Module Connected and Initialized.\n");

	//==================================================================================================//
	//----  Init finished, actual Module-Execution  ----------------------------------------------------//
	//--------------------------------------------------------------------------------------------------//
	//==================================================================================================//

	//--------------------------------------------------------------------------------------------------//
	void* CAT(C_PREF,_from_InfBrok_passthrough_cb)(void* arg)
	{
//		senddetermined(sock_Py,(char*)arg,sizeof(struct DenKr_InfBroker_Msg_Header)+(((struct DenKr_InfBroker_Msg_Header*)arg)->len));
		CAT(C_PREF,_send_msg_to_Py)(sock_Py,(struct DenKr_InfBroker_Msg_Header*)arg,(void*)(arg+sizeof(struct DenKr_InfBroker_Msg_Header)));
		free(arg);
		return NULL;
	}
	//--------------------------------------------------------------------------------------------------//

	//First Test
	/*
//	printf("size %d\naddr1 %d\naddr2 %d\naddr3 %d\naddr4 %d\naddr5 %d\n\n",sizeof(struct DenKr_InfBroker_Msg_Header),&(((struct DenKr_InfBroker_Msg_Header*)test_msg)->type),&(((struct DenKr_InfBroker_Msg_Header*)test_msg)->subtype),&(((struct DenKr_InfBroker_Msg_Header*)test_msg)->src),&(((struct DenKr_InfBroker_Msg_Header*)test_msg)->len),&(((struct DenKr_InfBroker_Msg_Header*)test_msg)->flags));
	int payloadsiz=8;
	int headsiz=member_size(struct DenKr_InfBroker_Msg_Header,type)+member_size(struct DenKr_InfBroker_Msg_Header,subtype)+member_size(struct DenKr_InfBroker_Msg_Header,src)+member_size(struct DenKr_InfBroker_Msg_Header,len)+member_size(struct DenKr_InfBroker_Msg_Header,flags);
	char* test_msg=malloc(headsiz+payloadsiz);
	char* test_msg_wr=test_msg;
//	memset(test_msg,0xFF,sizeof(struct DenKr_InfBroker_Msg_Header)+8);
	*((DenKr_InfBroker_Msg_Header__type*)test_msg_wr)=2;
	test_msg_wr+=member_size(struct DenKr_InfBroker_Msg_Header,type);
	*((DenKr_InfBroker_Msg_Header__subtype*)test_msg_wr)=3;
	test_msg_wr+=member_size(struct DenKr_InfBroker_Msg_Header,subtype);
	*((DenKr_essentials_ThreadID*)test_msg_wr)=1;
	test_msg_wr+=member_size(struct DenKr_InfBroker_Msg_Header,src);
	*((DenKr_InfBroker_Msg_Header__msglen*)test_msg_wr)=payloadsiz;
	test_msg_wr+=member_size(struct DenKr_InfBroker_Msg_Header,len);
	*((DenKr_InfBroker_Msg_Header__flags*)test_msg_wr)=0;
	test_msg_wr+=member_size(struct DenKr_InfBroker_Msg_Header,flags);
	*((uint32_t*)test_msg_wr)=6;
	*((uint32_t*)(test_msg_wr+4))=9;
	senddetermined(sock_Py,test_msg,headsiz+payloadsiz);
//	free(test_msg);
	*/

	int b_recvd;
	struct DenKr_InfBroker_Msg_Header msgh;
	char* msg=NULL;
	while(1){
//		err=recvdetermined(&b_recvd,sock_Py,&msgh,sizeof(msgh),0);
		err=CAT(C_PREF,_recv_head_from_Py)(sock_Py,&msgh);
			#ifdef DEBUG_module_ext_py_connect
				printf("\nTest, Header:\n");
				printf("%d | %d | %d | %d | %d\n",msgh.type,msgh.subtype,msgh.src,msgh.len,msgh.flags);
			#endif
		switch(err){
		case 1://proper operation
			//Header is received. Now use the 'len' to get the actual msg and Mux the Types.
			if(msgh.len){
				msg=malloc(msgh.len);
				err=recvdetermined(&b_recvd,sock_Py,msg,msgh.len,0);
				switch(err){
				case 1://proper operation
					break;
				case 0://socket shutdown at counterside
					//TODO
					printfc(yellow,"\nSocket Shutdown at Counterside inside Python-Module-Connector (1).\n");
					goto Termination;
					break;
				case -1://recv error, check ERRNO
					//TODO
					printfc(red,"\n!ERROR!!!ERROR! TODO. Receive Error inside Python-Module-Connector 2.\n");
					goto Termination;
					break;
				}
			}
			switch(msgh.type){
			case DenKr_InfBroker_Msg_Type__Management:
				;
				char* temp;
				char* context=NULL;
				switch(msgh.subtype){
				case DenKr_InfBroker_Msg_SubType__Management__RegConsumerSocket:
				case DenKr_InfBroker_Msg_SubType__Management__RegConsumerCallback:
//					DenKr_ContextBroker_register_Consumer_callback(InfBrokerIface, CAT(C_PREF,_from_InfBrok_passthrough_cb), msg);
					temp=malloc(msgh.len+sizeof(InfBrok_Func_Callback));
					*((void**)temp)=CAT(C_PREF,_from_InfBrok_passthrough_cb);
					memcpy(temp+sizeof(InfBrok_Func_Callback),msg,msgh.len);
					DenKr_ContextBroker_send_noContext(InfBrokerIface,DenKr_InfBroker_Msg_Type__Management,DenKr_InfBroker_Msg_SubType__Management__RegConsumerCallback,msgh.flags,temp,msgh.len+sizeof(InfBrok_Func_Callback));
					if(temp)
						free(temp);
					//Keep track of the registered contexts.
					// In the current implementation it is not necessary to keep also track of the registered callback, since there is only one, but hey... no one knows what will come...
					// TODO: Conditionally scale down the arrays or switch to HastTable
					// NOTE: To-Do, if future change: Change the written in callback-Adress
					// TODO: Betterment would be to memset all empty places to '0' and to check accordingly. But the case shouldn't occur, that a place entry will be checked if it is empty, since the counter is only incrementally used
//					printf("\ntest\n%s\n",msg);
					context=msg;
					if(contextTrackConsCurr >= contextTrackConsArrSize){
						contextTrackConsArrSize=CONTEXT_TRACK_SIZE_ADDITION;
						contextTrackCons=realloc(contextTrackCons, contextTrackConsArrSize);
						callbackTrackCons=realloc(callbackTrackCons, contextTrackConsArrSize);
					}
					while('\0'!=*context){
						contextTrackCons[contextTrackConsCurr]=malloc(strlen(context)+1);
						strcpy(contextTrackCons[contextTrackConsCurr], context);
						callbackTrackCons[contextTrackConsCurr]=CAT(C_PREF,_from_InfBrok_passthrough_cb);
//						depr(1,"context: %s, CurrCount % d\n",context, contextTrackConsCurr);
						contextTrackConsCurr++;
						for(;*context!='\0';context++){}context++;
					}
//					printf("-->> Verlassen der Loop: Count %d\n",contextTrackConsCurr);
					break;
				case DenKr_InfBroker_Msg_SubType__Management__RegProducerSocket:
				case DenKr_InfBroker_Msg_SubType__Management__RegProducerCallback:
//					DenKr_ContextBroker_register_Producer_callback(InfBrokerIface, CAT(C_PREF,_from_InfBrok_passthrough_cb), msg);
					temp=malloc(msgh.len+sizeof(InfBrok_Func_Callback));
					*((void**)temp)=CAT(C_PREF,_from_InfBrok_passthrough_cb);
					memcpy(temp+sizeof(InfBrok_Func_Callback),msg,msgh.len);
					DenKr_ContextBroker_send_noContext(InfBrokerIface,DenKr_InfBroker_Msg_Type__Management,DenKr_InfBroker_Msg_SubType__Management__RegProducerCallback,msgh.flags,temp,msgh.len+sizeof(InfBrok_Func_Callback));
					if(temp)
						free(temp);
					//See Comments from RegConsumerCallback;
					context=msg;
					if(contextTrackProdCurr >= contextTrackProdArrSize){
						contextTrackProdArrSize=CONTEXT_TRACK_SIZE_ADDITION;
						contextTrackProd=realloc(contextTrackProd, contextTrackProdArrSize);
						callbackTrackProd=realloc(callbackTrackProd, contextTrackProdArrSize);
					}
					while('\0'!=*context){
						contextTrackProd[contextTrackProdCurr]=malloc(strlen(context)+1);
						strcpy(contextTrackProd[contextTrackProdCurr], context);
						callbackTrackProd[contextTrackProdCurr]=CAT(C_PREF,_from_InfBrok_passthrough_cb);
						contextTrackProdCurr++;
						for(;*context!='\0';context++){}context++;
					}
					break;
				case DenKr_InfBroker_Msg_SubType__Management__RemoveConsumerSocket:
				case DenKr_InfBroker_Msg_SubType__Management__RemoveConsumerCallback:
					break;
				case DenKr_InfBroker_Msg_SubType__Management__RemoveProducerSocket:
				case DenKr_InfBroker_Msg_SubType__Management__RemoveProducerCallback:
					break;
				default:
					break;
				}
				break;
			default:
				DenKr_ContextBroker_send_noContext(InfBrokerIface,msgh.type,msgh.subtype,msgh.flags,msg,msgh.len);
				break;
			}
			#ifdef DEBUG_module_ext_py_connect
//				DenKr_InfBroker_msg* msg_deb=malloc(sizeof(*msg_deb));
//				msg_deb->head=&msgh;
//				msg_deb->msg=msg;
//				msg_deb->context=NULL;
//				DenKr_ContextBroker_Debug_printmsg(msg_deb);
			#endif
			if(msg){
				free(msg);
				msg=NULL;
			}
			break;
		case 0://socket shutdown at counterside
			//TODO
			printfc(yellow,"\nSocket Shutdown at Counterside inside Python-Module-Connector (4).\n");
			goto Termination;
			break;
		case -1://recv error, check ERRNO
			//TODO
			printfc(red,"\n!ERROR!!!ERROR! TODO. Receive Error inside Python-Module-Connector 3.\n");
			goto Termination;
			break;
		}
	}



    //==================================================================================================//
    //----  Module-Termination  ------------------------------------------------------------------------//
    //==================================================================================================//

	Termination:
	printfc(yellow,"IMPORTANT:");
	printfc(DENKR__COLOR_ANSI__THREAD_BASIC," Thread ");
	printf("is attempting to close: "STRINGIFY(C_PREF)". Sub-Thread-ID: TODO.\n");
	//Unregister the Callbacks for the tracked contexts
	//TODO: Adjust sent callback, if changed in the future to be using different ones.
	int i;
	int contlen=0;
	char* currcont=NULL;
	char* sendstr=NULL;
	if(contextTrackConsCurr){
		for(i=0;i<contextTrackConsCurr;i++){
			contlen+=strlen(contextTrackCons[i])+1;
//			printf("test (Count: %d):  %s\n",contextTrackConsCurr,contextTrackCons[i]);
		}
		contlen++;
	//		printf("test contlen:  %d\n",contlen);
		contlen+=sizeof(InfBrok_Func_Callback);
	//		printf("test contlen:  %d\n",contlen);
		sendstr=malloc(contlen);
		*((InfBrok_Func_Callback*)sendstr)=CAT(C_PREF,_from_InfBrok_passthrough_cb);
		currcont=sendstr+sizeof(InfBrok_Func_Callback);
		for(i=0;i<contextTrackConsCurr;i++){
			strcpy(currcont,contextTrackCons[i]);
			currcont+=strlen(contextTrackCons[i])+1;
			if(contextTrackCons[i])
				free(contextTrackCons[i]);
		}
		DenKr_ContextBroker_send_noContext(InfBrokerIface,DenKr_InfBroker_Msg_Type__Management,DenKr_InfBroker_Msg_SubType__Management__RemoveConsumerCallback,msgh.flags,sendstr,contlen);
		if(sendstr)
			free(sendstr);
		if(contextTrackCons)
			free(contextTrackCons);
	}
	// - - - - - - - - -
	if(contextTrackProdCurr){
		contlen=0;
		sendstr=NULL;
		for(i=0;i<contextTrackProdCurr;i++){
			contlen+=strlen(contextTrackProd[i])+1;
			printf("test:  %s\n",contextTrackCons[i]);
		}
		contlen++;
		contlen+=sizeof(InfBrok_Func_Callback);
		sendstr=malloc(contlen);
		*((InfBrok_Func_Callback*)sendstr)=CAT(C_PREF,_from_InfBrok_passthrough_cb);
		currcont=sendstr+sizeof(InfBrok_Func_Callback);
		for(i=0;i<contextTrackProdCurr;i++){
			strcpy(currcont,contextTrackProd[i]);
			currcont+=strlen(contextTrackProd[i])+1;
			if(contextTrackProd[i])
				free(contextTrackProd[i]);
		}
		DenKr_ContextBroker_send_noContext(InfBrokerIface,DenKr_InfBroker_Msg_Type__Management,DenKr_InfBroker_Msg_SubType__Management__RemoveProducerCallback,msgh.flags,sendstr,contlen);
		if(sendstr)
			free(sendstr);
		if(contextTrackProd)
			free(contextTrackProd);
	}

	WAITNS(1,0);
	printfc(yellow,"IMPORTANT:");
	printfc(DENKR__COLOR_ANSI__THREAD_BASIC," Thread ");
	printf("is closing: "STRINGIFY(C_PREF)". Sub-Thread-ID: TODO.\n");
	pthread_exit(NULL);
	return NULL;
}


void* CAT(hook_role_thread_,C_PREF)(void* arg)
{
    //==================================================================================================//
    //----  Initialization  ----------------------------------------------------------------------------//
    //==================================================================================================//
//	DenKr_generic_module_thread__startPreamble
//	int err;
	//----------------------------------------------------------------------------------------------------
	ThreadManager* thrall = (((struct ContextBroker_connect_external_Python_ThreadArgPassing *)arg)->thrall);
	PluginManager* plugman = (((struct ContextBroker_connect_external_Python_ThreadArgPassing *)arg)->plugman);
	struct ShMemHeader *shmem_headers = (((struct ContextBroker_connect_external_Python_ThreadArgPassing *)arg)->shmem_headers);
	DenKr_essentials_ThreadID ownID = (((struct ContextBroker_connect_external_Python_ThreadArgPassing *)arg)->ownID);
	DenKr_essentials_ThreadID mainThreadID = (((struct ContextBroker_connect_external_Python_ThreadArgPassing *)arg)->mainThreadID);
	DenKr_InfBroker_Iface_Client* InfBrokerIface = (DenKr_InfBroker_Iface_Client*)(((struct ContextBroker_connect_external_Python_ThreadArgPassing *)arg)->ContextBrokerInterface);
	free(arg);
	printfc(DENKR__COLOR_ANSI__THREAD_BASIC,"Thread started:");printf(" "STRINGIFY(C_PREF)" - Context-Broker Module to enable Runtime-Connection of external Python-Modules.\n");
	int err;
	#define shmem_recv_start_self ShMem_recv_start(&(shmem_headers[ownID]))
	#define shmem_recv_finish_self ShMem_recv_finish(&(shmem_headers[ownID]))
	#define shmem_self (shmem_headers[ownID])

	//CREATE_PROGRAM_PATH_REUSE(*argvg);

    //==================================================================================================//
    //----  Further Steps  -----------------------------------------------------------------------------//
    //==================================================================================================//

    //==================================================================================================//
	/*Tell Main, that i am Ready and than wait for Main, to tell me, that everyone is Ready.*/
	DenKr_Thread_Ready_Init(shmem_headers, mainThreadID, ownID)

    //==================================================================================================//
    //----  Init finished, actual Module-Execution  ----------------------------------------------------//
    //==================================================================================================//


	int s; /* connected socket descriptor */
    int socket_Python_ls;
//	socklen_t remote_Python_addrlen;
    socklen_t remote_Python_in_addrlen;
    struct sockaddr_un remote_Python = {AF_UNIX, DENKREMENT_SOCKET_C_PYTHON};
//    remote_Python_addrlen = sizeof(DENKREMENT_SOCKET_C_PYTHON)-1+sizeof(remote_Python.sun_family);
    struct sockaddr_un remote_Python_in;

    socket_Python_ls = socket(AF_UNIX, SOCK_STREAM, 0);
	if(socket_Python_ls == -1){
		printfc(red,"ERROR:");printf(" unable to create listening-socket for external Python-Modules to connect\n");
		exit(1);
	}
//	if(bind(socket_Python_ls, (struct sockaddr*)(&remote_Python), sizeof(remote_Python)) == -1){
	if(bind(socket_Python_ls, (struct sockaddr*)(&remote_Python), sizeof(DENKREMENT_SOCKET_C_PYTHON)-1+sizeof(remote_Python.sun_family)) == -1){
		printfc(red,"ERROR:");printf(" unable to bind named listening-socket for external Python-Modules to connect\n");
		exit(1);
	}
	if(listen(socket_Python_ls, 5) == -1){
		printfc(red,"ERROR:");printf(" unable to listen on listening-socket for external Python-Modules to connect\n");
		exit(1);
	}

//	remote_Python_addrlen = sizeof(struct sockaddr_in);
//	remote_Python_in_addrlen = sizeof(remote_Python_in);

	//TODO: There is currently no real internal tracking of spawned Client Threads. Kind-of over the generic Plugins, but not really independent or nicely seperatable.
	int next_client_idx=0;
	struct ContextBroker_connect_external_Python__Client_GenericPluginRole* role;
    while( (s=accept(socket_Python_ls,(struct sockaddr *)&remote_Python_in,(socklen_t*)&remote_Python_in_addrlen)) ){
        if (s<0) {
    		printfc(red,"ERROR:");printf(" at \"listening-socket for external Python-Modules to connect\": ");printfc(red,"accept failed");printf(" in acceptance loop.");printf("\n");
            return (void*)1;
        }
    	printfc(DENKR__COLOR_ANSI__THREAD_BASIC,"Thread:");printf("Python-Module-Connector: Incoming Python-Module Connection.\n");

    	role=malloc(sizeof(*role));
    	memcpy(role->RoleName,ContextBroker_connect_external_Python__Client_GenericPluginRole__Name,sizeof(ContextBroker_connect_external_Python__Client_GenericPluginRole__Name));
    	role->idx=next_client_idx;
    //	printfc(yellow,"asdf:");printf("\n%s\n%d\n\n\n",role->RoleName,sizeof(*role));

    	PluginManager_register_role_generic(plugman, (char*)role, sizeof(*role), DenKr_plugin_working_type__thread, NULL, DenKr_plugin_interComm_method__infBroker, CAT(C_PREF,_connect_translate__Thread));

//    	int i=0;
		struct PluginRoleGeneric* plugin;
		plugin=plugman->generic;
		//Yeah, you could check via 'i' if you run over the 'plugman->generic_c' Borders. But that anyhow could only result out of a Bug. So... who really cares. You would have to fix THAT primordial Bug at any rate... and not actually a Handling here...
		while(((struct ContextBroker_connect_external_Python__Client_GenericPluginRole*)(plugin->role))->idx != next_client_idx){
			plugin=plugin->next;
		}

		void* temparg=malloc(sizeof(int));
		*((int*)(temparg))=s;
		DenKr_Thread_startThread_generic(thrall, plugin, mainThreadID, plugman, shmem_headers, (InfBrokerIface->hidden).send_to_Broker, temparg, sizeof(int));
		free(temparg);

//	    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	    ShMem_send_start(&(shmem_headers[plugin->ThreadID]), sizeof(int), SHMEM_MSG_TYPE__GENERIC);
//	    //-------- content.
//		ShMem_send_finish(&(shmem_headers[plugin->ThreadID]));
//	    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    	next_client_idx++;
    }
    if (s<0) {
		printfc(red,"ERROR:");printf(" at \"listening-socket for external Python-Modules to connect\": ");printfc(red,"accept failed");printf(" breaking acceptance loop.");printf("\n");
        return (void*)1;
    }





    //==================================================================================================//
    //----  Module-Termination  ------------------------------------------------------------------------//
    //==================================================================================================//

	printfc(yellow,"IMPORTANT:");
	printfc(DENKR__COLOR_ANSI__THREAD_BASIC," Thread ");
	printf("is closing: DenKrement_plugin_predefined__connect_external_Python.\n");
	#undef shmem_recv_start_self
	#undef shmem_recv_finish_self
	#undef shmem_self
	return NULL;
}


int CAT(DENKR_PLUGIN_DISC_INIT_FUNC_PREFIX,C_PREF)(PluginManager *plugman)
{
//	int err;
	int role=DenKr_plugin_role__Context_Broker__External_Connector_Python;

	PluginManager_register_role_predefined(plugman, role, DenKr_plugin_working_type__thread, CAT(hook_role_thread_,C_PREF));

	return role;
}




#undef C_PREF
