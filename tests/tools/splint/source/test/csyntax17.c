typedef char SBYTE;
typedef SBYTE EID_UNIT_NUM;

struct qry_rsp   
{
  int rsp[(EID_UNIT_NUM)16];
};

int f (void)
{
  int x = (EID_UNIT_NUM) 3;
}

