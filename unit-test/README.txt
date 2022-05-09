##############################################################################
## File: README.txt
##
## Purpose: CFS MD application unit test instructions, results, and coverage
##############################################################################

-------------------------
MD Unit Test Instructions
-------------------------
This unit test was run in a virtual machine running Ubuntu 18.04 and uses the
ut-assert stubs and default hooks for the cFE, OSAL and PSP.

To run the unit test enter the following commands at the command line prompt in
the top-level cFS directory (after ensuring that MD is listed as a target). Note
that in order to successfully compile the unit tests the "-Werror" compilation
flag must be disabled.

make distclean
make SIMULATION=native ENABLE_UNIT_TESTS=true prep
make
make test
make lcov

MD 2.4.0 Unit Test Results:

Tests Executed:    136
Assert Pass Count: 738
Assert Fail Count: 0

==========================================================================
md_app.c - Line Coverage:      96.2%
           Function Coverage: 100.0%
           Branch Coverage:    95.9%

Line and branch coverage gaps in this file have 3 causes:
- MD_InitTableServices has uncovered lines and branches caused by a current
inability to force the return value of snprintf.
- MD_ExecRequest has an unreachable condition.  The "default"
case for the switch statement is required by coding standard but is
technically unreachable because it would be caught by the earlier call to
MD_SearchCmdHndlrTbl.
- MD_SearchCmdHndlrTbl has an unreachable condition which cannot be
tested without a change to the command handler table.

==========================================================================
md_cmds.c - Line Coverage:     100.0%
            Function Coverage: 100.0%
            Branch Coverage:   100.0%

==========================================================================
md_dwell_pkt.c - Line Coverage:     100.0%
                 Function Coverage: 100.0%
                 Branch Coverage:   100.0%

==========================================================================
md_dwell_tbl.c - Line Coverage:      99.4%
                 Function Coverage: 100.0%
                 Branch Coverage:    98.5%

Line and branch coverage gaps are caused by a single unreachable condition
in MD_TableValidationFunc. An "else" clause is required by coding standard
but is not technically reachable because all possible return codes from
MD_CheckTableEntries are explicitly covered.

==========================================================================
md_utils.c - Line Coverage:     100.0%
             Function Coverage: 100.0%
             Branch Coverage:   100.0%

==========================================================================
