project(CFS_MD C)

include_directories(fsw/src)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)

set(APP_SRC_FILES
  fsw/src/md_dwell_tbl.c
  fsw/src/md_utils.c
  fsw/src/md_dwell_pkt.c
  fsw/src/md_cmds.c
  fsw/src/md_app.c
)

# Create the app module
add_cfe_app(md ${APP_SRC_FILES})

set(APP_TABLE_FILES
  fsw/tables/md_dw02.c
  fsw/tables/md_dw03.c
  fsw/tables/md_dw04.c
  fsw/tables/md_dw01.c
)

# Add tables to build
add_cfe_tables(md ${APP_TABLE_FILES})

# If UT is enabled, then add the tests from the subdirectory
# Note that this is an app, and therefore does not provide
# stub functions, as other entities would not typically make
# direct function calls into this application.
if(ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif()
