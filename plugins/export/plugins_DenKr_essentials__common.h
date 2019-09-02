/*
 * DenKrement_plugins.h
 *
 *  Created on: 26.07.2017
 *      Author: Dennis Krummacker
 */

#ifndef PLUGINS__EXPORT__PLUGINS_DENKR_ESSENTIALS__COMMON_H
#define PLUGINS__EXPORT__PLUGINS_DENKR_ESSENTIALS__COMMON_H



//INSTRUCTION
//   Always remember to define the Macros & Structs as explained inside "plugins_DenKr_essentials__common.h"



//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//==================================================================================================//
//--------------------------------------------------------------------------------------------------//
//----  This section contains essential stuff for the !!! DenKr_essentials !!! Library  ------------//
//----  It's Values are needed by the DL_Libs Part  ------------------------------------------------//
//----  So you better use it as a Template  --------------------------------------------------------//
//--------------------------------------------------------------------------------------------------//
//==================================================================================================//

//Define your own predefined Roles here (i.e. replace the ones here)
//  These automatically become prefixed with "DenKr_plugin_role__"
//  -> This means: If you want to use one role (say to fill in at *int role=<here>* in your Plugins-Init-Function)
//     You need to write e.g. *int role=DenKr_plugin_role__<WhatStaysHereInsideDenKr_plugin_roles_ENTRIES>*
#define DenKr_plugin_roles_ENTRIES \
	Context_Broker__External_Connector_Python,\
	Some_Dummy

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//INFO:
// Inside "DenKr_essentials/Plugins/plugins_codeGeneration.h" are some fine Macros defined, which automatically generate code, based on the defined Roles here.
// These macros are used on several occurrences and work tightly intertwined with the stuff from "src/plugins/export/plugins_export.h".
// Amongst others, it creates an enumeration   »typedef enum DenKr_plugin_roles_t{ [...] }DenKr_plugin_roles;«
//////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------//
//==================================================================================================//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////










#endif /* PLUGINS__EXPORT__PLUGINS_DENKR_ESSENTIALS__COMMON_H */
