/***************************************************************************
                                     Timeclock
                 A small program to collect timecard information
                            -------------------
    begin                : Tue Feb 22 09:41:09 CST 2000
    copyright            : (C) 2000 by Stephen Toothman
    email                : stephen.toothman@cicplan.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This is a complete GPL time clock system for small business.  It will *
 *   have a time clock, a reporting system, an user management system, and *
 *   an administrative system for setting system options.  The timeclock   *
 *   section will rely on several external files when it is finished.      *
 *   These files will be an administration file that holds the basic       *
 *   company information, such as when the pay week begins and when the    *
 *   pay week ends and in future versions may allow different payroll      *
 *   systems such as bi-monthly and monthly.  The second file will hold a  *
 *   list of user ids and a flag for the privileges allowed individual     *
 *   user.  The last file will be the timecard file that holds the         *
 *   individual timecard records.                                          *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include "timeclock.h" */

	

int main(int argc, char *argv[])
{
	int i, j;
	char userid[4] = "zzz";     /* the user id or employee number */
	int usertype = 0;      /* the user type used to determine access */
	int weekends = 0;
	
	i = initializeprogram(&userid[0], &usertype, &weekends);
	j = startprogram(&userid[0], &usertype, &weekends);
	return EXIT_SUCCESS;
}

int initializeprogram(char *puserid1, int *pusertype1, int *pweekends1)
{
	int m, n, loop;
	char tempuser[4];
	int tempusertype, tempweekends;
	
	m = getenvironment(&tempweekends);
	n = getuserid(&tempuser[0], &tempusertype);
  for( loop = 0; loop < 4; loop++)
  {
 		*puserid1 = toupper((unsigned char) tempuser[loop]);
		puserid1 = puserid1 + 1;
  }
	*pusertype1 = tempusertype;
	*pweekends1 = tempweekends;
	return n;	
}

int getenvironment(int *pweekends2)
{
 	FILE *fp;
	char fieldname[80];
	int testvalue;
	fp = fopen("admin.txt", "r");
	while ( !feof(fp))
	{
		fscanf(fp, "%s %02d\n", fieldname, &testvalue);
		if (strcmp(fieldname, "WEEKENDS") == 0)
		{
			*pweekends2 = testvalue;
		}
	}
	fclose(fp);
	return 0;
}
int getuserid(char *puserid2, int *pusertype2)
{
	char *z;
	char name[80];
  int w = 0, x = 0, y, loop;
 	FILE *fp;
	char testuserid[4];
	int testrights;
	
	while (x == 0)
	{
  	printf("Enter your user id\n");
	  z = fgets(name, 80, stdin);
	  y = (z == NULL ? 0 : 1);
	  for( loop = 0; loop < 4; loop++)
	  {
	  	if (loop == 3 )
	  	{
	  		*puserid2 = '\0';
	  	}
	  	else
	  	{
	  		*puserid2 = toupper((unsigned char) name[loop]);
  			puserid2 = puserid2 + 1;
	  	}
	  }
	  puserid2 = puserid2 - loop + 1;
		fp = fopen("userlist.txt", "r");
		while ( !feof(fp) && x == 0)
		{
			fscanf(fp, "%s %02d\n", testuserid, &testrights);
			if (strcmp(puserid2, testuserid) == 0)
			{
				x = 1;
				*pusertype2 = testrights;
			}
		}
		fclose(fp);
		if ( x == 0 && w >= 1)
		{
			printf("You have entered an incorrect user id\n");
			printf("Please contact your supervisor for assistance.\n");
			printf("Press <ENTER> to exit\n")	;
			getchar();
			exit(1);
		}
		if (x == 0 && w < 1)
		{
			printf("You have entered an incorrect user id\n");
			printf("Please enter your information again.\n");
			w += 1;
		}
	}
	return y;
}

int startprogram(char *puserid, int *pusertype, int *pweekends)
{
	int m, n;
	char y[80];
	printf("Please enter what action you wish to take %s -- %d -- %d\n", puserid, *pusertype, *pweekends);
	while (m < 1 || m > 4)
	{
		switch (*pusertype)
		{
			case 1 :
				printf(" 1     Use Timeclock.\n");
		    break;
			case 2 :
				printf(" 1     Use Timeclock.\n");
				printf(" 2     Use Manager functions.\n");
		    break;
		  case 3 :
				printf(" 1     Use Timeclock.\n");
				printf(" 2     Use Manager functions.\n");
				printf(" 3     Use Reporting functions.\n");
				break;
		  case 4 :
				printf(" 1     Use Timeclock.\n");
				printf(" 2     Use Manager functions.\n");
				printf(" 3     Use Reporting functions.\n");
				printf(" 4     Use Administrative functions.\n");
	      break;
	    default :
				printf(" 1     Use Timeclock.\n");
		    break;
		}
		printf("\nEnter an item from the list ===> ");
		fgets(y, 80, stdin);
		m = atoi(y);
		/* this if statement checks that an item on the menu was entered */
		if (m < 1 || m > 4)
		{
			printf("\n");
			printf("The number you entered is not valid.\n");
			printf("\n");
			printf("\n");
			printf("the number must be from the list below. \n");
			
			m = 0;
		}
		if ( m != 0)
		{
			switch (*pusertype)
			{
				case 1 :
					if (m != 1)
					{
						m = 99;
					}
					break;
				case 2 :
					if (m != 1 && m != 2)
					{
						m = 99;
					}
			    break;
			  case 3 :
					if (m != 1 && m != 2 && m != 3)
					{
						m = 99;
					}
			    break;
			  case 4 :
					if (m != 1 && m != 2 && m != 3 && m != 4)
					{
						m = 99;
					}
					break;
				default :
					if (m != 1)
					{
						m = 99;
					}
					break;
			}
		}
		if ( m == 99 )
		{
			printf("\n");
			printf("The number you entered is not valid.\n");
			printf("That number is not an allowed choice.\n");
			printf("\n");
			printf("The number must be from the list below. \n");
		
			m = 0;
		}
	}
	switch (m)
	{
		case 1 :
	  {
	    n = timecard(puserid, *pusertype, *pweekends);
	    break;
	  }
		case 2 :
	  {
	    n = manager(puserid, *pusertype, *pweekends);
	    break;
	  }
	  case 3 :
	  {
	    n = reporter(puserid, *pusertype, *pweekends);
			break;
	  }
	  case 4 :
	  {
	    n = administrator(puserid, *pusertype, *pweekends);
      break;
	  }
		default :
	  {
	    n = timecard(puserid, *pusertype, *pweekends);
	    break;
	  }
	}
	return 0;
}

int timecard(char userid[4], int usertype, int weekends)
{
	int j, k, l, n, o, p, loop;  /* return and miscellaneous variables */
	
	timecardrecord currentpunch;
		
	int itemflag[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ;   /* A flag array to hold whether a time card item has been used */
	timecardrecord clockpunch[14];  /* an array of structures to hold previous time clock entries */
	
	char weekstring[3]; /* filename portions */
	char yearstring[5];
	char filename[13];

		
	/* The process works as follows:                                           *
	 * First the user id is obtained from the user                             *
	 * Next the program gets the current time and gets the filename components *
	 * Next the program generates a filename                                   *
	 * Next the program gets an action, clock in or clock out, from the user   *
	 * Finally the timecard record is outputed to a file                       */
	
  for( loop = 0; loop < 4; loop++)
  {
 		currentpunch.userid[loop] = userid[loop];
  }
	j = gettimeanddate(&currentpunch, &weekstring[0], &yearstring[0], weekends);
	k = getfilename(&filename[0], &weekstring[0], &yearstring[0]);
	o = gettimecarddata(&currentpunch, &clockpunch[0], &itemflag[0], &filename[0]);
	p = printtimecarddata(&currentpunch, &clockpunch[0]);
	l = getaction(&currentpunch, &clockpunch[0], &itemflag[0]);
	n = writerecord(&currentpunch, &filename[0]);
	return n;
}

int gettimeanddate(timecardrecord *currentpunch, char *pweekstring, char *pyearstring, int weekends)
{
  time_t now, test;
  struct tm *timenow, *timetest, *timetemp;

  int z, adjustfactor;
  int weekdaynow;
  int yeartest;
	char weekstringnow[3], weekstringtest[3];
	char yearstringnow[5], yearstringtest[5];
	
  /* get the current time information */
  now = time(0);
	timenow = localtime(&now);
	/* assigns the time information */
	currentpunch->hour = timenow->tm_hour;
	currentpunch->minute = timenow->tm_min;
	currentpunch->second = timenow->tm_sec;
	currentpunch->day = timenow->tm_mday;
	currentpunch->month = timenow->tm_mon + 1;
	currentpunch->year = timenow->tm_year + 1900;
	weekdaynow = timenow->tm_wday;
	strftime(weekstringnow, 3, "%U", timenow);
	strftime(yearstringnow, 5, "%Y", timenow);
	
	/* Find the adjustment needed to be added to the current time
	 * to return the last day of the pay period */
	adjustfactor = ((weekends <= weekdaynow) ? 6 - weekdaynow + weekends : weekends - weekdaynow - 1);
	
	/* get the time information of the last day of the payperiod */
	timetemp = timenow;
	timetemp->tm_mday = timetemp->tm_mday + adjustfactor;
	test = mktime(timetemp);
	timetest = localtime(&test);
	yeartest = timetest->tm_year + 1900;
	strftime(weekstringtest, 3, "%U", timetest);
	strftime(yearstringtest, 5, "%Y", timetest);

	/* puts the correct information into the file name components */
  strcpy(pweekstring, weekstringtest);
  if (currentpunch->year == yeartest)
	{
		strcpy(pyearstring, yearstringnow);
	}
	else
	{
		strcpy(pyearstring, yearstringtest);
	}

	z = 1;
	return z;
}

int getfilename(char *pfilename, char *pweekstring, char *pyearstring)
{
	int z;
	
	/* this code generates the complete file name */
	sprintf (pfilename, "tc%s%s.tcd", pyearstring, pweekstring);
	
	z = 1;
	return z;
}

int gettimecarddata(timecardrecord *currentpunch, timecardrecord *clockpunch, int *pitemflag, char *pfilename)
{
 	int a, b, c, d, w = 0, z, y;
	FILE *fp;
	char testuserid[4];
	int testaction, testyear, testmonth, testday, testhour, testminute, testsecond;
	y = 1;
	while (w < 14)
	{
		strcpy(clockpunch[w].userid, "   ");
		clockpunch[w].action = 0;
		clockpunch[w].month = 0;
		clockpunch[w].day = 0;
		clockpunch[w].year = 0;
		clockpunch[w].hour = 0;
		clockpunch[w].minute = 0;
		clockpunch[w].second = 0;
		w += 1;
	}
	
	if ((fp = fopen(pfilename, "r")) != NULL)
	{
		while ( !feof(fp))
		{
			fscanf(fp, "%s %02d%02d%02d%04d%02d%02d%02d\n", testuserid, &testaction, &testmonth, &testday, &testyear, &testhour, &testminute, &testsecond);
			a = strcmp(currentpunch->userid, testuserid);
			b = (currentpunch->month == testmonth ? 0 : 1);
			c = (currentpunch->day == testday ? 0 : 1);
			d = (currentpunch->year == testyear ? 0 : 1);
			y = ((a == 0 && b == 0 && c == 0 && d == 0) ? 0 : 1);
			if (y == 0)
			{
				z = testaction - 1;
				strcpy(clockpunch[z].userid, testuserid);
				clockpunch[z].action = testaction;
				clockpunch[z].month = testmonth;
				clockpunch[z].day = testday;
				clockpunch[z].year = testyear;
				clockpunch[z].hour = testhour;
				clockpunch[z].minute = testminute;
				clockpunch[z].second = testsecond;
				pitemflag[z] = 1;
			}
		}
		fclose(fp);
	}
	return 0;
}

int printtimecarddata(timecardrecord *currentpunch, timecardrecord *clockpunch)
{
	int y = 0, z;
	
	clearscreen();
	printf("\n\n%s-  -%02d-%02d-%04d-%02d-%02d-%02d\n\n", currentpunch->userid, currentpunch->month, currentpunch->day, currentpunch->year, currentpunch->hour, currentpunch->minute, currentpunch->second);
	
	while (y < 14)
	{
		z = strcmp(clockpunch[y].userid, currentpunch->userid);
		if (0 == z)
		{
			printf("%s-%02d-%02d-%02d-%04d-%02d-%02d-%02d\n", clockpunch[y].userid, clockpunch[y].action, clockpunch[y].month, clockpunch[y].day, clockpunch[y].year, clockpunch[y].hour, clockpunch[y].minute, clockpunch[y].second);
		}
		y += 1;
	}
	printf("\n");
	return 0;
}

int getaction(timecardrecord *currentpunch, timecardrecord *clockpunch, int *pitemflag)
{
	int u, v = 0, w = 0, x = 0, z;
	int i = 0;
	char y[80];
	/* This is the action menu */
	if (pitemflag[0] == 1)
	{
		while (w < 14)
		{
			u = (clockpunch[w].hour*10000)+(clockpunch[w].minute*100)+(clockpunch[w].second);
			if (u >= v)
			{
				v = u;
				x = clockpunch[w].action;
			}
			if (clockpunch[w].action == 6)
			{
				w = 14;
				v = 6;
			}
			w += 1;
		}
	}
	else
	{
		x = 0;
	}
	while (i < 1 || i > 14)
	{
		printf("Please enter what action you wish to take\n");
		switch (x)
		{
			case 0 :
				printf(" 1     Clock in for the day.\n");
		    break;
			case 1 :
				printf(" 2     Clock out for lunch.\n");
				printf(" 6     Clock out for the day.\n");
				printf(" 7     Clock out for first break.\n");
		    break;
		  case 2 :
				printf(" 3     Clock in from lunch.\n");
        break;
      case 3 :
				printf(" 4     Clock out for dinner.\n");
				printf(" 6     Clock out for the day.\n");
				if (pitemflag[6] == 0)
				{
					printf(" 7     Clock out for first break.\n");
				}
				else if (pitemflag[8] == 0)
				{
					printf(" 9     Clock out for second break.\n");
				}
				else if (pitemflag[10] == 0)
				{
					printf("11     Clock out for third break.\n");
				}
				else if (pitemflag[12] == 0)
				{
					printf("13     Clock out for fourth break.\n");
				}
				break;
			case 4 :
				printf(" 5     Clock in from dinner.\n");
				break;
      case 5 :
				printf(" 6     Clock out for the day.\n");
				if (pitemflag[6] == 0)
				{
					printf(" 7     Clock out for first break.\n");
				}
				else if (pitemflag[8] == 0)
				{
					printf(" 9     Clock out for second break.\n");
				}
				else if (pitemflag[10] == 0)
				{
					printf("11     Clock out for third break.\n");
				}
				else if (pitemflag[12] == 0)
				{
					printf("13     Clock out for fourth break.\n");
				}
				break;
      case 6 :
				printf("\n\nYou have exhausted all of your options for today\n");
				printf("Please contact your supervisor if you believe\n");
				printf("you have received this message in error or you\n");
				printf("are not finished working for today.\n");
				printf("Press <ENTER> to exit\n")	;
				getchar();
				exit(1);
				break;
			case 7 :
				printf(" 8     Clock in from first break.\n");
				break;
			case 8 :
				if (pitemflag[1] == 0)
				{
					printf(" 2     Clock out for lunch.\n");
				}
				else if (pitemflag[3] == 0)
				{
					printf(" 4     Clock out for dinner.\n");
				}
				printf(" 6     Clock out for the day.\n");
				printf(" 9     Clock out for second break.\n");
				break;
			case 9 :
				printf("10     Clock in from second break.\n");
				break;
			case 10 :
				if (pitemflag[1] == 0)
				{
					printf(" 2     Clock out for lunch.\n");
				}
				else if (pitemflag[3] == 0)
				{
					printf(" 4     Clock out for dinner.\n");
				}
				printf(" 6     Clock out for the day.\n");
				printf("11     Clock out for third break.\n");
				break;
			case 11 :
				printf("12     Clock in from third break.\n");
				break;
			case 12 :
				if (pitemflag[1] == 0)
				{
					printf(" 2     Clock out for lunch.\n");
				}
				else if (pitemflag[3] == 0)
				{
					printf(" 4     Clock out for dinner.\n");
				}
				printf(" 6     Clock out for the day.\n");
				printf("13     Clock out for fourth break.\n");
				break;
			case 13 :
				printf("14     Clock in from fourth break.\n");
				break;
			case 14 :
				if (pitemflag[1] == 0)
				{
					printf(" 2     Clock out for lunch.\n");
				}
				else if (pitemflag[3] == 0)
				{
					printf(" 4     Clock out for dinner.\n");
				}
				printf(" 6     Clock out for the day.\n");
				break;
		}
		printf("\nEnter an item from the list ===> ");
		fgets(y, 80, stdin);
		i = atoi(y);
		/* this if statement checks that an item on the menu was entered */
		if (i < 1 || i > 14)
		{
			printf("\n");
			printf("The number you entered is not valid.\n");
			printf("\n");
			printf("\n");
			printf("the number must be from the list below. \n");
			
			i = 0;
		}
	
	/* This section of code makes sure that a menu item is only used once */
		if ( i != 0)
		{
			i = (pitemflag[i-1] == 1 ? 98 : i);
			if ( i == 98 )
			{
				printf("\n");
				printf("The number you entered is not valid.\n");
				printf("You have already used that action for today\n");
				printf("\n");
				printf("the number must be from the list below. \n");
		
				i = 0;
			}
		}
		if ( i != 0)
		{
			switch (x)
			{
				case 0 :
					if (i != 1)
					{
						i = 99;
					}
			    break;
				case 1 :
					if (i != 2 && i != 6 && i != 7)
					{
						i = 99;
					}
			    break;
			  case 2 :
					if (i != 3)
					{
						i = 99;
					}
 	       break;
 	     case 3 :
					if (pitemflag[6] == 0)
					{
						if (i != 4 && i != 6 && i != 7)
						{
							i = 99;
						}
					}
					else if (pitemflag[8] == 0)
					{
						if (i != 4 && i != 6 && i != 9)
						{
							i = 99;
						}
					}
					else if (pitemflag[10] == 0)
					{
						if (i != 4 && i != 6 && i != 11)
						{
							i = 99;
						}
					}
					else if (pitemflag[12] == 0)
					{
						if (i != 4 && i != 6 && i != 13)
						{
							i = 99;
						}
					}
					break;
				case 4 :
					if (i != 5)
					{
						i = 99;
					}
					break;
   	   case 5 :
					if (pitemflag[6] == 0)
					{
						if (i != 6 && i != 7)
						{
							i = 99;
						}
					}
					else if (pitemflag[8] == 0)
					{
						if (i != 6 && i != 9)
						{
							i = 99;
						}
					}
					else if (pitemflag[10] == 0)
					{
						if (i != 6 && i != 11)
						{
							i = 99;
						}
					}
					else if (pitemflag[12] == 0)
					{
						if (i != 6 && i != 13)
						{
							i = 99;
						}
					}
					break;
 	     case 6 :
					printf("You have exhausted all of your options for today\n");
					printf("Please contact your supervisor if you believe\n");
					printf("you have received this message in error or you\n");
					printf("are not finished working for today.\n");
					printf("Press <ENTER> to exit\n")	;
					getchar();
					exit(1);
					break;
				case 7 :
					if (i != 8)
					{
						i = 99;
					}
					break;
				case 8 :
					if (pitemflag[1] == 0)
					{
						if (i != 2 && i != 6 && i != 9)
						{
							i = 99;
						}
					}
					else if (pitemflag[3] == 0)
					{
						if (i != 4 && i != 6 && i != 9)
						{
							i = 99;
						}
					}
					break;
				case 9 :
					if (i != 10)
					{
						i = 99;
					}
					break;
				case 10 :
					if (pitemflag[1] == 0)
					{
						if (i != 2 && i != 6 && i != 11)
						{
							i = 99;
						}
					}
					else if (pitemflag[3] == 0)
					{
						if (i != 4 && i != 6 && i != 11)
						{
							i = 99;
						}
					}
					break;
				case 11 :
					if (i != 12)
					{
						i = 99;
					}
					break;
				case 12 :
					if (pitemflag[1] == 0)
					{
						if (i != 2 && i != 6 && i != 13)
						{
							i = 99;
						}
					}
					else if (pitemflag[3] == 0)
					{
						if (i != 4 && i != 6 && i != 13)
						{
							i = 99;
						}
					}
					break;
				case 13 :
					if (i != 14)
					{
						i = 99;
					}
					break;
				case 14 :
					if (pitemflag[1] == 0)
					{
						if (i != 2 && i != 6)
						{
							i = 99;
						}
					}
					else if (pitemflag[3] == 0)
					{
						if (i != 4 && i != 6)
						{
							i = 99;
						}
					}
					break;
			}
			if ( i == 99 )
			{
				printf("\n");
				printf("The number you entered is not valid.\n");
				printf("That number is not an allowed choice.\n");
				printf("\n");
				printf("The number must be from the list below. \n");
			
				i = 0;
			}
		}
	}

	/* If everything is correct the program assigns the action code to the pointer */
	currentpunch->action = i;
	
	z = 1;
	return z;
}

int writerecord(timecardrecord *currentpunch, char *pfilename)
{
	int a, b, c, d, e, f, g, h, z, y;
	FILE *fp;
	char testuserid[4];
	int testaction, testyear, testmonth, testday, testhour, testminute, testsecond;
	
	/* opens the file and writes the timecard record to it */
	fp = fopen(pfilename, "a");
	fprintf(fp, "%s %02d%02d%02d%04d%02d%02d%02d\n", currentpunch->userid, currentpunch->action, currentpunch->month, currentpunch->day, currentpunch->year, currentpunch->hour, currentpunch->minute, currentpunch->second);
	fclose(fp);
	
	/* opens the file and reads back in the record just entered */
	fp = fopen(pfilename, "r");
	y = 1;
	while ( y != 0)
	{
		fscanf(fp, "%s %02d%02d%02d%04d%02d%02d%02d\n", testuserid, &testaction, &testmonth, &testday, &testyear, &testhour, &testminute, &testsecond);
		a = strcmp(currentpunch->userid, testuserid);
		b = (currentpunch->action == testaction ? 0 : 1);
		c = (currentpunch->month == testmonth ? 0 : 1);
		d = (currentpunch->day == testday ? 0 : 1);
		e = (currentpunch->year == testyear ? 0 : 1);
		f = (currentpunch->hour == testhour ? 0 : 1);
		g = (currentpunch->minute == testminute ? 0 : 1);
		h = (currentpunch->second == testsecond ? 0 : 1);
		y = a + b + c + d + e + f + g + h;
	}
	fclose(fp);
	
	/* prints both copys of the information for verification */
	printf("\nprinted to filename %s - %s-%02d-%02d-%02d-%04d-%02d-%02d-%02d\n", pfilename, currentpunch->userid, currentpunch->action, currentpunch->month, currentpunch->day, currentpunch->year, currentpunch->hour, currentpunch->minute, currentpunch->second);
	printf("read from filename  %s - %s-%02d-%02d-%02d-%04d-%02d-%02d-%02d\n", pfilename, testuserid, testaction, testmonth, testday, testyear, testhour, testminute, testsecond);
	printf("\n\nPress <ENTER> to exit\n")	;
	getchar();
	
	z = 1;
	return z;
}	

int manager(char userid[4], int usertype, int weekends)
{
	clearscreen();
	printf("\n\nManager Functions");
	printf("\n\nPress <ENTER> to exit\n");
	getchar();
	return 0;
}

int reporter(char userid[4], int usertype, int weekends)
{
	char usefile[12];
	int a, b;
	
	a = usefilename(&usefile[0]);
	if (a != 1)
	{
		b = printhoursreport(usefile, weekends);
	}
	exit(0);
}

int usefilename(char *pusefile)
{
	int h = 0, j;
	char y[80], z[13];
	FILE *fp;
	
	while (h == 0)
	{
		printf("Enter the name of the file you wish to open\n");
		printf("\nThe format for the name is \"tc<year><week number>.tcd\"\n");
		printf("This means the third week of the year 2000 would be\n");
		printf("written tc200003.scd.\n");
		printf("Type \"quit\" to quit the program.\n");
		printf("Filename :  ");
		fgets(y, 80, stdin);
		for (j = 0; j < 13; j++)
		{
			if (j == 13)
			{
				z[j] = '\0';
			}
			else
			{
				if (y[j] != '\n')
				{
					z[j] = y[j];
				}
				else
				{
					z[j] = '\0';
					j = 13;
				}
			}
		}
		if (!strcmp(y , "quit\n"))
		{
			printf("quiting\n");
			return 1;
		}
		if ( (fp = fopen(z, "r")) != NULL)
		{
			h = 1;
			fclose(fp);
			strcpy(pusefile, z);
		}
		else
		{
	 		printf("There was an error opening %s.  Please try\n", pusefile);
			printf("entering the file name again\n");
			h = 0;
		}
	}
	return 0;
}

int printhoursreport(char usefile[12], int weekends)
{
	int e, f;
	char reportfile[12];
	
	e = reportfilename(usefile, &reportfile[0]);
	f = writereportdata(usefile, reportfile, weekends);
	return 0;
}

int reportfilename(char usefile[12], char *preportfile)
{
	int y;
	char z[7];
	
	for (y = 2; y <= 8; y++)
	{
		if ( y == 8)
		{
			z[y - 2] = '\0';
		}
		else
		{
			z[y - 2] = usefile[y];
		}
	}
	sprintf (preportfile, "th%s.tcr", z);
	return 0;
}

int writereportdata(char usefile[12], char reportfile[12], int weekends)
{
	int i,j;
	daterecord filedates[7];
	
	i = getreportdates(usefile, &filedates[0], weekends);
	j = generatereport(usefile, reportfile, filedates);
	return 0;
}

int getreportdates(char usefile[12], daterecord *pfiledates, int weekends)
{
  time_t now, test;
  struct tm *timenow, *timetest, *timetemp;

  int a, y, z = 1;
  int weekfile, weektest;
  int yearfile;
  int weekdaynow;
	char weekstringfile[3], weekstringtest[3];
	char yearstringfile[5];
	
  /* get the first of year time information */
	for (y = 2; y <= 6; y++)
	{
		if (y == 6)
		{
			yearstringfile[y - 2] = '\0';
		}
		else
		{
			yearstringfile[y - 2] = usefile[y];
		}
	}
	for (y = 6; y <= 8; y++)
	{
		if (y == 8)
		{
			weekstringfile[y - 6] = '\0';
		}
		else
		{
			weekstringfile[y - 6] = usefile[y];
		}
	}
	weekfile = atoi(weekstringfile);
	yearfile = atoi(yearstringfile);
  now = time(0);
	timenow = localtime(&now);
	timetemp = timenow;
	timetemp->tm_year = yearfile - 1900;
	timetemp->tm_mon = 0;
	timetemp->tm_mday = 1;
	test = mktime(timetemp);
	timetest = localtime(&test);
	weekdaynow = timetest->tm_wday;
	if (weekdaynow <= weekends)
	{
		timetemp->tm_mday = timetemp->tm_mday + (6 - weekdaynow + weekends);
		test = mktime(timetemp);
		timetest = localtime(&test);
		timetemp = timetest;
		strftime(weekstringtest, 3, "%U", timetest);
		weektest = atoi(weekstringtest);
	}
	else
	{
		timetemp->tm_mday = timetemp->tm_mday + (weekends - weekdaynow);
		test = mktime(timetemp);
		timetest = localtime(&test);
		timetemp = timetest;
		strftime(weekstringtest, 3, "%U", timetest);
		weektest = atoi(weekstringtest);
	}
	while (weektest != weekfile)
	{
		timetemp->tm_mday = timetemp->tm_mday + 7;
		test = mktime(timetemp);
		timetest = localtime(&test);
		timetemp = timetest;
		strftime(weekstringtest, 3, "%U", timetest);
		weektest = atoi(weekstringtest);
	}
	timetemp->tm_mday = timenow->tm_mday - 7;
	test = mktime(timetemp);
	timetest = localtime(&test);
	for (z = 0; z < 7; z++)
	{
		timetemp->tm_mday = timetemp->tm_mday + 1;
		test = mktime(timetemp);
		timetest = localtime(&test);
		pfiledates[z].day = timetest->tm_mday;
		pfiledates[z].month = timetest->tm_mon + 1;
		pfiledates[z].year = timetest->tm_year + 1900;
	}
	
	return 0;
}

int generatereport(char usefile[12], char reportfile[12], daterecord filedates[7])
{
 	int a, b, c, d, e, f, y;
	FILE *fp, *fq, *fr;
	char testuserid1[4], testuserid2[4];
	int testaction, testyear, testmonth, testday, testhour, testminute, testsecond, testrights;
	float worktime, breaktime;
	float reporttimes[14] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} ;   /* A flag array to hold whether a time card item has been used */
	timecardrecord reportpunch[14];  /* an array of structures to hold previous time clock entries */
	
	fp = fopen("userlist.txt", "r");
	fr = fopen(reportfile, "w");
	while (!feof(fp))
	{
		fscanf(fp, "%s %02d\n", testuserid1, &testrights);
		fprintf(fr, "%s\n", testuserid1);
		fprintf(fr, "Date of Work   Work Hours   Break Hours\n");
		for (a = 0; a < 7; a++)
		{
			for (b = 0; b < 14; b++)
			{
				strcpy(reportpunch[b].userid, "   ");
				reportpunch[b].action = 0;
				reportpunch[b].month = 0;
				reportpunch[b].day = 0;
				reportpunch[b].year = 0;
				reportpunch[b].hour = 0;
				reportpunch[b].minute = 0;
				reportpunch[b].second = 0;
				reporttimes[b] = 0.0;
			}
			if ((fq = fopen(usefile, "r")) != NULL)
			{
				while ( !feof(fq))
				{
					fscanf(fq, "%s %02d%02d%02d%04d%02d%02d%02d\n", testuserid2, &testaction, &testmonth, &testday, &testyear, &testhour, &testminute, &testsecond);
					e = strcmp(testuserid1, testuserid2);
					f = (filedates[a].month == testmonth ? 0 : 1);
					c = (filedates[a].day == testday ? 0 : 1);
					d = (filedates[a].year == testyear ? 0 : 1);
					y = ((e == 0 && f == 0 && c == 0 && d == 0) ? 0 : 1);
					if (y == 0)
					{
						strcpy(reportpunch[testaction].userid, testuserid2);
						reportpunch[testaction - 1].action = testaction;
						reportpunch[testaction - 1].month = testmonth;
						reportpunch[testaction - 1].day = testday;
						reportpunch[testaction - 1].year = testyear;
						reportpunch[testaction - 1].hour = testhour;
						reportpunch[testaction - 1].minute = testminute;
						reportpunch[testaction - 1].second = testsecond;
						reporttimes[testaction - 1] = (float)(testhour) + (float)(testminute/60.0);
						reporttimes[testaction - 1] = (((int)(reporttimes[testaction - 1]*100) % 1 >= 5) ? (((reporttimes[testaction - 1]*100)+ 1)/100.0) : (((reporttimes[testaction - 1]*100)+ 0)/100.0));
					}
				}
				fclose(fq);
				breaktime = (reporttimes[7]-reporttimes[6]) + (reporttimes[9]-reporttimes[8]) + (reporttimes[11]-reporttimes[10]) + (reporttimes[13]-reporttimes[12]);
				worktime = (reporttimes[5]-reporttimes[0]) - (reporttimes[2]-reporttimes[1]) - (reporttimes[4]-reporttimes[3]);
				fprintf(fr, " %02d/%02d/%04d    %9.2f   %10.2f\n", filedates[a].month, filedates[a].day, filedates[a].year, worktime, breaktime);
			}
		}
	}
	fclose(fr);
	fclose(fp);
	return 0;
}

int administrator(char userid[4], int usertype, int weekends)
{
	clearscreen();
	printf("\n\nAdministrator Functions");
	printf("\n\nPress <ENTER> to exit\n");
	getchar();
	return 0;
}

void clearscreen(void)
{
	int c, d;
	for (c = 0; c <= 100; c++)
	{
		for (d = 0; d <= 200; d++)
		{
			printf(" ");
		}
		printf("\n");
	}
}

