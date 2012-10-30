

/* ****************************************************************************
*
* FILE            Pugi.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         pugi library
*
* DATE            7/15/11
*
* DESCRIPTION
*
*  Usefull functions to simplify development with pugi library
*  Everything ready for running xpath queries over xml data
*
* ****************************************************************************/

#ifndef _H_xml_Pugi
#define _H_xml_Pugi


#include <set>
#include <sstream>          // std::ostringstream
#include <string>           // std::string
#include <vector>

#include "au/string/StringUtilities.h"
#include "au/tables/pugixml.hpp"     // pugi::...

namespace au {
namespace tables {
class TreeItem;
}
}


namespace pugi {
// Working with TreeItems
au::tables::TreeItem *treeItemFromDocument(const xml_document& xml_doc);
au::tables::TreeItem *treeItemFromNode(const xml_node& xml_node);
}
#endif  // ifndef _H_xml_Pugi
