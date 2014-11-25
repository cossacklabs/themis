void test1(int value, int *data, int *result)
{
int i;
int j;

	*result = 0;
	switch(value)
	{
	case 1:
		for(i=1;i<100;i++)
		{
			for(j=0;j<100;j++)
			{
				if(data[j] == i)
					goto RETURN;
			}
		}
		*result = 1;
		break;
	case 3:
		for(i=1;i<100;i++)
		{
			for(j=0;j<100;j++)
			{
				if(data[j] == 2*i+1)
					goto RETURN;
			}
		}
		*result = 2;
		break;
	default:
		*result = -1;
		break;
	}
RETURN:
//	*result = 0;	// if this line is not here, lclint fails
}
