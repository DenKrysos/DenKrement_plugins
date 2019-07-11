/*
 * DenKrement_plugin_SDN_LinkMonitoring.h
 *
 *  Created on: 25.07.2017
 *      Author: Dennis Krummacker
 */

#ifndef HEADERS__DENKREMENT_PLUGIN_PREDEFINED__CONNECT_EXTERNAL_PYTHON__H
#define HEADERS__DENKREMENT_PLUGIN_PREDEFINED__CONNECT_EXTERNAL_PYTHON__H

#ifdef __cplusplus
extern "C" {
#endif




struct ContextBroker_connect_external_Python__connect_translate__ThreadArgPassing{
	ThreadManager* thrall;//The ThreadManager. Contains the two Arrays: pthread* allThreads: Mainly to start the listen thread inside. DenKr_ThreadSpawned* runningThreads: Passed to record the Listening-Thread as running;
	PluginManager* plugman;
	struct ShMemHeader *shmem_headers;//An Array
	DenKr_essentials_ThreadID ownID;
	DenKr_essentials_ThreadID mainThreadID;
	void* ContextBrokerInterface;
};


#define ContextBroker_connect_external_Python__Client_GenericPluginRole__Name "ExtModPyGen"
struct ContextBroker_connect_external_Python__Client_GenericPluginRole{
	char RoleName[sizeof(ContextBroker_connect_external_Python__Client_GenericPluginRole__Name)];
	int idx;
};








#ifdef  __cplusplus
}
#endif
#endif /* HEADERS__DENKREMENT_PLUGIN_PREDEFINED__CONNECT_EXTERNAL_PYTHON__H */
