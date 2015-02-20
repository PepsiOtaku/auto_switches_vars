// Auto Switches
// by PepsiOtaku
// Version 1.1

#include <DynRPG/DynRPG.h>
#define NOT_MAIN_MODULE
#include <sstream> // For std::stringstream

std::map<std::string, std::string> configuration;

std::string folderPath; // Switches
std::string filePrefix; // loot
std::string fileExt; // var
std::string filename; // .\\Switches\\loot##.var
std::stringstream buffStr; // .\\Switches\\loot00.var
std::string buffername;


unsigned short int confMasterSwitch; // 4041
unsigned short int confMasterVar; // 4022
unsigned short int confDeleteFileVar; // 4002
bool confHasFolder = false;

bool fail = false;

// this simple function gets the filename based on the folder path, prefix, save/load ID & extension
std::string getFilename (std::string fp, std::string fpr, int id, std::string fex, bool fold) {
    std::stringstream f;
    if (id < 10) {
        // for save IDs with less than 10 digits
        if (fold == true) f << ".\\" << fp << "\\" << fpr << "0" << id << "." << fex;
        else f << fpr << "0" << id << "." << fex;
    } else {
        // for save IDs with more than 10 digits
        if (fold == true) f << ".\\" << fp << "\\" << fpr << id << "." << fex;
        else f << fpr << id << "." << fex;
    }
    return f.str();
}

bool onStartup (char *pluginName) {
    configuration = RPG::loadConfiguration(pluginName);
    // get folder, file prefix, and file extension, and create the buffer string
    confHasFolder = configuration["HasFolder"] == "true";
    folderPath = configuration["Folder"];
    filePrefix = configuration["FilePrefix"];
    fileExt = configuration["FileExt"];
    if (confHasFolder == true) buffStr << ".\\" << folderPath << "\\" << filePrefix << "00." << fileExt;
    else buffStr << filePrefix << "00." << fileExt;
    buffername = buffStr.str(); //convert stringstream to string
    // get the master switch (required) & delete file var (optional -- see notes in delete_aswitch_file command)
    confMasterSwitch = atoi(configuration["MasterSwitch"].c_str());
    confMasterVar = atoi(configuration["MasterVar"].c_str());
    confDeleteFileVar = atoi(configuration["DeleteFileVar"].c_str());
    // create the folder if it doesn't exist
    if (CreateDirectory(folderPath.c_str(), NULL)
        || ERROR_ALREADY_EXISTS == GetLastError()){
        // Directory is created
    } else {} //Failed to create directory
    return true;
}

void onInitTitleScreen() {
    // This checks for any non-existent save files on startup, and deletes the auto-switch data if the save doesn't
    // exist anymore
    std::stringstream savename;
    std::stringstream filename;
    std::string savePrefix; // loot
    std::string saveExt; // var
    savePrefix = "save";
    saveExt = "lsd";
    WIN32_FIND_DATA lpFindFileData;
    HANDLE hFind1;
    HANDLE hFind2;
    // Find saves that begin with "save," and end with ".lsd" with any numeric value inbetween
    for (int i = 0; i<=99;i++) {
        filename.str(std::string()); // clear any previous stringstream data
        savename.str(std::string()); // clear any previous stringstream data
        savename << getFilename (folderPath, savePrefix, i, saveExt, false);
        filename << getFilename (folderPath, filePrefix, i, fileExt, confHasFolder);
        hFind1 = FindFirstFile(savename.str().c_str(), &lpFindFileData);
        if(hFind1 == INVALID_HANDLE_VALUE) {
            // did not find
            hFind2 = FindFirstFile(filename.str().c_str(), &lpFindFileData);
            if(hFind2 != INVALID_HANDLE_VALUE) {
                DeleteFile(filename.str().c_str());
                FindClose(hFind2);
            }
            FindClose(hFind1);
        }
    }
}


bool onComment ( const char* text,
                 const RPG::ParsedCommentData* parsedData,
                 RPG::EventScriptLine* nextScriptLine,
                 RPG::EventScriptData* scriptData,
                 int eventId,
                 int pageId,
                 int lineId,
                 int* nextLineId )
{
    std::string cmd = parsedData->command;
    std::stringstream evnStr;
    std::stringstream filename;

    //switch to import on/off value to
    int returnSwitchVal;
    //get the map id the hero is on to be used as the ini heade
    char mapId[10];
    itoa(RPG::hero->mapId, mapId, 10);
    //creates a pointer value to the event & page id in the ini file.
    evnStr << eventId << "." << pageId;
    if (confHasFolder == true) filename << ".\\" << folderPath << "\\" << filePrefix << "00." << fileExt;
    else filename << filePrefix << "00." << fileExt;
    // Example: event 99 page 3 on map 302 would be under:
    //[302]
    //99.3=#

    if(!cmd.compare("export_event_state"))
    {
        // actually export the map ID, event & page #, with a value of "1" meaning the auto-switch was turned on.
        if (parsedData->parametersCount == 1 && parsedData->parameters[0].type == RPG::PARAM_NUMBER
                && parsedData->parameters[0].number == 0){
            WritePrivateProfileString( mapId, evnStr.str().c_str(), "0", filename.str().c_str());
        } else {
            WritePrivateProfileString( mapId, evnStr.str().c_str(), "1", filename.str().c_str());
        }
    }

    if(!cmd.compare("import_event_state"))
    {
        // checks the ini file for the map ID, event & page # (which would have a value of "1")
        // if it doesn't exist in the ini, returnSwitchVal gets a value of 0
        // if it does exist, returnSwitchVal gets a value of 1
        returnSwitchVal = GetPrivateProfileInt( mapId, evnStr.str().c_str(), 0, filename.str().c_str());
        //  sends this value back to the master switch. 0=off, 1=on
        RPG::switches[confMasterSwitch] = returnSwitchVal;
    }

    // The next two commands save & load variables by name (Similar to DynVarStorage but quicker)
    if(!cmd.compare("save_var"))
    {
        // ensures there are 2 parameters: the first being the string to store the var, and the second being the var
        // can also use "V####" when you set your comments to pass a variable to the ini
        if (parsedData->parametersCount == 2 && parsedData->parameters[0].type == RPG::PARAM_STRING
            && parsedData->parameters[1].type == RPG::PARAM_NUMBER) {
            char varValue[10];
            itoa(parsedData->parameters[1].number,varValue,10);
            WritePrivateProfileString("vars", parsedData->parameters[0].text, varValue, filename.str().c_str());
        }
    }

    if(!cmd.compare("load_var"))
    {
        // Checks to see if the first parameter exists & stores the var into a temporary variable
        if (parsedData->parametersCount >= 1 && parsedData->parameters[0].type == RPG::PARAM_STRING) {
            returnSwitchVal = GetPrivateProfileInt( "vars", parsedData->parameters[0].text, 0, filename.str().c_str());
            // if a 2nd parameter exists, store the temp variable into the defined variable ID
            if (parsedData->parametersCount == 2 && parsedData->parameters[1].type == RPG::PARAM_NUMBER) {
                RPG::variables[parsedData->parameters[1].number] = returnSwitchVal;
            } else {
            // otherwise, store it in the variable defined as the "MasterVar" in DynRPG.ini
                RPG::variables[confMasterVar] = returnSwitchVal;
            }
        }
    }

    if(!cmd.compare("delete_aswitch_file"))
    {
        // this command was built for my CMS tools plugin series, so that when a file is deleted,
        // the auto_switches file for that save ID will also be deleted
        filename.str(std::string()); // clear any previous stringstream data
        // get the filename based on the save ID defined in the confDeleteFileVar variable
        filename << getFilename (folderPath, filePrefix, RPG::variables[confDeleteFileVar], fileExt, confHasFolder);
        // actually delete the buffer file
        DeleteFile(filename.str().c_str());
    }
    return true;
}

void onSaveGame( int id, void __cdecl(*savePluginData) (char* data, int length)) {
    // get the filename based on the save ID chosen
    filename = getFilename (folderPath, filePrefix, id, fileExt, confHasFolder);
    // FindFirstFile function load
    WIN32_FIND_DATA lpFindFileData;
    HANDLE hFind;
    // Find the file defined in char "buffername"
    hFind = FindFirstFile(buffername.c_str(), &lpFindFileData);
    // if the file already exists, delete it
    if(hFind == INVALID_HANDLE_VALUE) {
        DeleteFile(filename.c_str());
    }
    // copy the buffer file to the file associated to the save ID
    CopyFile(buffername.c_str(), filename.c_str(), fail);
    FindClose(hFind);
}

void onLoadGame (int id, char* data, int length) {
    // get the filename based on the save ID chosen
    filename = getFilename (folderPath, filePrefix, id, fileExt, confHasFolder);
    // if the buffer file exists, delete it, as the old one isn't needed anymore
    DeleteFile(buffername.c_str());
    // copy the file associated to the save ID as the new buffer file
    CopyFile(filename.c_str(), buffername.c_str(), fail);
}

void onExit () {
    // delete the buffer file if it exists
    DeleteFile(buffername.c_str());
}

void onNewGame () {
    // delete the buffer file if it exists
    DeleteFile(buffername.c_str());
}


