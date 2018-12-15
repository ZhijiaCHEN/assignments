/***************************************************************************
Matrix multiplication seq program
****************************************************************************/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "my_global.h" // MySQL globals
#include "mysql.h"

double C[N][N];
double A[N][N], B[N][N];
double wall_clock();

int main(int argc, char *argv[])
{
	char host[128];
	int i, j, k, logDB;
	double t0, t1, Mflops;
	FILE *fd;
	int log = 0;
	MYSQL *con; // MySQL connection
	MYSQL_RES *result; // MySQL result set
	MYSQL_ROW row;
	char *server;
	char *user;
	char *password;
	char *database;
	char query[1000];
	char order[4];
	struct tm *newtime;
	time_t ltime;
	char *asctime(const struct tm *time);

	if (argc < 6) {
		printf("Usage: %s order SERVER USER PASSWORD DATABASE (logDB)\n", argv[0]);
		exit(0);
	}
	strcpy(order, argv[1]);
	server = argv[2];
	user = argv[3];
	password = argv[4];
	database = argv[5];
	if (argc > 6) {
		logDB = 1;
	} else logDB = 0;
	printf("Order(%s)\n",order);
	time(&ltime);
	newtime = localtime(&ltime);
	
//	printf("The MySQL client version is: %s\n", mysql_get_client_info());
	con = mysql_init(NULL); // get a connection
	if (logDB) {
		if (!mysql_real_connect(con, server, user, password, database, 0, NULL, 0)) {
			perror(mysql_error(con));
			exit(-1); 
		}
	}
	/* truncate the table
	strcpy(query, "truncate table PLogs");
	if (mysql_query(con, query)) {
		perror(mysql_error(con));
		exit(-2);
	} */
	// Get hostname
	gethostname(host, sizeof(host));
	for (i=0; i < N ; i++)
	{
		for (j=0; j < N; j++)
		{
			C[i][j] = 0;
			B[i][j] = (double) i * j ; 
			A[i][j] = (double) i * j ;
		}
	}
	t0 = wall_clock();	
	if (strcmp(order,"ijk") == 0) {
		// ijk
		for (i=0; i < N; i++)
			for (j=0; j < N; j++)
			for (k=0; k < N; k++)
				C[i][j] += A[i][k]*B[k][j];
		t1 = wall_clock() - t0;
		// fprintf(fd, "%d %f %f\n", N, t1, ((N*N)/t1)*N);
		if (t1 > 0) Mflops = ((N*N)/t1)*N;
		else Mflops = 0;
	  	printf ("(%s)[%s](%d) i-j-k(us)(%f) MFLOPS(%f)\n",asctime(newtime),host,N, t1,Mflops);
		sprintf(query,
		  "insert into PLogs (Timestamp,Host,Size,ElapsedTime,MFLOPS,LoopOrder) values (\"%s\",\"%s\",%d,%f,%f,\"i-j-k\")",
			asctime(newtime),host,N,t1,Mflops);
		if (logDB) {
			if (mysql_query(con, query)) {
				perror(mysql_error(con));
				exit(-2);
			}
		}
	} else if (strcmp(order, "ikj") == 0) {
		// ikj
		for (i=0; i < N; i++)
			for (k=0; k < N; k++)
			for (j=0; j < N; j++)
				C[i][j] += A[i][k]*B[k][j];
		t1 = wall_clock() - t0;
		if (t1 > 0) Mflops = ((N*N)/t1)*N;
		else Mflops = 0;
		printf ("(%s)[%s](%d) i-k-j (us)(%f) MFLOPS(%f)\n",
			asctime(newtime),host,N, t1,Mflops);
		sprintf(query,"insert into PLogs (Timestamp,Host,Size,ElapsedTime,MFLOPS,LoopOrder) values (\"%s\",\"%s\",%d,%f,%f,\"i-k-j\")",
			asctime(newtime),host,N,t1,Mflops);
		if (logDB) {
			if (mysql_query(con, query)) {
				perror(mysql_error(con));
				exit(-2);
			}
		}
	} else if (strcmp(order, "jik") == 0) {
		for (j=0; j < N; j++)
			for (i=0; i < N; i++)
			for (k=0; k < N; k++)
				C[i][j] += A[i][k]*B[k][j];
		t1 = wall_clock() - t0;
		if (t1 > 0) Mflops = ((N*N)/t1)*N;
		else Mflops = 0;
	  	printf ("(%s)[%s](%d) j-i-k Elapsed (us):(%f) MFLOPS:(%f)\n",
			asctime(newtime),host,N, t1,Mflops);
		sprintf(query,"insert into PLogs (Timestamp,Host,Size,ElapsedTime,MFLOPS,LoopOrder) values (\"%s\",\"%s\",%d,%f,%f,\"j-i-k\")",
			asctime(newtime),host,N,t1,Mflops);
		if (logDB) {
			if (mysql_query(con, query)) {
				perror(mysql_error(con));
				exit(-2);
			}
		}
	} else if (strcmp(order,"jki") == 0) {
		for (j=0; j < N; j++)
			for (k=0; k < N; k++)
			for (i=0; i < N; i++)
				C[i][j] += A[i][k]*B[k][j];
		t1 = wall_clock() - t0;
		if (t1 > 0) Mflops = ((N*N)/t1)*N;
		else Mflops = 0;
	  	printf ("(%s)[%s](%d) j-k-i Elapsed (us):(%f) MFLOPS:(%f)\n",
			asctime(newtime),host,N, t1,Mflops);
		sprintf(query,"insert into PLogs (Timestamp,Host,Size,ElapsedTime,MFLOPS,LoopOrder) values (\"%s\",\"%s\",%d,%f,%f,\"j-k-i\")",
			asctime(newtime),host,N,t1,Mflops);
		if (logDB) {
			if (mysql_query(con, query)) {
				perror(mysql_error(con));
				exit(-2);
			}
		}
	} else if (strcmp(order, "kij") == 0) {
		for (k=0; k < N; k++)
			for (i=0; i < N; i++)
			for (j=0; j < N; j++)
				C[i][j] += A[i][k]*B[k][j];
		t1 = wall_clock() - t0;
		if (t1 > 0) Mflops = ((N*N)/t1)*N;
		else Mflops = 0;
	  	printf ("(%s)[%s](%d) k-i-j Elapsed (us):(%f) MFLOPS:(%f)\n",
			asctime(newtime),host,N, t1,Mflops);
		sprintf(query,"insert into PLogs (Timestamp,Host,Size,ElapsedTime,MFLOPS,LoopOrder) values (\"%s\",\"%s\",%d,%f,%f,\"k-i-j\")",
			asctime(newtime),host,N,t1,Mflops);
		if (logDB) {
			if (mysql_query(con, query)) {
				perror(mysql_error(con));
				exit(-2);
			}
		}
	} else if (strcmp(order, "kji") == 0) {
		for (k=0; k < N; k++)
			for (j=0; j < N; j++)
			for (i=0; i < N; i++)
				C[i][j] += A[i][k]*B[k][j];
		t1 = wall_clock() - t0;
		if (t1 > 0) Mflops = ((N*N)/t1)*N;
		else Mflops = 0;
	  	printf ("(%s)[%s](%d) k-j-i Elapsed (us):(%f) MFLOPS:(%f)\n",
			asctime(newtime),host,N, t1,Mflops);
		sprintf(query,"insert into PLogs (Timestamp,Host,Size,ElapsedTime,MFLOPS,LoopOrder) values (\"%s\",\"%s\",%d,%f,%f,\"k-j-i\")",
			asctime(newtime),host,N,t1,Mflops);
		if (logDB) {
			if (mysql_query(con, query)) {
				perror(mysql_error(con));
				exit(-2);
			}
		}
	} else {
		printf("Unrecognized order? (%s) (%d)\n", order, strcmp(order,"ijk"));
		exit(0);
	}
	mysql_close(con);
	exit(0);
}


/* 		Wall_clock()		*/
/* Author: Wenhai Jiang			*/
/* Date:   October 1994			*/
#include <sys/time.h>
#include <sys/resource.h>

double wall_clock()
{
struct timeval tp;
struct timezone tzp;
double t;
	gettimeofday(&tp, &tzp);
	t = (tzp.tz_minuteswest*60 + tp.tv_sec)*1.0e6 + (tp.tv_usec)*1.0;
	return t;
}
