typedef struct _node *node;

typedef struct {
  char *val;
} *item;

struct _node {
  /*@dependent@*/ item val;
  /*@null@*/ node next;
}

void node_free1 (/*@only@*/ node n)
{
  free (n); /* errors: must free n->next */
}

void node_free2 (/*@only@*/ node n)
{
  node nn = n->next;
  free (n); /* okay */
} /* error - nn not released */
  
void node_free3 (/*@only@*/ node n)
{
  node nn = n->next;
  free (n->val); /* error: free dependent */
  free (n); /* okay */
  if (nn != NULL) {
    node_free1 (nn); /* okay (error for null) */
  }
} /* okay */
  
