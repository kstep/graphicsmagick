/*
  Monitor define declarations.
*/
#define QuantumTick(i,span) \
  ((((~(i) & (i-1))+1) == (i)) || (i == (span-1)))

/*
  Monitor typedef declarations.
*/
typedef void
  (*MonitorHandler)(char *,const unsigned int,const unsigned int);

/*
  Monitor declarations.
*/
extern Export MonitorHandler
  SetMonitorHandler(MonitorHandler);

extern Export void
  ProgressMonitor(char *,const unsigned int,const unsigned int);
