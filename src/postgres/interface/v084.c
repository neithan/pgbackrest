/***********************************************************************************************************************************
PostgreSQL 8.4 Interface

See postgres/interface.c for documentation.
***********************************************************************************************************************************/
#include "common/debug.h"
#include "common/log.h"
#include "postgres/interface/v084.h"

#include "postgres/interface/v084.auto.c"

/**********************************************************************************************************************************/
bool
pgInterfaceControlIs084(const Buffer *controlFile)
{
    FUNCTION_TEST_BEGIN();
        FUNCTION_TEST_PARAM(BUFFER, controlFile);
    FUNCTION_TEST_END();

    ASSERT(controlFile != NULL);

    ControlFileData *controlData = (ControlFileData *)bufPtr(controlFile);

    FUNCTION_TEST_RETURN(
        controlData->pg_control_version == PG_CONTROL_VERSION && controlData->catalog_version_no == CATALOG_VERSION_NO);
}

/**********************************************************************************************************************************/
PgControl
pgInterfaceControl084(const Buffer *controlFile)
{
    FUNCTION_LOG_BEGIN(logLevelTrace);
        FUNCTION_LOG_PARAM(BUFFER, controlFile);
    FUNCTION_LOG_END();

    ASSERT(controlFile != NULL);
    ASSERT(pgInterfaceControlIs084(controlFile));

    PgControl result = {0};
    ControlFileData *controlData = (ControlFileData *)bufPtr(controlFile);

    result.systemId = controlData->system_identifier;
    result.controlVersion = controlData->pg_control_version;
    result.catalogVersion = controlData->catalog_version_no;

    result.pageSize = controlData->blcksz;
    result.walSegmentSize = controlData->xlog_seg_size;

    FUNCTION_LOG_RETURN(PG_CONTROL, result);
}

/**********************************************************************************************************************************/
bool
pgInterfaceWalIs084(const Buffer *walFile)
{
    FUNCTION_TEST_BEGIN();
        FUNCTION_TEST_PARAM(BUFFER, controlFile);
    FUNCTION_TEST_END();

    ASSERT(walFile != NULL);

    FUNCTION_TEST_RETURN(((XLogPageHeaderData *)bufPtr(walFile))->xlp_magic == XLOG_PAGE_MAGIC);
}

/**********************************************************************************************************************************/
PgWal
pgInterfaceWal084(const Buffer *walFile)
{
    FUNCTION_LOG_BEGIN(logLevelTrace);
        FUNCTION_LOG_PARAM(BUFFER, walFile);
    FUNCTION_LOG_END();

    ASSERT(walFile != NULL);
    ASSERT(pgInterfaceWalIs084(walFile));

    PgWal result = {0};

    result.systemId = ((XLogLongPageHeaderData *)bufPtr(walFile))->xlp_sysid;

    FUNCTION_LOG_RETURN(PG_WAL, result);
}

#ifdef DEBUG

/**********************************************************************************************************************************/
void
pgInterfaceControlTest084(PgControl pgControl, Buffer *buffer)
{
    FUNCTION_TEST_BEGIN();
        FUNCTION_TEST_PARAM(PG_CONTROL, pgControl);
    FUNCTION_TEST_END();

    ControlFileData *controlData = (ControlFileData *)bufPtr(buffer);

    controlData->system_identifier = pgControl.systemId;
    controlData->pg_control_version = pgControl.controlVersion == 0 ? PG_CONTROL_VERSION : pgControl.controlVersion;
    controlData->catalog_version_no = pgControl.catalogVersion == 0 ? CATALOG_VERSION_NO : pgControl.catalogVersion;

    controlData->blcksz = pgControl.pageSize;
    controlData->xlog_seg_size = pgControl.walSegmentSize;

    FUNCTION_TEST_RETURN_VOID();
}

/**********************************************************************************************************************************/
void
pgInterfaceWalTest084(PgWal pgWal, Buffer *buffer)
{
    FUNCTION_TEST_BEGIN();
        FUNCTION_TEST_PARAM(PG_WAL, pgWal);
    FUNCTION_TEST_END();

    XLogLongPageHeaderData *walData = (XLogLongPageHeaderData *)bufPtr(buffer);

    walData->std.xlp_magic = XLOG_PAGE_MAGIC;
    walData->xlp_sysid = pgWal.systemId;

    FUNCTION_TEST_RETURN_VOID();
}

#endif
