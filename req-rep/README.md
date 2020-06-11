# REQ-REP
Ultra minimalistic request-reply example

Due to the blocking nature of REQ and REP the main function moves
both classes to worker threads.
This is for convenience only and should **not** be ported to
production environments.
