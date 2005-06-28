/*
 * client.c - ChaSen client
 *
 * Copyright (C) 1996-1997 Nara Institute of Science and Technology
 *
 * Author: A.Kitauchi <akira-k@is.aist-nara.ac.jp>, Apr. 1996
 *
 */

#ifndef _WIN32

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <signal.h>
#include <errno.h>

#if defined _WIN32 && ! defined __CYGWIN__

#ifdef HAVE_WINSOCK_H
#include <winsock.h>
#endif

#else

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#endif

#include <juman.h>

/* max of the command line length */
#define CHA_COMM_LINE_MAX 1024

/* chalib.hからの定義
   NACSIS 吉岡
*/
#define CHA_INPUT_SIZE      8192

/* juman.cからの定義
   NACSIS 吉岡
*/
extern FILE	*Jumanrc_Fileptr;

/*
 * check_status
 */
static void check_status(ifp, mes)
    FILE *ifp;
    char *mes;
{
    char line[CHA_INPUT_SIZE];

    fgets(line, sizeof(line), ifp);

#if 0    
    fprintf(stderr, "##%s\n", line);
#endif

    if (strncmp(line, "200 ", 4)) {
	if (mes == NULL)
	  fputs(line + 4, stderr);
	else
	  cha_exit(1, mes);
	exit(1);
    }
}

/*
 * do_chasen_client
 */
/*
  cha_fgetsをデフォルトで用いられるfget_lineに変更
  NACSIS 吉岡
*/
static void fp_copy(ofp, ifp)
    FILE *ofp, *ifp;
{
    char line[CHA_INPUT_SIZE];

    /* while (fget_line(line, sizeof(line), ifp) != NULL) { */
    while (fgets(line, sizeof(line), ifp) != NULL) {
	fputs(line, ofp);
	fflush(ofp);
    }

    if (ifp != stdin)
      fclose(ifp);
}

/*
 * send_chasenrc
 */
/*
  send_chasenrcをsend_jumanrcに改名
  rcファイルに関するチェックは起動時のオプションのチェックで行っているので、
  ここでは省略
  NACSIS 吉岡
*/
static void send_jumanrc(ifp, ofp)
    FILE *ifp, *ofp;
{
    FILE *fp;
  
    fputs("RC\n", ofp);
    fp_copy(ofp, Jumanrc_Fileptr);
    fprintf(ofp, "\n%c\n", EOf);
    fflush(ofp);

    check_status(ifp, "Error found in reading .jumanrc");
}

/*
 * escape_string
 */
static char *escape_string(dst_str, src_str)
    char *dst_str, *src_str;
{
    char *src, *dst;

    dst = dst_str;
    for (src = src_str; *src; src++) {
	if (*src == ' ' || *src == '"' || *src == '\'' || *src == '\\')
	  *dst++ = '\\';
	*dst++ = *src;
    }
    *dst = '\0';

    return dst_str;
}

/*
 * getopt_client
 */
/*
  getopt_clientをoption_proc_for_clientに改名
  オプションの取扱いをjuman.cのoption_procを参考にしながら変更
 NACSIS 吉岡
*/
static char *option_proc_for_client(argc, argv)
    int argc;
    char **argv;
{
    int 	i;
    static char option[CHA_COMM_LINE_MAX];
    static char arg[CHA_COMM_LINE_MAX];
    char *op;

    op = option;

    for ( i=1; i<argc; i++ ) {
      if ( argv[i][0] != '-' ) {
	fprintf(stderr, "Invalid Option !!\nHELP command for more detail.\n");
      }
      else if ( argv[i][1] == 'b' || argv[i][1] == 'm' || argv[i][1] == 'p' ||
		argv[i][1] == 'B' || argv[i][1] == 'P' || argv[i][1] == 'f' ||
		argv[i][1] == 'e' || argv[i][1] == 'c' || argv[i][1] == 'E')
	{
	  sprintf(op, "%s ", argv[i]);
	  op += strlen(op);
	}
      else if ( argv[i][1] == 'i' )
	{
	  sprintf(op, "%s %s ", argv[i], escape_string(arg, argv[i+1]));
	  i++;
	  op += strlen(op);
	}
      else if ( argv[i][1] == 'r' || argv[i][1] == 'C'
		|| argv[i][1] == 'N')
	i++;
      else
	{
	  fprintf(stderr, "Invalid Option !!\n");
	}
    }
    
    return option;
}
    

static int fork_and_gets(ifp)
    FILE *ifp;
{
    int pid;
    int i;
    char line[CHA_INPUT_SIZE];

    if ((pid = fork()) < 0) {
	cha_perror("fork");
	return -1;
    }

    if (pid)
      return pid;

    /* child process */
    check_status(ifp, "error");

    while (fgets(line, sizeof(line), ifp) != NULL) {
	if (line[0] == EOf &&
	    (line[1] == '\n' || line[1] == '\r' && line[2] == '\n'))
	    break;
	fputs(line, stdout);
	fflush(stdout);
    }
    fclose(ifp);
    exit(0);
}

/*
 * close_connection
 */
static void close_connection(pid, ofp)
    int pid;
    FILE *ofp;
{
    int status;

    fprintf(ofp, "\n%c\nQUIT\n", EOf);
    fclose(ofp);
    while (wait(&status) != pid)
      ;
}

/*
 * connect_server
 */
static int open_connection(server, port)
    char *server;
    unsigned short port;
{
    int sfd;
    struct sockaddr_in sin;
    struct hostent *host;

    if((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	cha_perror("socket");
	return -1;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_port = htons(port);

    if ((host = gethostbyname(server)) != NULL) {
	memcpy(&sin.sin_addr.s_addr, host->h_addr, host->h_length);
	sin.sin_family = host->h_addrtype;
    } else if ((sin.sin_addr.s_addr = inet_addr(server)) != (unsigned long)-1) {
	sin.sin_family = AF_INET;
    } else {
#if 0
	cha_perror("inet_addr");
#endif
	cha_exit(-1, "Can't get address: %s\n", server);
	return -1;
    }

    if (connect(sfd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
	cha_perror("connect");
	close(sfd);
	return -1;
    }

    return sfd;
}

/*
 * chasen_client
 *
 * return code: exit code
 */
/*
  引数としてargcを追加オプションの扱いをJUMAN用へ変更
  NACSIS 吉岡
*/
int juman_client(argc, argv, server, port)
    int argc;
    char **argv;
    char *server;
    int port;
{
    int pid, sfd;
    char *option;
    FILE *ifp, *ofp;

    /* open connection to server */
    if ((sfd = open_connection(server, port)) < 0)
      return 1;

    ifp = fdopen(sfd, "r");
    ofp = fdopen(sfd, "w");
    check_status(ifp, "connection error");

    if (Jumanrc_Fileptr) 
	send_jumanrc(ifp, ofp);

    /* send RUN command with option */
    option = option_proc_for_client(argc, argv);
    fprintf(ofp, "RUN %s\n", option);

    /* fork_and_getsの中でforkする
       → 子プロセスがfork_and_getsの中でサーバからの出力を受け取って表示
       → 親プロセスがここにかえってきて入力をサーバに送る */

    if ((pid = fork_and_gets(ifp)) < 0)
      return 1;

    fp_copy(ofp, stdin);
    
    close_connection(pid, ofp);
    close(sfd);
    
    return 0;
}

#endif
