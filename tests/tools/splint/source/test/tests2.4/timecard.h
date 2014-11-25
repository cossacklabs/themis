
/***************************************************************************
                                 timecard.h
               A small program to collect timecard information
                             -------------------
    begin                : Thu Feb 24 2000
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

/*DEFINE list*/

/* User defined variable types */
typedef struct {
	char userid[4];     /* the user id or employee number */
	int hour;           /* time portion - hours */
	int minute;         /* time portion - minutes */
	int second;
	int month;          /* date portion - month */
	int day;
	int year;
	int action;         /* holds the action being taken */
}timecardrecord;

typedef struct {
	int month;          /* date portion - month */
	int day;
	int year;
}daterecord;

/* Function List*/
int initializeprogram(char *puserid, int *pusertype, int *pweekends);
int getenvironment(int *pweekends);
int getuserid(char *puserid, int *pusertype);
int startprogram(char *puserid, int *pusertype, int *pweekends);

int timecard(char userid[4], int usertype, int weekends);
int gettimeanddate(timecardrecord *currentpunch, char *pweekstring, char *pyearstring, int weekends);
int getfilename(char *pfilename, char *pweekstring, char *pyearstring);
int gettimecarddata(timecardrecord *currentpunch, timecardrecord *clockpunch, int *pitemflag, char *pfilename);
int printtimecarddata(timecardrecord *currentpunch, timecardrecord *clockpunch);
int getaction(timecardrecord *currentpunch, timecardrecord *clockpunch, int *pitemflag);
int writerecord(timecardrecord *currentpunch, char *pfilename);

int manager(char userid[4], int usertype, int weekends);

int reporter(char userid[4], int usertype, int weekends);
int usefilename(char *pusefile);
int printhoursreport(char *pusefile, int weekends);
int reportfilename(char usefile[13], char *preportfile);
int writereportdata(char usefile[13], char reportfile[13], int weekends);
int getreportdates(char usefile[13], daterecord *pfiledates, int weekends);
int generatereport(char usefile[13], char reportfile[13], daterecord filedates[7]);

int administrator(char userid[4], int usertype, int weekends);

void clearscreen(void);
