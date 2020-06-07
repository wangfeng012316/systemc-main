//FILE basic_main.cpp (systemc)
// $Info: Entry point for executing simulation for 'basic'.$
// Copyright 2018 Doulos Inc. All rights reserved.
// Licensed under Apache 2.0 - see accompanying LICENSE FILE.
//-----------------------------------------------------------------------------
#include "basic_top.hpp"
#include "wallclock.hpp"
#if __cplusplus >= 201103L
#  include <memory>
#endif
#include "report.hpp"

namespace {
  const char* MSGID = "/Doulos Inc/SystemC-Example/main";
  double elaboration_cpuTime=-1.0, starting_cpuTime=-1.0, finished_cpuTime=-1.0;
  int summary(void);
}

using namespace sc_core;
using namespace std;

// We don't always use argc and argv, but this is the only allowed
// syntax for sc_main.  Moreover, if we desire to use argc and argv
// elsewhere, we simply use sc_core::sc_argc() and sc_core::sc_argv(),
// which have been captured prior to calling here.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int sc_main(int argc, char *argv[]) //< main entry-point to SystemC
{
#pragma GCC diagnostic pop

  // Pointer to top-level module
  #if __cplusplus < 201103L
  Basic_top* basic_top;
  #else
  unique_ptr<Basic_top> basic_top;
  #endif

  // Elaborate
  elaboration_cpuTime = get_cpu_time(); //< Capture start of elaboration
  try {
    // Construct top-level
    #if __cplusplus < 201103L
    basic_top = new Basic_top("basic_top");
    #else
    basic_top.reset(new Basic_top("basic_top"));
    #endif
  } catch (sc_core::sc_exception& e) {
    REPORT(INFO,"\n" << e.what() << "\n\n*** Please fix elaboration errors and retry. ***");
    return summary();
  } catch (...) {
    REPORT(INFO,"\n Error: *** Caught unknown exception during elaboration. ***");
    return summary();
  }//endtry

  // Simulate
  sc_core::sc_time finished_simTime;
  try {
    REPORT(INFO,"Starting kernel");
    starting_cpuTime = get_cpu_time(); //< Capture start of simulation
    sc_core::sc_start();
    finished_simTime = sc_core::sc_time_stamp();
    finished_cpuTime = get_cpu_time();
  } catch (sc_core::sc_exception& e) {
    REPORT(WARNING,"\n\nCaught exception during active simulation.\n" << e.what());
  } catch (...) {
    REPORT(WARNING,"Error: Caught unknown exception during active simulation.");
  }//endtry
  REPORT(INFO,"Exited kernel at " << finished_simTime);

  // Clean up
  if (! sc_core::sc_end_of_simulation_invoked()) {
    try {
      REPORT(INFO,"ERROR: Simulation stopped without explicit sc_stop()");
    } catch(sc_core::sc_exception& e) { REPORT(INFO,"\n\n" << e.what()); }
    sc_core::sc_stop(); //< this will invoke end_of_simulation() callbacks
  }//endif

  return summary();

}

//-----------------------------------------------------------------------------
namespace {
  // Summarize results
  int summary(void)
  {
    string kind = "Simulation";
    if ( starting_cpuTime < 0.0 ) {
      kind = "Elaboration";
      starting_cpuTime = finished_cpuTime = get_cpu_time();
    } 
    if ( finished_cpuTime < 0.0 ) {
      finished_cpuTime = get_cpu_time();
    }
    auto errors = sc_report_handler::get_count(SC_ERROR)
                + sc_report_handler::get_count(SC_FATAL);
    REPORT(INFO, "\n" << string(80,'#') << "\nSummary for " << sc_argv()[0] << ":\n  "
      << "CPU elaboration time " << setprecision(4) << (starting_cpuTime - elaboration_cpuTime) << " sec\n  "
      << "CPU simulation  time " << setprecision(4) << (finished_cpuTime - starting_cpuTime) << " sec\n  "
      << setw(2) << sc_report_handler::get_count(SC_INFO)    << " informational messages" << "\n  "
      << setw(2) << sc_report_handler::get_count(SC_WARNING) << " warnings" << "\n  "
      << setw(2) << sc_report_handler::get_count(SC_ERROR)   << " errors"   << "\n  "
      << setw(2) << sc_report_handler::get_count(SC_FATAL)   << " fatals"   << "\n\n"
      << kind << " " << (errors?"FAILED":"PASSED")
    );
    return (errors?1:0);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Copyright 2018 Doulos Inc. All rights reserved.
// Licensed under Apache 2.0 - see accompanying LICENSE FILE.
// -*- C++ -*- vim:sw=2:tw=0:fdm=marker:fmr=<<<,>>>
//END main.cpp $Id$ >>>}
