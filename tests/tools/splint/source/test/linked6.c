typedef struct _node *node;

typedef struct {
  char *val;
} *item;

struct _node {
  /*@only@*/ item val;
  /*@null@*/ node next;
}

extern void keeper (/*@keep@*/ item iv);

void node_free1 (/*@only@*/ node n)
{
  free (n); /* 2 errors: must free n->next, n->val */
}

void node_free2 (/*@only@*/ node n)
{
  node nn = n->next;
  keeper (n->val);
  free (n); /* okay */
} /* error - nn not released */
  
void node_free3 (/*@only@*/ node n)
{
  node nn = n->next;
  keeper (n->val);
  free (n); /* okay */
  if (nn != NULL) {
    node_free1 (nn); /* okay (error for null) */
  }
} /* okay */
  
