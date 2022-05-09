# core Flight System (cFS) Memory Dwell Application (MD)

## Introduction

The Memory Dwell application (MD) is a core Flight System (cFS) application
that is a plug in to the Core Flight Executive (cFE) component of the cFS.

The MD application monitors memory addresses accessed by the CPU. This task
is used for both debugging and monitoring unanticipated telemetry that had
not been previously defined in the system prior to deployment.

The MD application is written in C and depends on the cFS Operating System
Abstraction Layer (OSAL) and cFE components. 

User's guide information can be generated using Doxygen (from top mission directory):
```
  make prep
  make -C build/docs/md-usersguide md-usersguide
```

## Software Required

cFS Framework (cFE, OSAL, PSP)

An integrated bundle including the cFE, OSAL, and PSP can
be obtained at https://github.com/nasa/cfs

## About cFS

The cFS is a platform and project independent reusable software framework and
set of reusable applications developed by NASA Goddard Space Flight Center.
This framework is used as the basis for the flight software for satellite data
systems and instruments, but can be used on other embedded systems.  More
information on the cFS can be found at http://cfs.gsfc.nasa.gov
