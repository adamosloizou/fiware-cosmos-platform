/* ****************************************************************************
*
* FILE            logMsg.c - log message source file
*
* AUTHOR          Ken Zangelin
*
*
* TODO
* o We want only one file and one stdout
* o New function - lmRestart(int index): reopen log file
* o New function - lmOnlyErrors()
*      For stdout: print only errors to stdout
*
* HISTORY
* $Log: logMsg.c,v $
* Revision 1.23  2000/04/07 10:50:40  kzangeli
* Fixed stupid bug calling baVsprintf instead of baVsnprintf.
*
* Revision 1.22  2000/03/20 15:56:36  kzangeli
* Using baSnprintf.
* Removed use of strcpy and strcat (for strncpy and strncat).-
*
* Revision 1.21  1999/12/22 11:06:47  baigorri
* *** empty log message ***
*
* Revision 1.20  1999/12/17 09:17:04  kzangeli
* *** empty log message ***
*
*
*
*/
#define F_OK 0

#include <sys/types.h>          /* types needed for other includes           */
#include <stdio.h>              /*                                           */
#include <unistd.h>             /* getpid, write, ...                        */
#include <string.h>             /* strncat, strdup, memset                   */
#include <stdarg.h>             /* va_start, va_arg, va_end                  */
#include <stdlib.h>             /* atoi                                      */
#if !defined(__APPLE__)
#include <malloc.h>             /* free                                      */
#endif
#include <fcntl.h>              /* O_RDWR, O_TRUNC, O_CREAT                  */
#include <ctype.h>              /* isprint                                   */
#include <sys/stat.h>           /* fstat, S_IFDIR                            */
#include <sys/time.h>           /* gettimeofday                              */
#include <time.h>               /* time, localtime_r, .                      */

#include "logMsg.h"             /* Own interface                             */



/******************************************************************************
*
* globals
*/
int inSigHandler = 0;



/* ****************************************************************************
*
* parameter checking macros
*/
#define INIT_CHECK() \
	if (initDone == false) \
		return LmsInitNotDone

#define PROGNAME_CHECK() \
	if (progName == NULL) \
		return LmsPrognameNotSet

#define INDEX_CHECK(index) \
	if ((index < 0) || (index >= FDS_MAX)) \
		return LmsFdNotFound

#define STRING_CHECK(s, l) \
	if (s == NULL) \
		return LmsNull; \
	if (strlen(s) >= l) \
		return LmsStringTooLong

#define POINTER_CHECK(p) \
	if (p == NULL) \
		return LmsNull

#define NOT_OCC_CHECK(i) \
	if (fds[i].state != Occupied) \
		return LmsFdNotFound;


#define LOG_OUT(s)


/* ****************************************************************************
*
* DELIMITER -    a comma is used as delimiter in the trace level format string
* ADD -          value for adding levels
* SUB -          value for subtracting levels
* TRACE_LEVELS - number of trace levels
*/
#define DELIMITER        ','
#define ADD              0
#define SUB              1
#define TRACE_LEVELS     256
#define FDS_MAX          2
#define LINE_MAX         4096
#define TEXT_MAX         512
#define FORMAT_LEN       512
#define FORMAT_DEF       "TYPE:DATE:EXEC/FILE[LINE] FUNC: TEXT"
#define DEF1             "TYPE:EXEC/FUNC: TEXT"
#define TIME_FORMAT_DEF  "%A %d %h %H:%M:%S %Y"
#define F_LEN            128
#define TF_LEN           64
#define INFO_LEN         512
#define TMS_LEN          20
#define TME_LEN          20
#define TMS_DEF          "<trace "
#define TME_DEF          ">"
#define AUX_LEN          16
#define LOG_PERM         0666
#define LOG_MASK         0



/* ****************************************************************************
*
* FdState - 
*/
typedef enum FdState
{
	Free = 0,
	Occupied
} FdState;



/* ****************************************************************************
*
* Type - 
*/
typedef enum Type
{
	Stdout,
	Fichero
} Type;




/* ****************************************************************************
*
* Fds - file descriptors of log files
*/
typedef struct Fds
{
	int        fd;                   /* file descriptor                      */
	int        type;                 /* stdout or file                       */
	int        state;                /* Free or Occupied                     */
	char       format[F_LEN];        /* Output format                        */
	char       timeFormat[TF_LEN];   /* Output time format                   */
	char       info[INFO_LEN];       /* fd info (path, stdout, ...)          */
	char       tMarkStart[TMS_LEN];  /* Start string for trace at EOL        */
	char       tMarkEnd[TME_LEN];    /* End string for trace at EOL          */
	LmWriteFp  write;                /* Other write function                 */
	bool       traceShow;            /* show trace level at EOL (if LM_T)    */
	bool       onlyErrorAndVerbose;  /* No tracing to stdout                 */
} Fds;



/* ****************************************************************************
*
* Line - 
*/
typedef struct Line
{
	char type;
	char remove;
} Line;



/* ****************************************************************************
*
* Global variables
*/
char* progName = (char*) "PROGNAME";



/* ****************************************************************************
*
* Static Variables
*/
static Fds               fds[FDS_MAX];
static bool              initDone;
static time_t            secondsAtStart;

static bool              tLevel[TRACE_LEVELS];

static LmExitFp          exitFunction    = NULL;
static void*             exitInput       = NULL;
static LmWarningFp       warningFunction = NULL;
static void*             warningInput    = NULL;
static LmErrorFp         errorFunction   = NULL;
static void*             errorInput      = NULL;

static char              aux[AUX_LEN];
static bool              auxOn = false;

static int               atLines   = 5000;
static int               keepLines = 1000;
static int               lastLines = 1000;

static int               logLines  = 0;
static bool              doClear   = true;
static int               fdNoOf    = 0;

static LmTracelevelName  userTracelevelName     = NULL;
static int               lmSd                   = -1;



/* ****************************************************************************
*
* Global variables
*/
bool  lmDebug   = false;
bool  lmVerbose = false;
bool  lmToDo    = false;
bool  lmDoubt   = false;
bool  lmReads   = false;
bool  lmWrites  = false;
bool  lmBug     = false;
bool  lmBuf     = false;
bool  lmFix     = false;

LmxFp lmxFp     = NULL;



/* ****************************************************************************
*
* lmProgName - 
*/
char* lmProgName(char* pn, int levels, bool pid)
{
	char*        start;
	static char  pName[128];

	if (pn == NULL)
		return NULL;

	if (levels < 1)
		levels = 1;

	start = &pn[strlen(pn) - 1];
	while (start > pn)
	{
		if (*start == '/')
			levels--;
		if (levels == 0)
			break;
		--start;
	}

	if (*start == '/')
		++start;

	strncpy(pName, start, sizeof(pName));
	if (pid == true)
	{
		char  pid[8];
		strncat(pName, "_", sizeof(pName));
		sprintf(pid, "%d", (int) getpid());
		strncat(pName, pid, sizeof(pName));		
	}

	return pName;
}
/* ****************************************************************************
*
* lmTraceIsSet - 
*/
bool lmTraceIsSet(int level)
{
	return tLevel[level];
}



/* ****************************************************************************
*
* lmTraceNameCbSet - 
*/
void lmTraceNameCbSet(LmTracelevelName cb)
{
	userTracelevelName = cb;
}



/* ****************************************************************************
*
* addLevels - 
*/
static void addLevels(bool* tLevelP, unsigned char from, unsigned char to)
{
	int   i;

	for (i = from; i <= to; i++)
	{
		LOG_OUT(("ADD: trace level %d is set", i));
		tLevelP[i] = true;
	}
}



/* ****************************************************************************
*
* subLevels - 
*/
static void subLevels(bool* tLevelP, unsigned char from, unsigned char to)
{
	int i;

	for (i = from; i <= to; i++)
	{
		LOG_OUT(("trace level %d is removed", i));
		tLevelP[i] = false;
	}
}



/* ****************************************************************************
*
* zeroDelimiter - 
*/
static char* zeroDelimiter(char* string, char delimiter)
{
    if (string == NULL)
	return NULL;

    while (*string != 0)
    {
	if (*string == delimiter)
	{
	    *string = 0;
	    return ++string;
	}
	else
	    ++string;
    }

    return NULL;
}



/* ****************************************************************************
*
* ws - is the character 'c' a whitespace (space, tab or '\n')
*/
static bool ws(char c)
{
	switch (c)
	{
	case ' ':
	case '\t':
	case '\n':
		return true;
		break;
	default:
		return false;
		break;
	}
}



/* ****************************************************************************
*
* wsNoOf - number of whitespaces in the string 'string'
*/
static int wsNoOf(char* string)
{
	int no = 0;

	while (*string != 0)
	{
		if (ws(*string) == true)
			++no;
		++string;
	}
	return no;
}



/* ****************************************************************************
*
* traceFix - fix trace levels
*
* DESCRIPTION
* traceFix sets the trace levels according to the level format string
* 'levelFormat' and the way 'way' (ADD or SUB).
* 'levelFormat' contains a number of levels, delimited by comma (,) and with
* the format (x and y are integers .LT. 256):
*
* 1. x     set level x
* 2. <x    set all levels from 0 to x   (including 0)
* 3. <=x   set all levels from 0 to x   (including 0 and x)
* 4. >x    set all levels from x to 256 (including 256)
* 5. >x    set all levels from x to 256 (including x and 256)
* 6. x-y   set all levels from x to y   (including x and y)
*
*
* NOTE
* Whitespace is not allowed in the level format string.
*
*/
static void traceFix(char* levelFormat, unsigned int way)
{
	char*          currP;
	char*          nextP;
	unsigned char  min;
	unsigned char  max;
	char*          minusP;
	char*          levelFormatP;


	/* No whitespace allowed + ADD or SUB */
	if ((wsNoOf(levelFormat) != 0) || (way > 1))
		return;

	levelFormatP = strdup(levelFormat);

	currP = &levelFormatP[0];

	while (currP != NULL)
	{
		nextP = zeroDelimiter(currP, DELIMITER);
		
		if (*currP == '<')
		{
			++currP;
			min  = 0;
			if (*currP == '=')
				max  = atoi(&currP[1]);
			else
			{
				max  = atoi(currP);
				if (max == 0)
					min = 1;
				else
					--max;
			}
		}
		else if (*currP == '>')
		{
			++currP;
			max  = TRACE_LEVELS - 1;
			if (*currP == '=')
				min  = atoi(&currP[1]);
			else
			{
				min  = atoi(currP);
				if (min == 255)
					max = 254;
				else
					++min;
			}
			
		}
		else if ((*currP >= '0') && (*currP <= '9'))
		{
			minusP       = zeroDelimiter(currP, '-');
			min          = atoi(currP);
			max          = min;
			
			if (minusP != NULL)
				max = atoi(minusP);
		}
		else
			break;
		
		if (way == ADD)
			addLevels(tLevel, min, max);
		else if (way == SUB)
			subLevels(tLevel, min, max);

		currP = nextP;
	}

	free(levelFormatP);	
	return;
}



/* ****************************************************************************
*
* dateGet - 
*/
static char* dateGet(int index, char* line, int lineSize)
{
	time_t     secondsNow = time(NULL);
	struct tm tmP;

	if (strcmp(fds[index].timeFormat, "UNIX") == 0)
	{
		char secs[32];

		sprintf(secs, "%ds", (int) secondsNow);
		strncpy(line, secs, lineSize);
	}
	else if (strcmp(fds[index].timeFormat, "DIFF") == 0)
	{
		int tm = (int) secondsNow - (int) secondsAtStart;
		int days;
		int hours;
		int mins;
		int secs;
		
		secs  = tm % 60;
		tm    = tm / 60;
		mins  = tm % 60;
		tm    = tm / 60;
		hours = tm % 24;
		tm    = tm / 24;
		days  = tm;

		if (days != 0)
			snprintf(line, lineSize, "%d days %02d:%02d:%02d",
				 days, hours, mins, secs);
		else
			snprintf(line, lineSize, "%02d:%02d:%02d", hours, mins, secs);
	}
	else
	{
		localtime_r(&secondsNow, &tmP);
		strftime(line, 80, fds[index].timeFormat, &tmP);
	}
	
	return line;
}


/* ****************************************************************************
*
* timeStampGet - 
*/
static char* timeStampGet(char *line)
{
	struct timeval tv;
	

	gettimeofday(&tv, NULL);

	snprintf(line, LINE_MAX, "timestamp: %d.%.6d secs\n", (int)tv.tv_sec, (int)tv.tv_usec);
	
	return line;
}


#define CHAR_ADD(c, l)                   \
do                                       \
{                                        \
	char xin[2];                         \
	xin[0] = c;                          \
	xin[1] = 0;                          \
	strncat(line, xin, sizeof(xin));     \
	fi += l;                             \
} while (0)

#define STRING_ADD(s, l)                 \
do                                       \
{                                        \
	strncat(line, s, lineLen);       \
	fi += l;                         \
} while (0)

#define INT_ADD(i, l)                          \
do                                             \
{                                              \
	char xin[20];                          \
	snprintf(xin, sizeof(xin), "%d", i);   \
	strncat(line, xin, lineLen);           \
	fi += l;                               \
} while (0)

#define TLEV_ADD(type, tLev)                                    \
do                                                              \
{                                                               \
	char xin[4];                                            \
                                                                \
	if ((type != 'T') && (type != 'X'))                     \
		strncpy(xin, "   ", sizeof(xin));               \
	else                                                    \
		snprintf(xin, sizeof(xin), "%03d", tLev);       \
	strncat(line, xin, lineLen);                            \
	fi += 4;                                                \
} while (0)



/* ****************************************************************************
*
* lmLineFix - 
*/
static char* lmLineFix
(
	int          index,
	char*        line,
	int          lineLen,
	char         type,
	const char*  file,
	int          lineNo,
	const char*  fName,
	int          tLev
)
{
	char   xin[256];
	int    fLen;
	int    fi     = 0;
	Fds*   fdP    = &fds[index];
	char*  format = fdP->format;

	memset(line, 0, lineLen);

	fLen = strlen(format);
	while (fi < fLen)
	{
		if (strncmp(&format[fi], "TYPE", 4) == 0)
			CHAR_ADD((type == 'P')? 'E' : type, 4);
		else if (strncmp(&format[fi], "PID", 3) == 0)
			INT_ADD((int) getpid(), 3);
		else if (strncmp(&format[fi], "DATE", 4) == 0)
			STRING_ADD(dateGet(index, xin, sizeof(xin)), 4);
		else if (strncmp(&format[fi], "EXEC", 4) == 0)
			STRING_ADD(progName, 4);
		else if (strncmp(&format[fi], "AUX", 3) == 0)
			STRING_ADD(aux, 3);
		else if (strncmp(&format[fi], "FILE", 4) == 0)
			STRING_ADD(file, 4);
		else if (strncmp(&format[fi], "LINE", 4) == 0)
			INT_ADD(lineNo, 4);
		else if (strncmp(&format[fi], "TLEV", 4) == 0)
			TLEV_ADD(type, tLev);
		else if (strncmp(&format[fi], "TEXT", 4) == 0)
			STRING_ADD("%s", 4);
		else if (strncmp(&format[fi], "FUNC", 4) == 0)
			STRING_ADD(fName, 4);
		else  /* just a normal character */
			CHAR_ADD(format[fi], 1);
	}

	if ((type == 'T') && (fdP->traceShow == true))
		snprintf(xin, sizeof(xin), "%s%d%s\n",
				   fdP->tMarkStart, tLev, fdP->tMarkEnd);
#if 0
	else if (type == 'P') /* type 'P' => tLev == errno */
		snprintf(xin, sizeof(xin), ": %s\n", strerror(tLev)); 
#endif
	else if (type == 'x') /* type 'x' => */
		snprintf(xin, sizeof(xin), ": %s\n", strerror(errno));
	else
		strncpy(xin, "\n", sizeof(xin));

	strncat(line, xin, lineLen);

	return line;
}



/* ****************************************************************************
*
* fdsFreeGet - 
*/
static int fdsFreeGet(void)
{
	int i = 0;

	while (i < FDS_MAX)
	{
		if (fds[i].state != Occupied)
			return i;
		i++;
	}
	
	return -1;
}



/* ****************************************************************************
*
* isdir - is path a directory?
*/
static bool isdir(char* path)
{
	struct stat xStat;
	
	if (stat(path, &xStat) == -1)
		return false;
	else if ((xStat.st_mode & S_IFDIR) == S_IFDIR)
		return true;

	return false;
}



/* ****************************************************************************
*
* asciiToLeft - 
*/
static void asciiToLeft
(
	char*     line,
	int       lineLen,
	char*     buffer,
	int    	  size,
	LmFormat  form,
	int       last
)
{
    int   i;
    int   offset;
	char  tmp[80];

	switch (form)
	{
	case LmfByte: offset = 16 * 3 + 2 - size * 3;                 break;
	case LmfWord: offset = 8 * 5 + 2 - (size / 2) * 5 - last;     break;
	case LmfLong: offset = 4 * 9 + 2 - (size / 4) * 9 - last * 2; break;
	default:      return;
	}

	while (offset-- >= 0)
		strncat(line, " ", lineLen);
    
	for (i = 0; i < size; i++)
	{
		if (buffer[i] == 0x25)
			strncpy(tmp, ".", sizeof(tmp));
		else if (isprint((int) buffer[i]))
			snprintf(tmp, sizeof(tmp), "%c", buffer[i]);
		else
			strncpy(tmp, ".", sizeof(tmp));

		strncat(line, tmp, lineLen);
	}
}



/* ************************************************************************* */
/* ********************** EXTERNAL FUNCTIONS ******************************* */
/* ************************************************************************* */



/* ****************************************************************************
*
* lmInit - 
*/
LmStatus lmInit(void)
{
	if (fdNoOf == 0)
		return LmsNoFiles;

	if (initDone == true)
		return LmsInitAlreadyDone;
	
	PROGNAME_CHECK();

	secondsAtStart = time(NULL);

	auxOn          = false;
	aux[0]         = 0;
	memset(tLevel, 0, sizeof(tLevel));
	logLines = 4;
	initDone = true;

	return LmsOk;
}



/* ****************************************************************************
*
* lmInitX - 
*/
LmStatus lmInitX(char* pName, char* tLevel, int* i1P, int* i2P)
{
	LmStatus s;

	if ((progName = lmProgName(pName, 1, false)) == NULL)
		return LmsPrognameError;

	if ((s = lmFdRegister(1, DEF1, "DEF", "stdout", i1P)) != LmsOk)
		return s;

	if ((s = lmPathRegister("/tmp", "DEF", "DEF", i2P)) != LmsOk)
		return s;

	if ((s = lmInit()) != LmsOk)
		return s;
	
	if ((s = lmTraceSet(tLevel)) != LmsOk)
		return s;

	return LmsOk;
}



/* ****************************************************************************
*
* lmStrerror
*/
const char* lmStrerror(LmStatus s)
{
	switch (s)
	{
	case LmsOk:               return "no problem";

	case LmsInitNotDone:      return "lmInit must be called first";
	case LmsInitAlreadyDone:  return "init already done";
	case LmsNull:             return "NULL pointer";
	case LmsFdNotFound:       return "fd not found";
	case LmsFdInvalid:        return "invalid file descriptor";
	case LmsFdOccupied:       return "file descriptor occupied";

	case LmsMalloc:           return "malloc error";
	case LmsOpen:             return "cannot open file";
	case LmsFopen:            return "error fopening for clearing out file";
	case LmsLseek:            return "lseek error";
	case LmsFseek:            return "fseek error";
	case LmsWrite:            return "write error";
	case LmsFgets:            return "error reading (fgets)";

	case LmsNoFiles:          return "no log files registrated";
	case LmsProgNameLevels:   return "bad program name level";
	case LmsLineTooLong:      return "log line too long";
	case LmsStringTooLong:    return "string too long";
	case LmsBadFormat:        return "bad format";
	case LmsBadSize:          return "bad size";
	case LmsBadParams:        return "bad parameters";
	case LmsPrognameNotSet:   return "progName not set";
	case LmsPrognameError:    return "error setting progName";
	case LmsClearNotAllowed:  return "clear option not set";
	}
	
	return "status code not recognized";
}



/* ***************************************************************************
* 
* lmTraceSet - set trace levels
*
* DESCRIPTION
* See lmTraceAdd
*/
LmStatus lmTraceSet(char* levelFormat)
{

    INIT_CHECK();

    if (levelFormat == NULL)
        return LmsOk;

    subLevels(tLevel, 0, TRACE_LEVELS - 1);
    traceFix(levelFormat, ADD);

    return LmsOk;
}



/* ***************************************************************************
* 
* lmTraceAdd - add trace levels
*
* DESCRIPTION
* See traceFix
*/
LmStatus lmTraceAdd(char* levelFormat)
{
	INIT_CHECK();

	traceFix(levelFormat, ADD);

	return LmsOk;
}



/* ***************************************************************************
* 
* lmTraceSub - remove trace levels
*
* DESCRIPTION
* See traceFix
*/
LmStatus lmTraceSub(char* levelFormat)
{
	traceFix(levelFormat, SUB);

	return LmsOk;
}



/* ****************************************************************************
*
* lmTraceGet - 
*
* NOTE
* levelString must be a string of at least 80 characters
*/
char* lmTraceGet(char* levelString)
{
	int       i;
	int       j = 0;
	int       levels[256];

	if (levelString == NULL)
	{
		LOG_OUT(("returning NULL"));
		return NULL;
	}
	
	levelString[0] = 0;

	for (i = 0; i < 256; i++)
	{
		if (tLevel[i] == true)
		{
			LOG_OUT(("GET: trace level %d is set", i));
			levels[j++] = i;
		}
	}

	if (j == 0)
	{
		LOG_OUT(("returning '%s'", levelString));
		return levelString;
	}
	
	snprintf(levelString, 80, "%d", levels[0]);
	
	for (i = 1; i < j; i++)
	{
		int       prev   = levels[i - 1];
		int       diss   = levels[i];
		int       next   = levels[i + 1];
		bool      before = (diss == prev + 1);
		bool      after  = (diss == next - 1);

		if (i == 255)
			after = false;

		if (before && after)
			;
		else if (before && !after)
		{
			char str[12];
			snprintf(str, sizeof(str), "-%d", diss);
			strncat(levelString, str, 80);
		}
		else if (!before && after)
		{
			char str[12];
			snprintf(str, sizeof(str), ", %d", diss);
			strncat(levelString, str, 80);
		}
		else if (!before && !after)
		{
			char str[12];
			snprintf(str, sizeof(str), ", %d", diss);
			strncat(levelString, str, 80);
		}
	}
	
	return levelString;
}



/* ****************************************************************************
*
* lmWriteFunction - use alternative write function
*/
LmStatus lmWriteFunction(int i, LmWriteFp fp)
{
	INIT_CHECK();
	INDEX_CHECK(i);
	NOT_OCC_CHECK(i);

	fds[i].write = fp;

	return LmsOk;
}



/* ****************************************************************************
*
* lmDoClear - 
*/
LmStatus lmDoClear(void)
{
	INIT_CHECK();
	doClear = true;
	return LmsOk;
}



/* ****************************************************************************
*
* lmDontClear - 
*/
LmStatus lmDontClear(void)
{
	INIT_CHECK();
	doClear = false;
	return LmsOk;
}



/* ****************************************************************************
*
* lmClearAt - 
*/
LmStatus lmClearAt(int atL, int keepL, int lastL)
{
	int a = atLines;
	int k = keepLines;
	int l = lastLines;

	INIT_CHECK();

	if (atL   != -1)      a = atL;
	if (keepL != -1)      k = keepL;
	if (lastL != -1)      l = lastL;

	if (a < (k + 4 + l))
		return LmsBadParams;

	atLines   = a;
	keepLines = k;
	lastLines = l;

	return LmsOk;
}



/* ****************************************************************************
*
* lmClearGet - 
*/
void lmClearGet
(
	bool*      clearOn,
	int*       atP,
	int*       keepP,
	int*       lastP,
	int*       logFileBytesP
)
{
	if (clearOn != NULL)
		*clearOn = doClear;

	if (atP != NULL)
		*atP = atLines;

	if (keepP != NULL)
		*keepP = keepLines;

	if (lastP != NULL)
		*lastP = lastLines;

	if (logFileBytesP != NULL)
		*logFileBytesP = logLines;
}



/* ****************************************************************************
*
* lmFormat - 
*/
LmStatus lmFormat(int index, char* f)
{
	INIT_CHECK();
	INDEX_CHECK(index);
	STRING_CHECK(f, F_LEN);

	if (strcmp(f, "DEF") == 0)
		strncpy(fds[index].format, FORMAT_DEF, sizeof(fds[index].format));
	else
		strncpy(fds[index].format, f, sizeof(fds[index].format));

	return LmsOk;
}



/* ****************************************************************************
*
* lmTimeFormat - 
*/
LmStatus lmTimeFormat(int index, char* f)
{
	INIT_CHECK();
	INDEX_CHECK(index);
	STRING_CHECK(f, TF_LEN);

	if (strcmp(f, "DEF") == 0)
		strncpy(fds[index].timeFormat, TIME_FORMAT_DEF,
				sizeof(fds[index].timeFormat));
	else
		strncpy(fds[index].timeFormat, f, sizeof(fds[index].timeFormat));

	return LmsOk;
}



/* ****************************************************************************
*
* lmGetInfo - 
*
* NOTE
* info must be a pointer to a buffer of at least the size 80 bytes 
*/
LmStatus lmGetInfo(int index, char* info)
{
	INIT_CHECK();
	INDEX_CHECK(index);
	NOT_OCC_CHECK(index);

	strncpy(info, fds[index].info, 80);

	return LmsOk;
}



/* ****************************************************************************
*
* lmFdGet - 
*/
LmStatus lmFdGet(int index, int* iP)
{
	INIT_CHECK();
	INDEX_CHECK(index);
	NOT_OCC_CHECK(index);

	*iP = fds[index].fd;

	return LmsOk;
}



/* ****************************************************************************
*
* lmTraceAtEnd - I don't like the name of this function ...
*/
LmStatus lmTraceAtEnd(int index, char* start, char* end)
{
	INIT_CHECK();
	INDEX_CHECK(index);
	STRING_CHECK(start, TMS_LEN);
	STRING_CHECK(end, TME_LEN);

	if ((start == NULL) || (end == NULL))
		fds[index].traceShow = false;
	else
	{
		if (strcmp(start, "DEF") == 0)
			strncpy(fds[index].tMarkStart, TMS_DEF,
					sizeof(fds[index].tMarkStart));
		else
			strncpy(fds[index].tMarkStart, start,
					sizeof(fds[index].tMarkStart));

		if (strcmp(end, "DEF") == 0)
			strncpy(fds[index].tMarkEnd, TME_DEF, sizeof(fds[index].tMarkEnd));
		else
			strncpy(fds[index].tMarkEnd, end, sizeof(fds[index].tMarkEnd));

		fds[index].traceShow  = true;
	}

	return LmsOk;
}



/* ****************************************************************************
*
* lmAux - 
*/
LmStatus lmAux(char* a)
{
	INIT_CHECK();
	STRING_CHECK(a, AUX_LEN);

	strncpy(aux, a, sizeof(aux));
	auxOn = true;

	return LmsOk;
}



/* ****************************************************************************
*
* lmTextGet - format message into string
*/
char* lmTextGet(const char* format, ...)
{
	va_list        args;
	char           vmsg[2048];
	char*          allocedString;
	char*          nl;

	/* "Parse" the varible arguments */
	va_start(args, format);

	/* Print message to variable */
	vsnprintf(vmsg, sizeof(vmsg), format, args);
	vmsg[2047] = 0;
	va_end(args);

	if ((nl = strchr(vmsg, '\n')) != NULL)
		*nl = 0;

	allocedString = (char*) strdup(vmsg);

	return allocedString;
}



/* ****************************************************************************
*
* lmOk - 
*/
LmStatus lmOk(char type, int tLev)
{
	if ((type == 'T') && (tLevel[tLev] == false))
		return LmsNull;
	if ((type == 'D') && (lmDebug == false))
		return LmsNull;
	if ((type == 'V') && (lmVerbose == false))
		return LmsNull;
	if ((type == 't') && (lmToDo == false))
		return LmsNull;
	if ((type == 'w') && (lmWrites == false))
		return LmsNull;
	if ((type == 'r') && (lmReads == false))
		return LmsNull;
	if ((type == 'b') && (lmBuf == false))
		return LmsNull;
	if ((type == 'B') && (lmBug == false))
		return LmsNull;
	if ((type == 'F') && (lmFix == false))
		return LmsNull;
	if ((type == 'd') && (lmDoubt == false))
		return LmsNull;

	return LmsOk;
}



/* ****************************************************************************
*
* lmFdRegister - 
*/
LmStatus lmFdRegister(int fd, const char* format, const char* timeFormat, const char* info, int* indexP)
{
	char       startMsg[256];
	char       dt[256];
	int        sz;
	int        index;
	time_t     secsNow;
	struct tm tmP;

	PROGNAME_CHECK();

	if (initDone == true)
		return LmsInitAlreadyDone;

	if (fdNoOf == 0)
	{
		int i;
		for (i = 0; i < FDS_MAX; i++)
			fds[i].state = Free;
	}
	
	STRING_CHECK(info, INFO_LEN);
	STRING_CHECK(format, F_LEN);
	STRING_CHECK(timeFormat, TF_LEN);

	if (fd < 0)
		return LmsFdInvalid;

	if ((index = fdsFreeGet()) == -1)
		return LmsFdOccupied;

	secsNow = time(NULL);
	localtime_r(&secsNow, &tmP);
	
	if ((fd >= 0) && (strcmp(info, "stdout") != 0))
	{
		strftime(dt, 256, "%A %d %h %H:%M:%S %Y", &tmP);
		snprintf(startMsg, sizeof(startMsg),
				   "%s log\n-----------------\nStarted %s\nCleared at ...\n",
				   progName, dt);

		sz = strlen(startMsg);

		if (write(fd, startMsg, sz) != sz)
			return LmsWrite;
	}

	fds[index].fd    = fd;
	fds[index].state = Occupied;
	if (strcmp(format, "DEF") == 0)
		strncpy(fds[index].format, FORMAT_DEF, sizeof(fds[index].format));
	else
		strncpy(fds[index].format, format, sizeof(fds[index].format));

	if (strcmp(timeFormat, "DEF") == 0)
		strncpy(fds[index].timeFormat, TIME_FORMAT_DEF,
				sizeof(fds[index].timeFormat));
	else
		strncpy(fds[index].timeFormat, timeFormat,
				sizeof(fds[index].timeFormat));

	strncpy(fds[index].info, info, sizeof(fds[index].info));

	if (indexP)
	   *indexP = index;
	fdNoOf++;
	fds[index].type = Stdout;

	if ((strcmp(info, "stdout") == 0) ||  (strcmp(info, "stderr") == 0))
	{
		if (indexP)
			lmSd = *indexP;
	}

	return LmsOk;
}



/* ****************************************************************************
*
* lmPathRegister - 
*/
LmStatus lmPathRegister(const char* path, const char* format, const char* timeFormat, int* indexP)
{
	int       fd;
	LmStatus  s;
	char      fileName[512];
	int       index;

	PROGNAME_CHECK();

	if (initDone == true)
		return LmsInitAlreadyDone;

	STRING_CHECK(format, F_LEN);
	
	if (isdir((char*) path) == true)
		snprintf(fileName, sizeof(fileName), "%s/%sLog", path, &progName[0]);
	else
		strncpy(fileName, path, sizeof(fileName));

	if (access(fileName, F_OK) == 0)
	{
		char newName[512];
		snprintf(newName, sizeof(newName), "%s.old", fileName);
		rename(fileName, newName);
	}
		
	fd = open(fileName, O_RDWR | O_TRUNC | O_CREAT, 0777);
	if (fd == -1)
	{
		char str[256];
		snprintf(str, sizeof(str), "open(%s)", fileName);
		perror(str);
		return LmsOpen;
	}
	
	chmod(fileName, 0777);

	s = lmFdRegister(fd, format, timeFormat, fileName/* info */, &index);
	if (s != LmsOk)
	{
		close(fd);
		return s;
	}


	fds[index].type = Fichero;

	if (indexP)
		*indexP = index;

	return LmsOk;
}



/* ****************************************************************************
*
* lmOut - 
*/
LmStatus lmOut(char* text, char type, const char* file, int lineNo, const char* fName,
			   int tLev, const char* stre)
{
	int   i;
	char  line[LINE_MAX];
	int   sz;
	int   nb;
	char  format[FORMAT_LEN + 1];
	char* tmP;

	tmP = strrchr((char*) file, '/');
	if (tmP != NULL)
	   file = &tmP[1];

	if (inSigHandler && (type != 'X' || type != 'x'))
	{
		lmAddMsgBuf(text, type, file, lineNo, fName, tLev, (char*) stre);
		return LmsOk;
	} 


	INIT_CHECK();
	POINTER_CHECK(format);
	POINTER_CHECK(text);
	
	
	memset(line,   0, sizeof(line));
	memset(format, 0, sizeof(format));

	for (i = 0; i < FDS_MAX; i++)
	{
		if (fds[i].state != Occupied)
			continue;

		if ((fds[i].type == Stdout) && (fds[i].onlyErrorAndVerbose == true))
		{
			if ((type == 'T')
			||  (type == 'D')
			||  (type == 't'))
				continue;
		}

		if (type == 'R')
		{
			if (text[1] != ':')
				snprintf(line, sizeof(line), "R: %s\n%c", text, 0);
			else
				snprintf(line, sizeof(line), "%s\n%c", text, 0);
		}
		else if (type == 'S')
		{
			char stampStr[LINE_MAX];
			snprintf(line, sizeof(line), "%s:%s", text, timeStampGet(stampStr));

		}
		else
		{
			/* Danger: 'format' might be too short ... */
			if (lmLineFix(i, format, sizeof(format), type, file, lineNo, fName, tLev) == NULL)
				continue;

			if ((strlen(format) + strlen(text) + strlen(line)) > LINE_MAX)
				snprintf(line, sizeof(line), "%s\n%c",
						   "LM ERROR: LINE TOO LONG", 0);
			else
				snprintf(line, sizeof(line), format, text);
		}
		if (stre != NULL)
			strncat(line, stre, sizeof(line));


		sz = strlen(line);
		
		if (fds[i].write != NULL)
		   fds[i].write(line);
		else
		{
			if ((nb = write(fds[i].fd, line, sz)) != sz)
			{
				if ((nb == -1) && (errno != EINTR))
					lmReopen(i);
				return LmsWrite;
			}
		}
	}
	
	++logLines;
	LOG_OUT(("logLines: %d", logLines));

	if (type == 'W')
	{
		if (warningFunction != NULL)
		{
			fprintf(stderr, "Calling warningFunction (at %p)\n", &warningFunction);

			warningFunction(warningInput, text, (char*) stre);
			fprintf(stderr, "warningFunction done\n");
		}
	}
	else if ((type == 'E') || (type == 'P'))
	{
		if (errorFunction != NULL)
			errorFunction(errorInput, text, (char*) stre);
	}
	else if ((type == 'X') || (type == 'x'))
	{
		if (exitFunction != NULL)
			exitFunction(tLev, exitInput, text, (char*) stre);

		/* exit here, just in case */
		exit(tLev);
	}
	
	if ((doClear == true) && (logLines >= atLines))
	{
		int i;
		
		for (i = 0; i < FDS_MAX; i++)
		{
			if (fds[i].state == Occupied)
			{
				LmStatus s;

				if (fds[i].type != Fichero)
					continue;

				if ((s = lmClear(i, keepLines, lastLines)) != LmsOk)
					return s;
			}
		}
	}

	return LmsOk;
}



/* ****************************************************************************
*
* lmExitFunction - 
*/
LmStatus lmExitFunction(LmExitFp fp, void* input)
{
	INIT_CHECK();
	POINTER_CHECK(fp);

	exitFunction = fp;
	exitInput    = input;
	
	return LmsOk;
}



/* ****************************************************************************
*
* lmWarningFunction - 
*/
LmStatus lmWarningFunction(LmWarningFp fp, void* input)
{
	INIT_CHECK();
	POINTER_CHECK(fp);

	warningFunction = fp;
	warningInput    = input;
	
	fprintf(stderr, "Set warningFunction to %p\n", &warningFunction);

	return LmsOk;
}



/* ****************************************************************************
*
* lmWarningFunctionDebug
*/
void lmWarningFunctionDebug(char* info, char* file, int line)
{
	fprintf(stderr, "%s[%d]: %s: warningFunction: %p\n", 
			file, line, info, &warningFunction);
}



/* ****************************************************************************
*
* lmErrorFunction - 
*/
LmStatus lmErrorFunction(LmErrorFp fp, void* input)
{
	INIT_CHECK();
	POINTER_CHECK(fp);

	errorFunction = fp;
	errorInput    = input;
	
	return LmsOk;
}



/******************************************************************************
*
* lmBufferPresent - 
*
* lmBufferPresent presents a buffer in a nice hexa decimal format.
*/
int lmBufferPresent
(
	char*       fName,
	char*       to,
	char*       description,
	void*       bufP,
	int         size, 
	LmFormat    format,
	int         type
)
{
	int   bIndex     = 0;
	int   bIndexLast = 0;
	int   xx         = 0;
	char* buffer     = (char*) bufP;
	int   start      = 0;
	char  line[160];
	char  tmp[80];
	char  msg[160];

	if (size > 0x800)
		size = 0x800;

	if (lmOk(type, type) != LmsOk)
		return LmsOk;

	if (size <= 0)
		return LmsBadSize;

	if ((format != LmfByte) && (format != LmfWord) && (format != LmfLong))
		return LmsBadFormat;

	memset(line, 0, sizeof(line));

	if (to != NULL)
	{
		snprintf(msg, sizeof(msg), "%s %s %s (%d bytes) %s %s", progName,
				(type == 'r')? "reading" : "writing", description, size,
				(type == 'r')? "from"    : "to", to);

		LM_RAW(("%c: ----  %s ----",  type, msg));
	}

	while (bIndex < size)
	{
		if ((bIndex % 0x10) == 0)
			snprintf(line, sizeof(line), "%c: %.8x:   ", type, start);

		switch (format)
		{
		case LmfLong:
			if (bIndex + 4 <= size)
			{
				snprintf(tmp, sizeof(tmp),"%.8x ",*((int*) &buffer[bIndex]));
				strncat(line, tmp, sizeof(line));
				bIndex += 4;
			}				
			else if (bIndex + 1 == size)
			{
				snprintf(tmp, sizeof(tmp), "%.2xxxxxxx",
						(*((int*) &buffer[bIndex]) & 0xFF000000) >> 24);
				strncat(line, tmp, sizeof(line));
				bIndex += 1;
				xx=1;
			}
			else if (bIndex + 2 == size)
			{
				snprintf(tmp, sizeof(tmp), "%.4xxxxx",
						(*((int*) &buffer[bIndex]) & 0xFFFF0000) >> 16);
				strncat(line, tmp, sizeof(line));
				bIndex += 2;
				xx=2;
			}
			else if (bIndex + 3 == size)
			{
				snprintf(tmp, sizeof(tmp), "%.6xxx",
						(*((int*) &buffer[bIndex]) & 0xFFFFFF00) >> 8);
				strncat(line, tmp, sizeof(line));
				bIndex += 3;
				xx=3;
			}
			
			break;

		case LmfWord:
			if (bIndex + 2 <= size)
			{
				snprintf(tmp, sizeof(tmp), "%.4x ", 
						   *((short*) &buffer[bIndex]) & 0xFFFF);
				strncat(line, tmp, sizeof(line));
				bIndex += 2;
			}
			else
			{
				snprintf(tmp, sizeof(tmp), "%.2xxx",
						(*((short*) &buffer[bIndex]) & 0xFF00) >> 8);
				strncat(line, tmp, sizeof(line));
				bIndex += 1;
				xx=1;
			}
			break;

		case LmfByte:
			snprintf(tmp, sizeof(tmp), "%.2x ", buffer[bIndex] & 0xFF);
			strncat(line, tmp, sizeof(line));
			bIndex += 1;
			break;
		}

		if (((bIndex % 0x10) == 0) || (bIndex >= size))
		{
			int len = bIndex - bIndexLast;

			if (bIndex > size)
				len = size - bIndexLast;
			
			asciiToLeft(line, sizeof(line), &buffer[bIndexLast], len,
						format, xx? 4 : 0);

			start     += 0x10;
			bIndexLast = bIndex;

			LM_RAW((line));
			memset(line, 0, sizeof(line));
		}
	}

	LM_RAW(("%c: --------------------------------------------------------------", type));

	return LmsOk;
}



/* ****************************************************************************
*
* lmReopen - 
*/
LmStatus lmReopen(int index)
{
	int      fdPos;
	LmStatus s;
	int      newLogLines = 0;
	FILE*    fP;
	char     tmpName[512];
	int      fd;

	fdPos = lseek(fds[index].fd, 0, SEEK_CUR);

	if ((fP = fopen(fds[index].info, "r")) == NULL)
		return LmsFopen;
	rewind(fP);

	s = LmsOk;

	snprintf(tmpName, sizeof(tmpName), "%s_%d",
			   fds[index].info, (int) getpid());

	if ((fd = open(tmpName, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
	{
		lseek(fds[index].fd, fdPos, SEEK_SET);
		return LmsOpen;
	}
	
	while (true)
	{
		char line[LINE_MAX];
		int len;
		int nb;

		if (fgets(line, LINE_MAX, fP) == NULL)
		{
			s = LmsFgets;
			break;
		}

		len = strlen(line);

		if (strncmp(line,  "Cleared ", 8) == 0)
		{
			time_t     now = time(NULL);
			struct tm tmP;
			char       tm[80];
			char       buf[80];
				
			localtime_r(&now, &tmP);
			strftime(tm, 80, TIME_FORMAT_DEF, &tmP);

			snprintf(buf, sizeof(buf), "Cleared at %s  (a reopen ...)\n",tm);
			if (write(fd, buf, strlen(buf)) != (int) strlen(buf))
			{
				s = LmsWrite;
				break;
			}	
			++newLogLines;
			break;
		}
		
		if ((nb = write(fd, line, len)) != len)
		{
			s = LmsWrite;
			break;
		}
		else
			++newLogLines;
	}

	if (fd != -1)
		if (close(fd) == -1)
			perror("close");
	if (fclose(fP) == -1)
		perror("fclose");

	if (s == LmsOk)
	{
		close(fds[index].fd);
		rename(tmpName, fds[index].info);
		fds[index].fd = open(fds[index].info, O_RDWR, 0666);

		if (fds[index].fd == -1)
		{
			fds[index].state = Free;
			return LmsOpen;
		}
		
		lseek(fds[index].fd, 0, SEEK_END);
		logLines = newLogLines;
	}
	
	return s;
}



typedef struct LineRemove
{
	char      type;
	int       offset;
	bool      remove;
} LineRemove;


/* ****************************************************************************
*
* lmClear - 
*/
LmStatus lmClear(int index, int keepLines, int lastLines)
{
#if 0
	LineRemove* lrV;
	int         initialLrv;
	char        line[LINE_MAX];
	int         i;
	int         j = 0;
	FILE*       fP;
	int         oldOffset;
	int         linesToRemove;
	char*       order       = "rwbRTdtDVFMWEPBXh";
	int         newLogLines = 0;
	int         fdPos;
	char        tmpName[512];
	int         len;
	int         fd;
	static int  headerLines = 4;

	INIT_CHECK();
	INDEX_CHECK(index);

	if (logLines < (keepLines + lastLines))
		return LmsOk;

	if ((fP = fopen(fds[index].info, "r")) == NULL)
	{
		atLines += 1000;
		if (atLines > 20000)
		{
			doClear = false;
			return LmsFopen;
		}

		return LmsFopen;
	}

	rewind(fP);

	lrV = (LineRemove*) malloc(sizeof(LineRemove) * (logLines + 4));
	if (lrV == NULL)
		return LmsMalloc;

	initialLrv = (int) lrV;
	memset(lrV, 0, sizeof(LineRemove) * (logLines + 4));

	i = 0;
	oldOffset = 0;
	while (fgets(line, LINE_MAX, fP) != NULL)
	{
		lrV[i].type   = (line[1] == ':')? line[0] : 'h';
		lrV[i].offset = oldOffset;
		lrV[i].remove = false;

		line[strlen(line) - 1] = 0;
		LOG_OUT(("got line %d: '%s'", i, line));
		oldOffset = ftell(fP);
		++i;
		if (i > logLines + 4)
			break;
	}
	
	linesToRemove = logLines - headerLines - keepLines - lastLines;

	LOG_OUT(("linesToRemove == %d", linesToRemove));
	LOG_OUT(("logLines      == %d", logLines));
	LOG_OUT(("headerLines   == %d", headerLines));
	LOG_OUT(("keepLines     == %d", keepLines));
	LOG_OUT(("lastLines     == %d", lastLines));
	LOG_OUT(("keeping lastLines (%d - EOF)", logLines - lastLines));
	LOG_OUT(("Removing from %d to %d", headerLines, logLines - lastLines));

	while (order[j] != 0)
	{
		for (i = headerLines; i < logLines - lastLines; i++)
		{
			if ((lrV[i].remove == false) && (lrV[i].type == order[j]))
			{
				lrV[i].remove = true;
				LOG_OUT(("Removing line %d", i));
				if (--linesToRemove <= 0)
					goto Removing;
			}			
		}
		++j;
	}


 Removing:
	fdPos = ftell(fP);
	rewind(fP);
	snprintf(tmpName, sizeof(tmpName), "%s_%d", fds[index].info,
			   (int) getpid());
	if ((fd = open(tmpName, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
	{
		perror("open");
		fclose(fP);
		lseek(fds[index].fd, fdPos, SEEK_SET);
		free((char*) lrV);
		return LmsOpen;
	}

#define CLEANUP(str, s) \
{ \
	if (str != NULL)                       \
		perror(str);                   \
	close(fd);                             \
	fclose(fP);                            \
	lseek(fds[index].fd, fdPos, SEEK_SET); \
	if (lrV != NULL)                       \
	    free((void*) lrV);                 \
	lrV = NULL;                            \
	unlink(tmpName);                       \
	return s;                              \
}

	for (i = 0; i < logLines; i++)
	{
		if (lrV[i].remove == false)
		{
			char line[LINE_MAX];

			if (fseek(fP, lrV[i].offset, SEEK_SET) != 0)
				CLEANUP("fseek", LmsFseek);

			fgets(line, LINE_MAX, fP);
			if (strncmp(line, "Cleared at", 10) != 0)
			{
				len = strlen(line);
				if (write(fd, line, len) != len)
					CLEANUP("write", LmsWrite);
			}

			if (newLogLines == 2)
			{
				time_t     now = time(NULL);
				struct tm tmP;
				char       tm[80];
				char       buf[80];
				
				localtime_r(&now, &tmP);
				strftime(tm, 80, TIME_FORMAT_DEF, &tmP);
				
				snprintf(buf, sizeof(buf), "Cleared at %s\n", tm);
				if (write(fd, buf, strlen(buf)) != strlen(buf))
					CLEANUP("write", LmsWrite);
			}

			++newLogLines;
		}
	}

	if (lrV != NULL)
	{
	    if ((int) lrV == initialLrv)
		free((void*) lrV);
	    else
		LOG_OUT(("ERROR: lrV has changed ... (from 0x%x to 0x%x)", (int) initialLrv, (int) lrV));
	}

	fclose(fP);
	close(fd);
	close(fds[index].fd);
	rename(tmpName, fds[index].info);
	fds[index].fd = open(fds[index].info, O_RDWR, 0666);

	if (fds[index].fd == -1)
	{
		fds[index].state = Free;
		return LmsOpen;
	}
		
	lseek(fds[index].fd, 0, SEEK_END);

	logLines = newLogLines;
	LOG_OUT(("Set logLines to %d", logLines));
#endif
	return LmsOk;
}



/* ****************************************************************************
*
* lmShowLog - 
*/
void lmShowLog(int logFd)
{
	char com[512];

	snprintf(com, sizeof(com), "echo xterm -e tail -f %s & > /tmp/xxx",
			   fds[logFd].info);
	system(com);
	chmod("/tmp/xxx", 0700);
	system("/tmp/xxx");
}



/* ****************************************************************************
*
* lmExitForced - 
*/
void lmExitForced(int c)
{
	if (exitFunction != NULL)
		exitFunction(c, exitInput, (char*) "exit forced", NULL);
	exit(c);
}



/* ****************************************************************************
*
* lmxFunction - extra function to call for LMX_ macros
*/
LmStatus lmxFunction(LmxFp fp)
{
	INIT_CHECK();
	POINTER_CHECK(fp);

	lmxFp = fp;

	return LmsOk;
}



/* ****************************************************************************
*
* lmOnlyErrors
*/
LmStatus lmOnlyErrors(int index)
{
	INIT_CHECK();
	INDEX_CHECK(index);

	if (fds[index].type == Stdout)
		fds[index].onlyErrorAndVerbose = true;

	return LmsOk;
}



/* ****************************************************************************
*
* lmTraceLevel - 
*/
const char* lmTraceLevel(int level)
{
	static char name[32];
	char*       userName = NULL;

	switch (level)
	{
	case LmtIn:              return "In function";
	case LmtFrom:            return "From function";
	case LmtLine:            return "Line number";
	case LmtEntry:           return "Function Entry";
	case LmtExit:            return "Function Exit";
	case LmtListCreate:      return "List Create";
	case LmtListInit:        return "List Init";
	case LmtListInsert:      return "List Insert";
	case LmtListItemCreate:  return "List Item Create";
	}

	if (userTracelevelName != NULL)
		userName = userTracelevelName(level);

	if (userName == NULL)
	{
		sprintf(name, "trace level %d", level);
		return name;
	}

	return (const char*) userName;
}



/* ****************************************************************************
*
* lmSdGet - 
*/
int lmSdGet(void)
{
	return lmSd;
}


struct logMsg 
{
	char msg[256];
	char type;
	char file[256];
	int  line;
	char func[256];
	int  tLev;
	char stre[256];
	struct logMsg* next;
};


static struct logMsg* logMsgs = NULL;



/* ****************************************************************************
*
* lmAddMsgBuf - This funtion was added because of the lack of reentrancy of 
* certain date functions (called from within signal handlers).
*/
void lmAddMsgBuf(char* text, char type, const char* file, int line, const char* func, int tLev, const char* stre)
{
	struct logMsg *newMsg, *logP, *lastlogP;

	newMsg = (logMsg*) malloc(sizeof(struct logMsg));
	if (newMsg == NULL)
		return;
	memset(newMsg, 0, sizeof(struct logMsg));

	if (text)
		strncpy(newMsg->msg, text, sizeof(newMsg->msg));
	if (file)
		strncpy(newMsg->file, file, sizeof(newMsg->file));
	newMsg->line = line;
	newMsg->type = type;
	if (func)
		strncpy(newMsg->func, func, sizeof(newMsg->func));
	if (stre)
		strncpy(newMsg->stre, stre, sizeof(newMsg->stre));
	if (tLev)
		newMsg->tLev = tLev;

	if (logMsgs)
	{
		logP = logMsgs;
		while (logP)
		{
			lastlogP = logP;
			logP     = logP->next;
		}
		lastlogP->next = newMsg;
		newMsg->next   = NULL;
	}
	else
	{
		logMsgs        = newMsg;
		newMsg->next   = NULL;
	}

	return;
}



/* ****************************************************************************
*
* lmPrintMsgBuf - 
*/
void lmPrintMsgBuf()
{

	struct logMsg *logP;

	while (logMsgs) 
	{
		logP = logMsgs;
		lmOut(logP->msg, logP->type, logP->file, logP->line, (char*) logP->func, logP->tLev, logP->stre);
		logMsgs = logP->next;
		free(logP);
	}
}
