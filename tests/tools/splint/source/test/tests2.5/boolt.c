typedef int pan_bool_t;

#define PAN_FALSE (0)
#define PAN_TRUE ((pan_bool_t) 1)

pan_bool_t test (pan_bool_t b) {
  (void) test (2);
  (void) test (PAN_FALSE);
  (void) test (PAN_TRUE);
  (void) test (b);
  return PAN_TRUE;
}
