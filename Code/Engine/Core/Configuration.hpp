#pragma once
#include <vector>

// Startup Config System - Setup Initial Configs by parsing supplied file.
bool ConfigSystemStartup(char const *config_file);

std::string ConvertBufferToString(std::vector<unsigned char>& outBuffer);

void LoadConfigurationToStrings(const std::string& s);

// Setters - feel free to use std::string
void ConfigSet(const char* name, const char* value);
void ConfigSet(const char* name, const int value);
void ConfigSet(const char* name, const float value);
void ConfigSet(const char* name, const bool value);

// Easy check for simple on/off flags. 
bool IsConfigSet(const char* id);
void ConfigUnset(const char* id);

// Fetching.  If a config exists and is 
// convertible to the desried type, 
// place its converted value in the out variable,
// and return true;
// Otherwise, return false.
bool ConfigGetString(const char** outv, const char* id);
bool ConfigGetString(std::string* outv, const char* id);
bool ConfigGetInt(int* outv, const char* id);
bool ConfigGetFloat(float* outv, const char* id);
bool ConfigGetBool(bool* outv, const char* id);

// [DEBUG FEATURE] List all currently existing configs.
void ConfigList();

// Load a config file
// see sample.config 
bool ConfigLoadFile(const char* filename);

//------------------------------------------------------------------------
// OPTIONALS
//------------------------------------------------------------------------

// Config Files support comments (Recommend '#' at Comment Symbol
//       which will cause parser to ignore the rest of the line. 

// Boolean Shorthand.  Example  
//   +fullscreen   # Equivalent to fullscreen=true;
//   -sound        # Can either be sound=false, or force it to be unset

// Support String Keys/Values.  Example
//   "player=name" = "Zidane Locke"   # results in ConfigGetString(&name, "player=name") 
//                                    # to fetch "Zidane Locke"

// Can have multiple configs per line (goes well with the previous task)
//   window_res_x=1280 window_res_y=720 fullscreen=true

// Arguments passed on command line are parsed (and override) configs 
//   from init file [Requires previous task].  ConfigSystemStartup becomes;
//void ConfigSystemStartup(char const *config_file, char const *cmd_line);