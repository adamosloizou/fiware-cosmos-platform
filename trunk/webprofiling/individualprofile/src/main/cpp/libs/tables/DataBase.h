#ifndef _H_AU_TABLE_DATABASE
#define _H_AU_TABLE_DATABASE

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/CommandLine.h"
#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/map.h"                 // au::map
#include "au/Token.h"
#include "au/string.h"              // au::str(...)
#include "au/ConsoleAutoComplete.h"

#include "au/namespace.h"


#include "tables/Table.h"
#include "tables/Tree.h"
#include "tables/Collection.h"

NAMESPACE_BEGIN(au)
NAMESPACE_BEGIN(tables)

// Ecosystem of tables to work with them

class DataBase
{
    au::map<std::string, Table> tables;
    au::map<std::string, TreeItem> trees;
    au::map<std::string, Collection> collections;
    
    au::Token token; // Mutex protection
    
public:
    
    DataBase();
    
    // Add a table, tree or collection to this database
    void addTable( std::string name , Table* table );
    void addTree( std::string name , TreeItem* tree );
    void addCollection( std::string name , Collection* collection );
    
    // Main execution function
    std::string runCommand( std::string command );

    // Main autocomplete function
    void autoComplete( au::ConsoleAutoComplete* info );

    // Get duplicate of tables, trees or collections
    Table* getTable( std::string name );
    TreeItem* getTree( std::string name );
    Collection* getCollection( std::string name );
    
    // Get select table
    Table* getTable( std::string name , SelectTableInformation* select );
    
    // Get all values of a particular column
    StringVector getValuesFromColumn( std::string table_name , std::string column_name );
    StringVector getValuesFromColumn( std::string table_name , std::string column_name , SelectCondition* condition );
    
    // Replace node in a tree
    void replaceNodeInTree( std::string tree_name , TreeItem* node );
    
private:
    
    // Non mutex protected methods
    void _addTable( std::string name , Table* table );
    void _addTree( std::string name , TreeItem* tree );
    void _addCollection( std::string name , Collection* collection );
    
    // Specific Autocomplete
    void autoCompleteTables( au::ConsoleAutoComplete* info );
    void autoCompleteTrees( au::ConsoleAutoComplete* info );
    void autoCompleteCollections( au::ConsoleAutoComplete* info );
    void autoCompleteFieldsOfTable( std::string table_name , au::ConsoleAutoComplete* info  );
    
    
    // Get spetial tables
    Table* getTableOfTables();
    Table* getTableOfTrees();
    Table* getTableOfCollections();
    Table* getErrorTable( int error_code , std::string error_message );

    
    
};

NAMESPACE_END
NAMESPACE_END

#endif