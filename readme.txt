========================================================================
Auto Switches w/ Custom Var Import/Export v1.1
For RPG Maker 2003 with DynRPG v0.20 or higher
By PepsiOtaku
========================================================================

This plugin allows for auto-switches by event # & page #, meaning you can add things like chests & 
searchable pots without using precious switches. Additionally, if you wanted to give something 
different search states (searched more than once) you can just add more pages.

It also includes commands to save/load additional variables by name.

These are stored in separate files that you define (that are essentially ini's), while the plugin does 
all the legwork. For instance, you could create a folder called "Switches" (within your game folder) 
and use a file prefix of "loot" followed by the save ID, and a file extension of "asw" (auto-switch)

This plugin also takes into account switching between saves & removing save files (through windows).
It uses a buffer file (Prefix+"00".ext) for these actions.


Additionally, if you're using Cherry's SaveLoad patch for a custom menu, along with my Save Detector 
& Delete Save plugin (http://rpgmaker.net/engines/rm2k3/utilities/31/), there's a command to delete
a file # based on a variable (see below).


Installation
-------------

To install the plugin, make sure that you have patched your project with cherry's DynRPG patch which
can be found here: http://cherrytree.at/dynrpg

1. Copy the "auto_swithes_vars.dll" into the DynPlugins folder of your own project.

2. Add the following (between the asterisks) to your DynRPG.ini 

**********************************************

[auto_switches_vars]
HasFolder=true
Folder=Switches
FilePrefix=Loot
FileExt=asw
MasterSwitch=4041
MasterVar=4022
DeleteFileVar=4002

**********************************************

3. Change "HasFolder" to false if you want to store the switch/variable files in the main game directory

4. If HasFolder=true, "Folder" will define the folder you want to store the switch/variable files.
   DO NOT include any special characters. Additional Subfolders are untested, but would probably work
   with "\\" inbetween each defined folder (example: Folder=DynPlugins\\Switches)

5. "FilePrefix" will be the prefix of the filename before the save ID.

6. "FileExt" will be the file extension of the filename. DO NOT include the "."

7. "MasterSwitch" will be the Switch that the file value will be passed to

8. If you're using the save/load variable functionality, "MasterVar" will be the Variable that the file value
   will be passed to

9. "DeleteFileVar" is only needed for the scenario I mentioned in the description above where you have both
   the SaveLoadPatch & my Delete Save plugin. This will delete the Switch/Var file based on the variable set
   here. (Ex: Setting it to "5" and calling @delete_aswitch_file will delete Loot05.asw)


Instructions
-------------

This plugin takes five commands:
  @export_event_state: Exports map ID, event & page # with a value of 1
  @import_event_state: Checks the event the command is called from
  @save_var: Saves a variable by name
  @load_var: Loads a stored variable
  @delete_aswitch_file


@export_event_state (1 parameter)
  Exports the map ID, event & page #, with a value of "1" meaning the auto-switch was turned on.
  Ex: In your Switch/Var file, event 99 page 3 on map 302 would be under:
    [302]
    99.3=1
	
  Parameter 1: If parameter 1 is "0" then, the switch will be set to OFF (resetting that event)
	Note: New to version 1.1

@import_event_state (no parameters)
  Checks the event the command is called from in the ini file for the map ID, event & page # 
    (which would have a value of "1"). 
  If it doesn't exist in the ini, the master switch gets a value of 0
  If it does exist, the master switch gets a value of 1

@save_var takes (2 parameters)
  Parameter 1: the name of the variable. Can be anything I think, but stick to letters & underscores 
    to be on the safe side
  Parameter 2: value of the variable. Can also use "V####" when you set your comments to pass a standard
    variable to the ini
  Ex: In your Switch/Var file, var "test" would be under:
    [vars]
    test=1234

@load_var takes (1 or 2 parameters)
  Parameter 1: the name of the variable. Same as @save_var
  Parameter 2 (optional): Variable to pass the value to. Defaults to "MasterVar" if this parameter is not
    included.

@delete_aswitch_file 
  Deletes a switch/variable file based on the scenario mentioned above


@save_var & @load_var can be used anywhere. Just be mindful to what RM2k3 variables you reference. Examples:
<>Comment: @save_var "test", 1234
<>Comment: @save_var "test2", V12 --- save the value of variable 12

<>Comment: @load_var "test" --- loads the value of variable "test" to the default MasterVar
<>COmment: @load_var "test2", 4011 --- loads the value of variable "test2" to RM2k3 variable 4011
<>COmment: @load_var "test2", V12 --- loads the value of variable "test2" to the RM2k3 variable stored in RM2k3 variable 12


To use @import_event_state and @export_event state, create an event on a map of your choice & add the following:

(This is just a simple example)
<>Comment: @import_event_state
<>Branch if Switch [XXXX:DynRPG-AutoSwMaster] is OFF
  <>Call Event: RandomLoot --- Common event that gives random loot
  <>Comment: @export_event_state
  <>
: Else Handler
  <>Call Event: SeenAlready --- Common event for when a chest has already been seen.
  <>
: End


You can potentially add this same code on multiple pages, and the plugin will make/get a different state for every page.

See the included test project for in-game examples!

