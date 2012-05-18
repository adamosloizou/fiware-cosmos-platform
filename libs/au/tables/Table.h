
#ifndef _H_AU_TABLE_TABLE
#define _H_AU_TABLE_TABLE

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/containers/StringVector.h"
#include "au/CommandLine.h"
#include "au/containers/simple_map.h"
#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/containers/map.h"                 // au::map
#include "au/containers/vector.h"
#include "au/string.h"              // au::str(...)
#include "au/console/ConsoleAutoComplete.h"

namespace au 
{
    namespace tables
    {
        
        class Table;
        class SelectTableInformation;
        class SelectCondition;
        class TableRow;
        
        // ------------------------------------------------------------
        // TableSelectCondition
        // ------------------------------------------------------------
        
        class TableSelectCondition
        {
            friend class Table;
            friend class TableRow;
            
        public:
            
            std::string name;
            std::string value;
            
            TableSelectCondition( std::string _name , std::string _value )
            {
                name = _name;
                value = _value;
            }
            
            bool check( TableRow *row );
            
        };
        
        // ------------------------------------------------------------
        // SelectTableInformation
        // ------------------------------------------------------------
        
        class SelectTableInformation
        {
            friend class Table;
            
        public:
            
            std::string title;                                // Title of the column ( when printing )
            au::StringVector columns;                         // Columns to select or print
            std::vector<TableSelectCondition> conditions;     // Conditions to meet by each row
            
            StringVector group_columns;                       // Group records by this ( if any )
            StringVector divide_columns;                      // If not grouped, it can be divided
            StringVector sort_columns;                        // If not grouped nor divided, it is sorted by these
            
            size_t limit;                                     // Max number of records ( in the final print )
            
            SelectTableInformation()
            {
                limit = 0;        
            }
            
            void addColumn( std::string definition );
            void addColumns( au::StringVector _columns );
            
            bool checkChangeDivision( TableRow* row , TableRow* row2);
            
            // Check all conditions over this row
            bool check( TableRow *row );

            // Add condition
            void add_conditions( std::string conditions );
            
        };
        
        // ------------------------------------------------------------
        // TableCell
        // ------------------------------------------------------------
       
        class TableCell
        {
            // Internal values
            StringVector values;
        
            
            friend class Table;
        public:

            TableCell(  )
            {
                
            }

            TableCell( std::string value )
            {
                values.push_back(value);
            }

            TableCell( StringVector _values )
            {
                values.copyFrom(_values);
            }
            
            // Duplicate a cell
            TableCell( TableCell* cell )
            {
                values.copyFrom( cell->values );
            }
            
            void addFrom( TableCell* cell )
            {
                values.copyFrom( cell->values );
            }
            
            void set( std::string value )
            {
                // Set a single values
                values.clear();
                values.push_back(value);
            }
            
            int compare( TableCell* cell )
            {
                if( values.size() !=  cell->values.size() )
                    return values.size() - cell->values.size();
                
                for( size_t i = 0 ; i < values.size() ; i++ )
                    if( values[i] != cell->values[i] )
                    {
                        if( values[i] < cell->values[i] )
                            return -1;
                        else
                            return 1;
                    }
                
                return 0;
            }
            
            // to be printed as it is...
            std::string str()
            {
                if( values.size() == 1 )
                    return values[0];
                else
                    return values.str_vector();
            }
            
        };

        // ------------------------------------------------------------
        // TableRow
        // ------------------------------------------------------------
        
        class TableRow
        {
            au::map<std::string,TableCell> cells;
            
            friend class DataBase;
            
        public:
            
            typedef enum 
            {
                normal,
                separator
            } Type;
            
        protected:
            
            Type type;
            
        public:
            
            TableRow( Type _type );
            TableRow( StringVector _columns , StringVector _values );
            TableRow( TableRow* table_row );
            
            ~TableRow()
            {
                cells.clearMap();
            }

            // Single value set
            void set( std::string name , std::string value );
            void set( std::string name , TableCell* cell );

            // Get a particular cell
            TableCell* get( std::string name );

            // Get single value
            std::string getValue( std::string name );
            
            // Function to compare two rows
            int compare( TableRow* row , StringVector &sort_columns );
            
            // Get type ( normal, separator, etc.. )
            Type getType();
            
            // get a table with all information
            Table* getTable();
            
            // Debug string
            std::string str();
            
        };
        
        // ------------------------------------------------------------
        // TableColumn
        // ------------------------------------------------------------

        class TableColumn
        {
            std::string name;      // Name of the column to display
            std::string title;     // Title to display
            
            
            std::string field_definition;
            
            
            friend class Table;
            
            typedef enum
            {
                sum,
                max,
                min,
                min_max,
                vector,
                different
            } TableColumnGroup;
            
            typedef enum
            {
                format_string,
                format_uint64,
                format_uint,
                format_time,
                format_timestamp,
                format_double,
                format_percentadge,
            } TableColumnFormat;

            // Format properties
            bool left;
            TableColumnGroup group;
            TableColumnFormat format;

        public:
            
            TableColumn( std::string field_definition );
            
            std::string getName();
            std::string getTitle();

            bool getLeft();
            
            std::string transform( StringVector& values );
            std::string transform_string( StringVector& values );
            std::string transform_time( StringVector& values );
            std::string transform_uint64( StringVector& values );
            std::string transform_double( StringVector& values );
            std::string transform_percentadge( StringVector& values );
            
        private:

            // Read a format definition
            void processModifier( std::string modifier );

            // Simple transformacion of the vaue
            std::string simple_transform( std::string value );

            // Sort some values
            void sort( StringVector& values );
            std::string str_sum( StringVector& values );
            
        };
        
        
        // ------------------------------------------------------------
        // Table
        // ------------------------------------------------------------
        
        class Table
        {
            std::string title;
            
            au::vector<TableRow > rows;      
            au::vector<TableColumn > columns;
            
            friend class DataBase;
            
        public:
            
            Table( std::string description );
            Table( StringVector _columns );
            Table( Table* table );
            
            ~Table();

            // Modify table
            void setTitle( std::string _default_title);
            void addRow( StringVector string_vector );
            void addSeparator();
            
            // Get information
            std::string getTitle( );
            size_t getNumRows();
            size_t getNumColumns();
            std::string getValue( size_t r , size_t c );
            std::string getValue( int r , std::string column_name );
            size_t getColumn( std::string title );
            std::string getColumn( size_t pos );
            std::string getFormatForColumn( size_t c);
            
            StringVector getValuesFromColumn( std::string name );
            StringVector getValuesFromColumn( std::string name , TableSelectCondition* condition );
            StringVector getColumnNames();
            
            // Get a string to print table on screen
            std::string str( );
            std::string str_xml( );
            std::string str_json( );
            std::string str_html( );
            std::string strFormatted( std::string format );

            // Select
            Table* selectTable( SelectTableInformation *select_table_information );
            std::string str( SelectTableInformation *select_table_information );
            std::string strSortedGroupedAndfiltered( std::string title 
                                                    , std::string group_field=""  
                                                    , std::string sort_field="" 
                                                    ,  std::string conditions="" 
                                                    , size_t limit=0 );            
            // Description table
            Table* getColumnDescriptionTable();
            
            // Generic sort function
            void sort( StringVector &sort_columns );
            
            // Reverse order of rows
            void reverseRows();
            
        private:
            
            std::string top_line( std::vector<size_t> &length );
            std::string top_line2( std::vector<size_t> &length );
            std::string bottom_line( std::vector<size_t> &length );
            std::string line( std::vector<size_t> &length   );
            std::string string_length( std::string value , int width , bool left );
            size_t getMaxWidthForColumn( size_t c );    
            
            TableColumn* getSelectColumn( std::string description );
            void addColumn( TableColumn * column ); // Only used in select
            
            
        };
        
        
        
    }
}

#endif
