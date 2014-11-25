typedef struct _node *node;

struct _node {
  int val;
  /*@null@*/ node next;
}

void node_free1 (/*@only@*/ node n)
{
  free (n); /* error: must free n->next */
}

void node_free2 (/*@only@*/ node n)
{
  node nn = n->next;
  free (n); /* okay */
} /* error: must free nn */
  
void node_free3 (/*@only@*/ node n)
{
  node nn = n->next;
  free (n); /* okay */
  node_free1 (nn); /* possibly null error */
} /* okay */
  
