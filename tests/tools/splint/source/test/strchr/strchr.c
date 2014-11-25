
void func () {
  char *c;
  c = strchr("dfdfedfd", 'e');
  *c = 'd'; /* modification of observer, possibly null */
}

void fixline (char * line)
{
  char *p;
 
  if ( (p = strchr(line, '\n')) ) 
  {
    *p = '\0';
  }
  
}
