typedef struct _node *node;

struct _node {
  int val;
  /*@dependent@*/ /*@null@*/ node next;
}

void node_free1 (/*@only@*/ node n)
{
  free (n); /* error: must free n->next */
}

void node_free2 (/*@only@*/ node n)
{
  node nn = n->next;
  free (n); /* okay */
} /* okay - no need to free nn since it is dependent */
  
void node_free3 (/*@only@*/ node n)
{
  node nn = n->next;
  free (n); /* okay */
  node_free1 (nn); /* error: dependent at only */
} /* okay */
  
