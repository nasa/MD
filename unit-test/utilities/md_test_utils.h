#ifndef _MD_TEST_UTILS_H_
#define _MD_TEST_UTILS_H_

#include "md_app.h"
#include "utstubs.h"

extern MD_AppData_t MD_AppData;

/*
 * Global context structures
 */

typedef struct
{
    uint16      EventID;
    uint16      EventType;
    const char *Spec;
} __attribute__((packed)) CFE_EVS_SendEvent_context_t;

typedef struct
{
    const char *Spec;
} __attribute__((packed)) CFE_ES_WriteToSysLog_context_t;

/*
 * Function Definitions
 */

int32 UT_Utils_stub_reporter_hook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context);
void  MD_Test_Setup(void);
void  MD_Test_TearDown(void);

#endif

/************************/
/*  End of File Comment */
/************************/
