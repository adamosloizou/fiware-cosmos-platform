/* ****************************************************************************
*
* FILE                     QsiBase.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 15 2011
*
*/
#include <stdlib.h>             // free

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "QsiBase.h"            // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* Base - 
*/
Base::Base(Box* _owner, Type _type, const char* _name, int _x, int _y, int _width, int _height)
{
	owner    = _owner;
	type     = _type;
	name     = strdup(_name);
	xInitial = _x;
	yInitial = _y;
	x        = 0;
	y        = 0;
	width    = _width;
	height   = _height;

	isBox    = false;
}



/* ****************************************************************************
*
* ~Base - 
*/
Base::~Base()
{
	if (name)
	{
		LM_T(LmtRemove, ("destroying Base '%s'", name));
		free(name);
	}
	else
		LM_T(LmtRemove, ("destroying a nameless Base object"));
}



/* ****************************************************************************
*
* getOwner - 
*/
Box* Base::getOwner(void)
{
	return owner;
}



/* ****************************************************************************
*
* typeName - 
*/
const char* Base::typeName(void)
{
	switch (type)
	{
	case SimpleText:       return "SimpleText";
	case Line:             return "Line";
	case Image:            return "Image";
	case Label:            return "Label";
	case Button:           return "Button";
	case Input:            return "Input";
	case Rectangle:        return "Rectangle";
	case BoxItem:          return "Box";
	case ExpandListItem:   return "ExpandList";
	}

	return "Unknown Type";
}



/* ****************************************************************************
*
* xGet - 
*/
int Base::xGet(void)
{
	return x;
}



/* ****************************************************************************
*
* xSet - 
*/
void Base::xSet(int _x)
{
	x = _x;
}



/* ****************************************************************************
*
* yGet - 
*/
int Base::yGet(void)
{
	return y;
}



/* ****************************************************************************
*
* ySet - 
*/
void Base::ySet(int _y)
{
	y = _y;
}



/* ****************************************************************************
*
* typeSet - 
*/
void Base::typeSet(Type _type)
{
	type = _type;
}



/* ****************************************************************************
*
* isAncestor - 
*/
bool Base::isAncestor(Box* qbP)
{
	if (owner == NULL)
		return false;
	if (qbP == owner)
		return true;

	return isAncestor(owner);
}

}
