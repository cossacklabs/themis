# include "global.h"

void pmsg (char *msg)
{
  fprintf (g_msgstream, "%s\n", msg);
}
