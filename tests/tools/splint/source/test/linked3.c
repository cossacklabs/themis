typedef struct _node *node;

struct _node {
  char *val;
  /*@null@*/ node next;
}

void node_free1 (/*@only@*/ node n)
{
  free (n); /* 2 errors: must free n->next, n->val */
}

void node_free2 (/*@only@*/ node n)
{
  node nn = n->next;
  free (n); /* error: must free n->val */
} /* error - nn not released */
  
void node_free3 (/*@only@*/ node n)
{
  node nn = n->next;
  free (n->val);
  free (n); /* okay */
  if (nn != NULL) {
    node_free1 (nn); /* okay (error for null) */
  }
} /* okay */
  
