/*
  Postscript coder common definitions
*/

#if defined(HasGS)
#include "ps/iapi.h"
#include "ps/errors.h"
#endif

/*
  We don't want to depend on Ghostscript's iapi.h under Windows so
  equivalent function vectors are defined here.
*/

#ifndef gs_main_instance_DEFINED
# define gs_main_instance_DEFINED
typedef struct gs_main_instance_s gs_main_instance;
#endif

#if !defined(MagickDLLCall)
#  if defined(WIN32)
#    define MagickDLLCall __stdcall
#  else
#    define MagickDLLCall
#  endif
#endif

typedef struct _GhostscriptVectors
{
  int
    (MagickDLLCall *exit)(gs_main_instance *),
    (MagickDLLCall *init_with_args)(gs_main_instance *,int,char **),
    (MagickDLLCall *new_instance)(gs_main_instance **,void *),
    (MagickDLLCall *run_string)(gs_main_instance *,const char *,int,int *);

  void
    (MagickDLLCall *delete_instance)(gs_main_instance *);
} GhostscriptVectors;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x e c u t e P o s t I n t e r p r e t e r                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ExecutePostscriptInterpreter executes the postscript interpreter
%  with the specified command.
%
%  The format of the ExecutePostscriptInterpreter method is:
%
%      unsigned int ExecutePostscriptInterpreter(const unsigned int verbose,
%        const char *command)
%
%  A description of each parameter follows:
%
%    o status:  Method ExecutePostscriptInterpreter returns True is the command
%      is successfully executed, otherwise False.
%
%    o verbose: A value other than zero displays the command prior to
%      executing it.
%
%    o command: The address of a character string containing the command to
%      execute.
%
%
*/
static unsigned int ExecutePostscriptInterpreter(const unsigned int verbose,
  const char *command)
{
#if defined(HasGS) || defined(WIN32)
  char
    **argv;

  gs_main_instance
    *interpreter;

  int
    argc,
    code,
    status;

  register long
    i;

#if defined(HasGS)
  GhostscriptVectors
    gs_func_struct;

  const GhostscriptVectors
    *gs_func;

  gs_func=(&gs_func_struct);
  gs_func_struct.exit=gsapi_exit;
  gs_func_struct.init_with_args=gsapi_init_with_args;
  gs_func_struct.new_instance=gsapi_new_instance;
  gs_func_struct.run_string=gsapi_run_string;
  gs_func_struct.delete_instance=gsapi_delete_instance;
#elif defined(WIN32)
  const GhostscriptVectors
    *gs_func;

  gs_func=NTGhostscriptDLLVectors();
#endif
  if (gs_func == (GhostscriptVectors*) NULL)
    {
      if (verbose)
        (void) fputs(command,stdout);
      return(SystemCommand(verbose,command));
    }
  if (verbose)
    {
      (void) fputs("gsdll32",stdout);
      (void) fputs(strchr(command,' '),stdout);
    }
  status=(gs_func->new_instance)(&interpreter,(void *) NULL);
  if (status < 0)
    return(False);
  argv=StringToArgv(command,&argc);
  status=(gs_func->init_with_args)(interpreter,argc-1,argv+1);
  if (status == 0)
    status=(gs_func->run_string)(interpreter,
      "systemdict /start get exec\n",0,&code);
  (gs_func->exit)(interpreter);
  (gs_func->delete_instance)(interpreter);
  for (i=0; i < argc; i++)
    LiberateMemory((void **) &argv[i]);
  LiberateMemory((void **) &argv);
  if ((status == 0) || (status == -101))
    return(False);
  return(True);
#else
  return(SystemCommand(verbose,command));
#endif
}
