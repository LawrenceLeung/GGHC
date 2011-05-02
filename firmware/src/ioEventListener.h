#ifndef __INCLUDED_IOEVENT_LISTENER_H
#define __INCLUDED_IOEVENT_LISTENER_H

#include "qp_port.h"

extern QActive * const AO_IOEventListener;     // opaque pointer to main active object

extern void IOListener_ctor(void);

#endif
