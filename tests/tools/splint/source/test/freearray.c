typedef /*@only@*/ char *ocp;

static /*@only@*/ ocp *stringTable;
static int stringTableSize;

void destroyMod (void)
     /*@globals killed stringTable@*/
{
  int i;

  for (i = 0; i < stringTableSize; i++)
    {
      free (stringTable[i]);
    }

  free (stringTable); /* okay */
}

void destroyMod2 (void)
     /*@globals killed stringTable@*/
{
  free (stringTable); /* error */
}
