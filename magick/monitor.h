/*
  Monitor define declarations.
*/
#define QuantumTick(i,span) \
  (((~((span)-i-1) & ((span)-i-2))+1) == ((span)-i-1))

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
