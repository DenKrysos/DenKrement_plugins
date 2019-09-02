/*
 * DenKrement_plugins.h
 *
 *  Created on: 26.07.2017
 *      Author: Dennis Krummacker
 */

#ifndef PLUGINS__EXPORT__PLUGINS_EXPORT_H
#define PLUGINS__EXPORT__PLUGINS_EXPORT_H

#include <global/global_settings.h>


//INFO:
//   The predefined Plugins are spawned as "PTHREAD_CREATE_JOINABLE" (joinable Threads)
//      Thus, the main() may want to join or detach them.

//Naming convention:
// Name the *additional* to pass Arguments:
//   <PluginName>__addArgs
//   & <PluginName>__addArgs_struct
//   INFO: <PluginName> refers to the "reduced" Name of the Plugin, i.e. what is typed into 'DenKr_plugin_roles_ENTRIES' of "plugins_DenKr_essentials__common.h"
//   -> Example "Context_Broker__External_Connector_Python__addArgs" & "Context_Broker__External_Connector_Python__addArgs_struct"  (i.e. NOT "DenKr_plugin_role__Context_Broker__External_Connector_Python__addArgs")
//INSTRUCTION:
// As of now, the stuff below has to be defined for every plugin. If no additional Plugins beside the default ones needs to be passed, just define it empty.
// Make sure, to only try to pass Values, which are available at this point (i.e. in case of DenKrement, everything available in the main, before Plugin Loading)
//FILLING in the content of the additional arguments to pass
// - In the "Macro Context_Broker__External_Connector_Python__addArgs, you only write, what you would write to fill into a struct
//    - Assume the 'TARGET' is just a struct of the type you defined a few lines above





//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//==================================================================================================//
//--------------------------------------------------------------------------------------------------//
//----  Further Descriptions and Ressources for predefined Roles  ----------------------------------//
//----  The Resources has to be #included by the plugins  ------------------------------------------//
//--------------------------------------------------------------------------------------------------//
//==================================================================================================//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - -   DenKrement_plugin_predefined__connect_external_Python   - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Working-Type: Thread (DenKr_plugin_working_type__thread)
// Started with *additional* Arguments:
//  (Actually not required for this Plugin, but done here to demonstrate how it would be done)
struct addArgs__Context_Broker__External_Connector_Python{};
#define Context_Broker__External_Connector_Python__addArgs_struct struct addArgs__Context_Broker__External_Connector_Python

#define Context_Broker__External_Connector_Python__addArgs(TARGET)

//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------//
//==================================================================================================//
//--------------------------------------------------------------------------------------------------//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - -   DenKr_plugin_role__Some_Dummy   - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Working-Type: Thread (DenKr_plugin_working_type__thread)
//
// The Thread is started with the default Arguments only.
struct addArgs__Some_Dummy{
		int test_dummy;
};
#define Some_Dummy__addArgs_struct struct addArgs__Some_Dummy

#define Some_Dummy__addArgs(TARGET)\
	TARGET.test_dummy=testaddArg;

//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------//
//==================================================================================================//
//--------------------------------------------------------------------------------------------------//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - -   DenKr_plugin_role__EXAMPLE (empty Args)   - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Working-Type: Thread (DenKr_plugin_working_type__thread)
//
// The Thread is started with the default Arguments only.
struct addArgs__ExampleEmpty{};
#define ExampleEmpty__addArgs_struct struct addArgs__ExampleEmpty

#define ExampleEmpty__addArgs(TARGET_P)


//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------//
//==================================================================================================//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////









#endif /* PLUGINS__EXPORT__PLUGINS_EXPORT_H */
