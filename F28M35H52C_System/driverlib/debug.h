//###########################################################################
// FILE:   debug.h
// TITLE:  Macros for assisting debug of the driver library.
//###########################################################################
// $TI Release: F28M35x Support Library v201 $
// $Release Date: Fri Jun  7 10:51:13 CDT 2013 $
//###########################################################################

#ifndef __DEBUG_H__
#define __DEBUG_H__

//*****************************************************************************
// Prototype for the function that is called when an invalid argument is passed
// to an API.  This is only used when doing a DEBUG build.
//*****************************************************************************
extern void __error__(char *pcFilename, unsigned long ulLine);

//*****************************************************************************
// The ASSERT macro, which does the actual assertion checking.  Typically, this
// will be for procedure arguments.
//*****************************************************************************
#ifdef DEBUG
#define ASSERT(expr)    {                                      \
        if(!(expr))                        \
        {                                  \
            __error__(__FILE__, __LINE__); \
        }                                  \
}
#else
#define ASSERT(expr)
#endif

#endif // __DEBUG_H__



