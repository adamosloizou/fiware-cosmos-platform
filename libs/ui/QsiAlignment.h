#ifndef QSI_ALIGNMENT_H
#define QSI_ALIGNMENT_H

/* ****************************************************************************
*
* FILE                     QsiAlignment.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 14 2011
*
*/



namespace Qsi
{



class QsiBase;



/* ****************************************************************************
*
* Alignment - 
*/
typedef struct Alignment
{
	typedef enum Type
	{
		Unaligned,
		North,
		South,
		East,
		West,
		Center
#if 0
		,
		NorthEast,
		NorthWest,
		SouthEast,
		SouthWest
#endif
	} Type;

	Type       type;
	QsiBase*   master;
	QsiBase*   slave;
	int        margin;

	static const char* name(Type type)
	{
		switch (type)
		{
		case North:      return "North";
		case South:      return "South";
		case East:       return "East";
		case West:       return "West";
		case Center:     return "Center";
		case Unaligned:  return "Unaligned";
		}

		return "Undefined alignment";
	}

	static Type invert(Type type)
	{
		switch (type)
		{
		case North:      return South;
		case South:      return North;
		case East:       return West;
		case West:       return East;
		case Center:     return Center;
		case Unaligned:  return Unaligned;
		}

		return Unaligned;
	}
} Alignment;

}

#endif
