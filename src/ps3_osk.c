#define _ps3_osk_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#include <sysutil/osk.h>
#include <sysutil/sysutil.h>
#include <sys/memory.h>
#include <sys/sem.h>

static sys_sem_t oskSemaphore;
static sys_mem_container_t oskContainer;

static void oskEventHandler(u64 status,u64 param,void *usrdata)
{
  switch(status) {
  case SYSUTIL_OSK_DONE:
    sysSemPost(oskSemaphore,1);
    break;
  }
}

static void init_osk(void) __attribute__((constructor));
static void init_osk(void)
{
  static const sys_sem_attr_t attr = {
    SYS_SEM_ATTR_PROTOCOL,
    SYS_SEM_ATTR_PSHARED,
  };
  sysSemCreate(&oskSemaphore,&attr,0,1);
  sysMemContainerCreate(&oskContainer, 0x500000);
  sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT1, oskEventHandler, 0);
}

static void term_osk(void) __attribute__((destructor));
static void term_osk(void)
{
  oskAbort();
  sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT1);
  sysMemContainerDestroy(oskContainer);
  sysSemDestroy(oskSemaphore);
}

int GetStringPS3OSK (char *buf, int MaxLen)
{
  s32 r;
  static const oskParam param = {
    OSK_PANEL_TYPE_LATIN|OSK_PANEL_TYPE_NUMERAL,
    OSK_PANEL_TYPE_LATIN,
    { 0.0, 0.0 },
    OSK_PROHIBIT_RETURN
  };
  u16 msg[] = { 0 }, startText[] = { 0 };
  u16 wbuf[MaxLen+1];
  const oskInputFieldInfo ifi = {msg, startText, MaxLen};
  oskCallbackReturnParam rparam = {0, MaxLen, wbuf};
  int i;

  buf[0] = 0;

  sysSemTryWait(oskSemaphore);

  oskSetLayoutMode(OSK_LAYOUTMODE_HORIZONTAL_ALIGN_CENTER |
		   OSK_LAYOUTMODE_VERTICAL_ALIGN_TOP);
  oskSetKeyLayoutOption(OSK_10KEY_PANEL | OSK_FULLKEY_PANEL);
  oskSetInitialKeyLayout(OSK_INITIAL_SYSTEM_PANEL);

  r = oskLoadAsync(oskContainer, &param, &ifi);
  if (r)
    return -1;

  while (sysSemTryWait(oskSemaphore)) {
    SDL_Flip (ne_screen);
    SDL_PumpEvents ();
  }

  r = oskUnloadAsync(&rparam);
  if (r)
    return -1;

  for(i=0; i<MaxLen; i++)
    if(!(buf[i] = (char)wbuf[i]))
      break;

  return (rparam.res == OSK_OK);
}
