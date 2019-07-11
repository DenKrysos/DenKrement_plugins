

This Plugin provides the opportunity to Enable/Disable Debug-Terminal-Print-Mode by Variation of its File-Name:

- "Plain" File-Name, i.e. "DenKrement_plugin_generic__PolicyManager.so"
	-> Normal-Operation. Means: No Debug Output

- Append "_DEBUG" to the File-Name, i.e. "DenKrement_plugin_generic__PolicyManager_DEBUG.so"
	-> During Operation this Plugin throws Debug Prints at stdout on you like hell.







Currently "Stability" of the Plugin:
- It adjusts itself to the Total Number of existing Physical Links and also the ARs coming from the Dashbaord
	-> But only during Initialization Phase. If these "Total Numbers" change during runtime, it does not adjust itself to the new number.