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

A demonstration bundle of the Core Flight System including the cFE, OSAL, and PSP can be obtained at https://github.com/nasa/cfs

For information about a mission ready cFS bundle, see: https://github.com/nasa/cFS#cfs-gov-mission-ready-version

## Known issues

See all [open issues](https://github.com/nasa/MD/issues) and closed to milestones later than this version.

## Getting Help

For best results, submit issues:questions or issues:help wanted requests at <https://github.com/nasa/cFS>.

Official cFS page: <http://cfs.gsfc.nasa.gov>
