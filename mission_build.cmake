###########################################################
#
# MD App mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# Add stand alone documentation
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/docs/dox_src ${MISSION_BINARY_DIR}/docs/md-usersguide)

# The list of header files that control the MD configuration
set(MD_MISSION_CONFIG_FILE_LIST
  md_fcncode_values.h
  md_interface_cfg_values.h
  md_mission_cfg.h
  md_perfids.h
  md_msg.h
  md_msgdefs.h
  md_msgstruct.h
  md_tbl.h
  md_tbldefs.h
  md_tblstruct.h
  md_topicid_values.h
  md_extern_typedefs.h
)

generate_configfile_set(${MD_MISSION_CONFIG_FILE_LIST})

# App specific mission scope configuration
