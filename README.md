# Memory Dwell
NASA core Flight System Memory Dwell Application

## Description
The Memory Dwell application (MD) is a core Flight System (cFS) application that is a plug in to the Core Flight Executive (cFE) component of the cFS.

The cFS is a platform and project independent reusable software framework and set of reusable applications developed by NASA Goddard Space Flight Center. This framework is used as the basis for the flight software for satellite data systems and instruments, but can be used on other embedded systems. More information on the cFS can be found at [http://cfs.gsfc.nasa.gov](http://cfs.gsfc.nasa.gov)

The MD application monitors memory addresses accessed by the CPU. This task is used for both debugging and monitoring unanticipated telemetry that had not been previously defined in the system prior to deployment.

MD requires use of the [cFS application library](https://github.com/nasa/cfs_lib).
