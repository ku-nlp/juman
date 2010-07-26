/*
==============================================================================
  juman.c
==============================================================================
*/

/*
------------------------------------------------------------------------------
  inclusion of header files
------------------------------------------------------------------------------
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include	<juman.h>

/*
------------------------------------------------------------------------------
  LOCAL:
  definition of global variables
------------------------------------------------------------------------------
*/

extern char		*ProgName;
extern int              Show_Opt1;
extern int              Show_Opt2;
extern char		Show_Opt_tag[MIDASI_MAX];
extern int		Show_Opt_jumanrc;
extern int		Show_Opt_debug;
extern int		Vocalize_Opt;
extern int		Repetition_Opt;
extern int              Onomatopoeia_Opt;
extern int		Normalized_Opt;
extern int		Unkword_Pat_Num;

extern FILE		*Jumanrc_Fileptr;
extern FILE 		*Cha_stderr;

extern U_CHAR	        String[LENMAX];

int             JUMAN_server_mode = FALSE;
int             JUMAN_server_foreground = FALSE;

#define JUMAN_PORT   32000
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

static char juman_host[MAXHOSTNAMELEN];
static unsigned short juman_port = JUMAN_PORT;


/*
------------------------------------------------------------------------------
  prototype definition of functions
------------------------------------------------------------------------------
*/

int	main(int argc, char **argv);
void	option_proc(int argc, char **argv);
void	juman_help(void);
void	juman_version(void);
/*
  サーバーモード対応のため、従来のmainの後半部であるjuman_standaloneと
  サーバーモードの関数であるjuman_serverを利用する。
  set_juman_server関数はクライアントモードのオプションであるホスト名と
  ポート番号の設定をオプションに対応して設定する関数 
  set_juman_port関数はサーバーモードのオプションであるポート番号の設定
  オプションに対応してポート番号を設定する関数 
  NACSIS 吉岡
*/
void    juman_standalone(void);
void    juman_server(char **argv, int port, int foreground);
static void set_juman_port(char* port);
static void set_juman_server(char* server);

/*
------------------------------------------------------------------------------
  PROCEDURE: <main>
------------------------------------------------------------------------------
*/
   
int main(int argc, char **argv)
{
    /*
         サーバモード：オプションで -s 指定
	 クライアントモード：環境変数 JUMANSERVER 指定 または
			     オプションで -D hostname 指定
	 スタンドアロンモード：その他
    */
    char *serv_env;
 
    ProgName = argv[0];
    option_proc(argc, argv);
    Cha_stderr = stderr;

#if ! defined _WIN32
    /* 環境変数 JUMANSERVER */

    if ((serv_env = getenv("JUMANSERVER")) != NULL)
	set_juman_server(serv_env);

    /* 各モードの処理 */
    if (JUMAN_server_mode) {	/* サーバモード */
	set_jumanrc_fileptr((Show_Opt_jumanrc ? argv[Show_Opt_jumanrc]: NULL),
			    TRUE, TRUE);
	juman_server(argv, juman_port, JUMAN_server_foreground);
    }
    else if (juman_host[0]) {	/* クライアントモード */
	set_jumanrc_fileptr((Show_Opt_jumanrc ? argv[Show_Opt_jumanrc]: NULL), 
			    FALSE, FALSE);	/* RC_DEFAULT をみない */
	juman_client(argc, argv, juman_host, juman_port);
    }
    else
#endif
    {			/* スタンドアロンモード */
	set_jumanrc_fileptr((Show_Opt_jumanrc ? argv[Show_Opt_jumanrc]: NULL), 
			    TRUE, TRUE);
	juman_standalone();
    }
    return 0;
}

int readtonl(FILE *fp)
{
    int input_buffer;

    while (1) {
	if ((input_buffer = fgetc(fp)) == EOF) return EOF;
	if (input_buffer == '\n') {
	    return FALSE;
	}
    }
    return TRUE;
}

    
/*
------------------------------------------------------------------------------
  PROCEDURE: <juman_standalone>
------------------------------------------------------------------------------
*/
/*
  従来のmainの後半部。ただし、出力のために呼び出す関数については、サーバー
  モード対応のための変更に応じて、stdoutを引数として取る。
  NACSIS 吉岡
 */

void juman_standalone(void)
{
    int length;
#ifdef _WIN32
    char *eucstr;
#endif
    
    if (!juman_init_rc(Jumanrc_Fileptr)) {	/* rcfile関係の初期化 */
	fprintf(stderr, "error in .jumanrc\n");
	exit(0);
    }
    juman_init_etc(); 	/* 未定義語処理，数詞処理，透過処理等の初期化 */
    if (Onomatopoeia_Opt) Unkword_Pat_Num = compile_unkword_patterns();

    String[LENMAX - 1] = '\n';
    while ( fgets(String, LENMAX, stdin) != NULL ) {
	if (String[LENMAX - 1] != '\n') {
	    String[LENMAX - 1] = '\0';
	    fprintf(stderr, "Too long input string (%s).\n", String);
	    String[LENMAX - 1] = '\n';
	    readtonl(stdin);
	    continue;
	}
#ifdef _WIN32
	eucstr = toStringEUC(String);
	strcpy(String,eucstr);
	free(eucstr);
#endif       
	
	length = strlen(String);
	if (length == LENMAX-1 && String[length - 1] != '\n') {
	    fprintf(stderr, "Too long input string (%s).\n", String);
	    continue;
	} else {
#ifdef _WIN32
	    if (String[length - 2] == '\r' && String[length - 1] == '\n')
		String[length - 2] = '\0';
#endif
	    if (String[length - 1] == '\n') String[length - 1] = '\0';
	    else String[length] = '\0';
	}

	if (Show_Opt_tag[0])
	    if (!strncmp(String , Show_Opt_tag , strlen(Show_Opt_tag))) {
		fprintf(stdout, "%s JUMAN:%s\n", String, VERSION);
		continue;
	    }

	if (juman_sent() == TRUE) {
	    switch (Show_Opt1) {
	    case Op_B:
		print_best_path(stdout); break;
	    case Op_M:
		print_all_mrph(stdout); break;
	    case Op_P:
		print_all_path(stdout); break;
	    case Op_BB:
	    case Op_PP:
		print_homograph_path(stdout); break;
	    default:
		break;
	    }
	}
	fprintf(stdout, "EOS\n");
	fflush(stdout);
    }
    juman_close();
}


/*
------------------------------------------------------------------------------
  PROCEDURE: <option_proc> <juman_help>
------------------------------------------------------------------------------
*/
/*
------------------------------------------------------------------------------
  PROCEDURE: <set_juman_port>
------------------------------------------------------------------------------
*/
/*
  サーバーモード用のオプションの取扱いに関する手続きとを付加すると共に、
  ソケットのポート番号を設定する関数であるset_juman_port関数を定義
  NACSIS 吉岡
*/  
void option_proc(int argc, char **argv)
{
    int 	i;

    Show_Opt1 = Op_BB;
    Show_Opt2 = Op_E2;
    Show_Opt_jumanrc = 0;
    Show_Opt_tag[0] = '\0';
    Show_Opt_debug = 0;
    Vocalize_Opt = 1;
    Repetition_Opt = 1;
    Onomatopoeia_Opt = 0;
    Normalized_Opt = 1;

    for ( i=1; i<argc; i++ ) {
	if ( argv[i][0] != '-' ) {
	    fprintf(stderr, "Invalid Option !!\n");
	    juman_help();
	}
else {
	    if ( argv[i][1] == 'b' ) Show_Opt1 = Op_B;
	    else if ( argv[i][1] == 'm' ) Show_Opt1 = Op_M;
	    else if ( argv[i][1] == 'p' ) Show_Opt1 = Op_P;
	    else if ( argv[i][1] == 'B' ) Show_Opt1 = Op_BB;
	    else if ( argv[i][1] == 'P' ) Show_Opt1 = Op_PP;
	    else if ( argv[i][1] == 'f' ) Show_Opt2 = Op_F;
	    else if ( argv[i][1] == 'e' && argv[i][2] == '\0' ) 
					  Show_Opt2 = Op_E;
	    else if ( argv[i][1] == 'e' && argv[i][2] == '2' ) 
					  Show_Opt2 = Op_E2;
	    else if ( argv[i][1] == 'c' ) Show_Opt2 = Op_C;
	    else if ( argv[i][1] == 'E' ) Show_Opt2 = Op_EE;
	    else if ( argv[i][1] == 'i' ) {
	        if (i != argc - 1)
		    strcpy(Show_Opt_tag, argv[i+1]), i++;
		else {
		    fprintf(stderr, "-i option needs a string !!\n");
		    juman_help();
		}
	    }
	    else if ( argv[i][1] == 'r' ) {
	        if (i != argc - 1)
		    Show_Opt_jumanrc = i+1, i++;
		else {
		    fprintf(stderr, "-r option needs an rc_file name !!\n");
		    juman_help();
		}
	    }
	    else if ( argv[i][1] == 'h' ) juman_help();
	    else if ( argv[i][1] == 'v' ) juman_version();
	    else if ( argv[i][1] == 'd' ) Show_Opt_debug = 1;
	    else if ( argv[i][1] == 'D' ) Show_Opt_debug = 2;
	    else if ( argv[i][1] == 'V' ) Vocalize_Opt = 0;
	    else if ( argv[i][1] == 'R' ) Repetition_Opt = 0;
	    else if ( argv[i][1] == 'o' ) Onomatopoeia_Opt = 1;
	    else if ( argv[i][1] == 'L' ) Normalized_Opt = 0;

#if ! defined _WIN32
	    /* サーバーモード用のオプションの取扱い */
            else if ( argv[i][1] == 'S' ) JUMAN_server_mode = TRUE;
            else if ( argv[i][1] == 'F' ) JUMAN_server_foreground = TRUE;
            else if ( argv[i][1] == 'N' ) /* port no */
		set_juman_port(argv[i+1]), i++;
	    /* クライアントモード用のオプションの取扱い */
            else if ( argv[i][1] == 'C' ) {
	        if (i != argc - 1)
		  set_juman_server(argv[i+1]), i++;
		else {
		  fprintf(stderr, "-C option needs server host name !!\n");
		  juman_help();
		}
	    }
#endif
	    else {
		fprintf(stderr, "Invalid Option !!\n");
		juman_help();
	    }
	}
    }
}

static void set_juman_port(char *port)
{
    if ((juman_port = atoi(port)) <= 0)
      fprintf(stderr, "Illegal port No: %s\n", port);
}

static void set_juman_server(server)
    char *server;
{
    char *colon;
    int len;

    if ((colon = strchr(server, ':')) == NULL) {
        len = strlen(server);
    } else {
        set_juman_port(colon + 1);
        len = (int)(colon - server);
    }

    if (len >= MAXHOSTNAMELEN)
      len = MAXHOSTNAMELEN - 1;

    memcpy(juman_host, server, len);
    juman_host[len] = '\0';
}

void juman_help()
{
#if ! defined _WIN32
    fprintf(stderr, "usage: juman -[b|B|m|p|P] -[f|c|e|E] [-S] [-N port] [-C host[:port]] [-i string] [-r rc_file]\n");
#else
    fprintf(stderr, "usage: juman -[b|B|m|p|P] -[f|c|e|E] [-i string] [-r rc_file]\n");
#endif
    fprintf(stderr, "\n");
    fprintf(stderr, "             -b : show best path\n");
    fprintf(stderr, "             -B : show best path including homographs (default)\n");
    fprintf(stderr, "             -m : show all morphemes\n");
    fprintf(stderr, "             -p : show all pathes\n");
    fprintf(stderr, "             -P : show all pathes by -B style\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "             -f : show formatted morpheme data\n");
    fprintf(stderr, "             -c : show coded morpheme data\n");
    fprintf(stderr, "             -e : show entire morpheme data\n");
    fprintf(stderr, "             -e2 : -e plus semantics data (default)\n");
    fprintf(stderr, "             -E : -e plus location and semantics data\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "             -V : not search voiceless morphemes\n");
    fprintf(stderr, "             -R : not recognize adverb automatically\n");
    fprintf(stderr, "             -L : not search normalized lowercase\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "             -i : ignore an input line startig with 'string'\n");
    fprintf(stderr, "             -r : use 'rc_file' as '.jumanrc'\n");
    fprintf(stderr, "             -v : show version\n");
    fprintf(stderr, "\n");
#if ! defined _WIN32
    fprintf(stderr, "             -S : start JUMAN server\n");
    fprintf(stderr, "             -F : force JUMAN server run in the foreground\n");
    fprintf(stderr, "                       (use with -S, do not go to the background)\n");
    fprintf(stderr, "             -N port : specify JUMAN server's port Number\n");
    fprintf(stderr, "                       (use with -S, the default is 32000)\n");
    fprintf(stderr, "             -C host[:port] connect to JUMAN server\n");
#endif
    exit(0);
}

void juman_version()
{
    fprintf(stderr, "%s %s\n", PACKAGE_NAME, VERSION);
    exit(0);
}
