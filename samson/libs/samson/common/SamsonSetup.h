#ifndef _H_SAMSON_SETUP
#define _H_SAMSON_SETUP

#include <iostream>            // std::cout
#include <map>                 // std::map
#include <stdlib.h>            // atoll
#include <string>              // std::string
#include <sys/stat.h>          // mkdir

#include "logMsg/logMsg.h"

#include "au/Environment.h"  // au::Environment
#include "au/Singleton.h"
#include "au/containers/StringVector.h"
#include "au/containers/map.h"  // au::map

#include "samson/common/status.h"


namespace au {
class ErrorManager;
}

namespace samson {
Status createDirectory(std::string path);
Status createFullDirectory(std::string path);
std::string cannonical_path(std::string path);

typedef enum {
  SetupItem_uint64,
  SetupItem_string
} SamsonAdaptorType;

class SetupItem {
  std::string name;
  std::string default_value;

  std::string value;

  std::string description;
  SamsonAdaptorType type;

public:

  SetupItem(std::string _name, std::string _default_value, std::string _description, SamsonAdaptorType _type);

  bool setValue(std::string _value);
  bool check_valid(std::string _value);
  std::string getValue();
  std::string getDefaultValue();
  std::string getSetValue();
  std::string getDescription();
  std::string getConcept();
  void resetToDefaultValue();
  void clearCustumValue();
};


class SetupItemCollection {
protected:

  au::map< std::string, SetupItem > items;

  // Add a new parameter to consider
  void  add(std::string _name, std::string _default_value, std::string _description, SamsonAdaptorType type);

public:

  ~SetupItemCollection();

  // Load a particular file to include all setup parameters
  void load(std::string file);

  // get the value of a parameter
  std::string getValueForParameter(std::string name);

  // Set manually a particular parameter ( special case )
  bool setValueForParameter(std::string name, std::string value);

  // Check if a particular property if defined
  bool isParameterDefined(std::string name);

  // Come back to default parameters
  void resetToDefaultValues();

  std::string str();
};


class SamsonSetup : public SetupItemCollection {
  friend class au::Singleton<SamsonSetup>;
  SamsonSetup();

  std::string _samson_home;                 // Home directory for SAMSON system
  std::string _samson_working;              // Working directory for SAMSON system

public:

  void SetWorkerDirectories(std::string samson_home, std::string samson_working);

  // Used only in unitTests, to have them more complete
  void addItem(std::string _name, std::string _default_value, std::string _description, SamsonAdaptorType type);

  // Get access to parameters
  std::string get(std::string name);
  size_t getUInt64(std::string name);
  int getInt(std::string name);

  std::string get_default(std::string name);

  // Get names fo files
  std::string setupFileName();                               // Get the Steup file
  std::string sharedMemoryLogFileName();
  std::string modulesDirectory();
  std::string blocksDirectory();
  std::string blockFileName(size_t block_id);
  size_t blockIdFromFileName(std::string fileName);
  std::string streamManagerLogFileName();
  std::string clusterInformationFileName();
  std::vector<std::string> getItemNames();


  // Clear values specified manually
  void clearCustumValues();

  int save();
};
}

#endif  // ifndef _H_SAMSON_SETUP
