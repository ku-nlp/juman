/*
==============================================================================
	juman_lib.c
==============================================================================
*/
 
/*
  連接表に基づく基本的処理を越える処理について，アルゴリズムを簡単に説
  明する．

  【連語処理】

  m_buffer 及び p_buffer の構造は，以前の juman と何ら変わりない．例と
  して，現在のラティスが次のような状態であるとする．

  発表 − する
  (T)     (T)

  連語「ことになる」が追加される場合，一気に 3つの m_buffer と 3つの 
  p_bufferが新規作成される．

  発表 − する − こと − に − なる
   (T)     (T)     (F)   (F)     (T)
                ^^^^^^^^^^^^^^^^^^^^ 一括追加

  但し，p_buffer の構造体に connect というメンバが追加されている．(上
  図において (T) が connect = TRUE，(F) が connect = FALSE を表してい
  る)これは，連語の途中に別の形態素へのつながりが侵入するのを防ぐため
  である．具体的には，連語内の形態素のうち，最後の形態素に対応する部分
  以外の p_bufferの connect を FALSE とする．これによって，次のような
  ラティスの生成を禁止できる．

  発表 − する − こと − に − なる
                       ＼
                          にな − ....  (連語内への割り込み)

  【透過処理】

  括弧や空白などが来た場合，その前の形態素の連接属性を透過させる処理．
  (例) 『ひとこと』で………   という文が入力された場合
  まず，次のようなラティスが生成される

  ひとこと　−  』(con_tbl は括弧閉のもの)
            ／
      こと

  その直後に through_word が呼ばれ，新たに 2つの m_buffer と 2つの 
  p_buffer が新規作成される．新規作成された m_bufferは，con_tbl だけが
  異なっていて，その他はオリジナルと全く同じである．

  ひとこと　−  』(「ひとこと」と同じ con_tbl)
            ＼
                』(con_tbl は括弧閉のもの)
            ／ 
      こと　−  』(「こと」と同じ con_tbl)

  以上のように，様々な連接属性(con_tbl)を持った m_buffer 及び p_buffer 
  が増植することによって，透過処理を実現している．

  (ただし，このままでは大量の空白などが続く場合にm_buffer, p_buffer が
  溢れてしまうので，重複するものを出来るだけ再利用し，枝刈りを適宜行う
  ことによって，爆発を防いでいる．)

  【数詞処理】

  数詞の直後に数詞が来た場合に，その2つを連結した新たな形態素を追加する．

  今月 − の − ２

  この次に数詞「０」が来た場合，まず次のようなラティスが生成される．

  今月 − の − ２ − ０

  その直後に suusi_word が呼ばれ，新たに数詞「２０」が追加される．

  今月 − の − ２ − ０
             ＼
                ２０

  ただし，この「２０」の形態素コストは後ろの「０」の方の形態素コストが
  コピーされる．
  */

/*
------------------------------------------------------------------------------
	inclusion of header files
------------------------------------------------------------------------------ */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include	<juman.h>
#include	<const.h>

/*
------------------------------------------------------------------------------
	definition of macros
------------------------------------------------------------------------------
*/

#define         DEF_CLASS_C             "品詞コスト"
#define         DEF_RENSETSU_W          "連接コスト重み"
#define         DEF_KEITAISO_W          "形態素コスト重み"
#define         DEF_COST_HABA           "コスト幅"
#define         DEF_DIC_FILE            "辞書ファイル"
#define         DEF_GRAM_FILE           "文法ファイル"

#define 	DEF_SUUSI_HINSI		"名詞"
#define 	DEF_SUUSI_BUNRUI	"数詞"
#define 	DEF_KAKKO_HINSI		"特殊"
#define 	DEF_KAKKO_BUNRUI1	"括弧始"
#define 	DEF_KAKKO_BUNRUI2	"括弧終"
#define 	DEF_KUUHAKU_HINSI	"特殊"
#define 	DEF_KUUHAKU_BUNRUI	"空白"

#define         DEF_ONOMATOPOEIA_HINSI  "副詞"
#define         DEF_ONOMATOPOEIA_IMIS   "自動認識"
#define         DEF_RENDAKU_HINSI1      "動詞"
#define         DEF_RENDAKU_RENYOU      "基本連用形"
#define         DEF_RENDAKU_HINSI2      "名詞"
#define         DEF_RENDAKU_BUNRUI2_1   "普通名詞"
#define         DEF_RENDAKU_BUNRUI2_2   "サ変名詞"
#define         DEF_RENDAKU_HINSI3      "形容詞"
#define         DEF_RENDAKU_HINSI4      "接尾辞"
#define         DEF_RENDAKU_BUNRUI4_1   "名詞性述語接尾辞"
#define         DEF_RENDAKU_BUNRUI4_2   "名詞性名詞接尾辞"
#define         DEF_RENDAKU_BUNRUI4_3   "名詞性名詞助数辞"
#define         DEF_RENDAKU_BUNRUI4_4   "名詞性特殊接尾辞"
#define         DEF_RENDAKU_FEATURE     "連濁可"
#define         DEF_RENDAKU_MIDASI_KA   "か"
#define         DEF_RENDAKU_REP         "代表表記"
#define         DEF_RENDAKU_IMIS        "濁音化"
#define         DEF_LOWERCASE_IMIS      "小文字化"

#define 	DEF_UNDEF		"未定義語"
#define 	DEF_UNDEF_KATA		"カタカナ"
#define 	DEF_UNDEF_ALPH		"アルファベット"
#define 	DEF_UNDEF_ETC		"その他"

/*
------------------------------------------------------------------------------
	GLOBAL:
	definition of global variables          >>> changed by T.Nakamura <<<
------------------------------------------------------------------------------
*/

char			*ProgName;
FILE			*db;
extern CLASS		Class[CLASSIFY_NO + 1][CLASSIFY_NO + 1];
extern TYPE		Type[TYPE_NO];
extern FORM		Form[TYPE_NO][FORM_NO];
extern RENSETU_PAIR     *rensetu_tbl;
extern U_CHAR           *rensetu_mtr;

DIC_FILES               DicFile;

COST_OMOMI       	cost_omomi;    
char             	Jumangram_Dirname[FILENAME_MAX]; 
extern FILE		*Jumanrc_Fileptr;
int              	LineNo;
int     		LineNoForError;       /* k.n */

char			pat_buffer[50000];

/*
------------------------------------------------------------------------------
	LOCAL:
	definition of global variables          >>> changed by T.Nakamura <<<
------------------------------------------------------------------------------
*/
int             Show_Opt1;
int             Show_Opt2;
char		Show_Opt_tag[MIDASI_MAX];
int		Show_Opt_jumanrc;
int		Show_Opt_debug;
int		Vocalize_Opt;
int		Repetition_Opt;
int             Onomatopoeia_Opt;
int		Normalized_Opt;

U_CHAR	        String[LENMAX];
U_CHAR	        NormalizedString[LENMAX];
int		Unkword_Pat_Num;
int             m_buffer_num;
int             Jiritsu_buffer[CLASSIFY_NO + 1];
int             undef_hinsi;
int		undef_kata_bunrui, undef_alph_bunrui, undef_etc_bunrui;
int		undef_kata_con_tbl, undef_alph_con_tbl, undef_etc_con_tbl;
int             suusi_hinsi, suusi_bunrui;
int             kakko_hinsi, kakko_bunrui1, kakko_bunrui2;
int		kuuhaku_hinsi, kuuhaku_bunrui, kuuhaku_con_tbl;
int		onomatopoeia_hinsi, onomatopoeia_bunrui, onomatopoeia_con_tbl;
int             rendaku_hinsi1, rendaku_hinsi2, rendaku_hinsi3, rendaku_hinsi4;
int             rendaku_renyou, rendaku_bunrui2_1, rendaku_bunrui2_2;
int             rendaku_bunrui4_1, rendaku_bunrui4_2, rendaku_bunrui4_3, rendaku_bunrui4_4;
int             jiritsu_num;
int             p_buffer_num;
CONNECT_COST	connect_cache[CONNECT_MATRIX_MAX];

/* MRPH_BUFFER_MAX の制限を撤廃，動的にメモリ確保 */
int		 mrph_buffer_max = 0;
MRPH *           m_buffer;
int *            m_check_buffer;

/* PROCESS_BUFFER_MAX の制限を撤廃，動的にメモリ確保 */
int		 process_buffer_max = 0;
PROCESS_BUFFER * p_buffer;
int *            path_buffer;
int *		 match_pbuf;

U_CHAR		kigou[MIDASI_MAX];   /* 先頭の記号(@) */
U_CHAR		midasi1[MIDASI_MAX]; /* 活用 */
U_CHAR		midasi2[MIDASI_MAX]; /* 原形 */
U_CHAR		yomi[MIDASI_MAX];    /* 活用の読み */

extern COST_OMOMI       cost_omomi;     /*k.n*/
extern char             Jumangram_Dirname[FILENAME_MAX];  /*k.n*/

/*
------------------------------------------------------------------------------
	prototype definition of functions       >>> changed by T.Nakamura <<<
------------------------------------------------------------------------------
*/

BOOL	juman_init_rc(FILE *fp);
int	juman_close(void);
void	realloc_mrph_buffer(void);
void	realloc_process_buffer(void);
void    read_class_cost(CELL *cell); /* k.n */
static BOOL    katuyou_process(int position, int *k, MRPH mrph, int *length, char opt);
int     search_all(int position);
int     take_data(int pos, char **pbuf, char opt);
char *	_take_data(char *s, MRPH *mrph, char opt);
int 	numeral_decode(char **str);
int 	numeral_decode2(char **str);
void 	hiragana_decode(char **str, char *yomi);
void    check_rc(void);
void    changeDictionary(int number);

int     trim_space(int pos);
int	undef_word(int pos);
int	check_code(U_CHAR *cp, int position);
void 	juman_init_etc(void);
int 	suusi_word(int pos , int m_num);
int	through_word(int pos , int m_num);
int 	is_through(MRPH *mrph_p);

/*
  結果の出力関数については、サーバーモード対応のため、出力先を引数として取る
  ように変更
  NACSIS 吉岡
*/   
void	print_path_mrph(FILE* output, int path_num , int para_flag);
char	**print_best_path(FILE* output);
char	**print_all_mrph(FILE* output);
void	_print_all_mrph(FILE* output, int path_num);
char	**print_all_path(FILE* output);
void	_print_all_path(FILE* output, int path_num, int pathes);
char	**print_homograph_path(FILE* output);
int	_print_homograph_path(FILE* output, int pbuf_start, int new_p);

int	pos_match_process(int pos, int p_start);
int	pos_right_process(int position);
int	check_connect(int pos_start, int m_num, char opt);
int	juman_sent(void);

/*
------------------------------------------------------------------------------
	PROCEDURE: <changeDictionary>          >>> changed by T.Nakamura <<<
------------------------------------------------------------------------------
*/

void changeDictionary(int number)
{
    db = DicFile.dic[number];
    DicFile.now = number;
}

/*
------------------------------------------------------------------------------
	PROCEDURE: <juman_init_rc>
------------------------------------------------------------------------------
*/

BOOL juman_init_rc(FILE *fp)
{
    int  num, win32_decided = 0;
    char dic_file_name[BUFSIZE], full_file_name[BUFSIZE];
    CELL *cell1, *cell2;
    
    LineNo = 0 ;
    
    /* DEFAULT VALUE*/
    
    cost_omomi.keitaiso = KEITAISO_WEIGHT_DEFAULT;
    cost_omomi.rensetsu = RENSETSU_WEIGHT_DEFAULT * MRPH_DEFAULT_WEIGHT;
    cost_omomi.cost_haba = COST_WIDTH_DEFAULT * MRPH_DEFAULT_WEIGHT;

    /* 
     *  MS Windows は辞書のパスを,juman.ini から取得する (辞書Dir も1つのみ (やや手抜))
     *  しかも gramfile == dicfile
     *  Changed by Taku Kudoh (taku@pine.kuee.kyoto-u.ac.jp)
     */
#ifdef _WIN32
    /* 文法ファイル */
    num = 0;
    GetPrivateProfileString("juman","dicfile","",Jumangram_Dirname,sizeof(Jumangram_Dirname),"juman.ini");
    if (Jumangram_Dirname[0]) {
	grammar(NULL);
	katuyou(NULL);
	connect_table(NULL);
	connect_matrix(NULL);

	/* 辞書ファイル */
	GetPrivateProfileString("juman","dicfile","",dic_file_name,sizeof(dic_file_name),"juman.ini");
	if ((endchar(dic_file_name)) != '\\')
	    strcat(dic_file_name, "\\");
    
	sprintf(full_file_name, "%s%s", dic_file_name, PATFILE);
	strcat(dic_file_name, DICFILE);
	DicFile.dic[num] = my_fopen(dic_file_name , "rb");
	pat_init_tree_top(&DicFile.tree_top[0]);
	com_l(full_file_name, &DicFile.tree_top[0]);
	DicFile.number = 1;
	changeDictionary(0);
	win32_decided = 1;
    }
    /* juman.iniが利用できなければ、jumanrcから読む */
#endif
    
    while (!s_feof(fp)) { 
	LineNoForError = LineNo ;
	cell1 = s_read(fp);

	/* 文法ファイル */
	if (!win32_decided && !strcmp(DEF_GRAM_FILE, _Atom(car(cell1)))) {
	    if (!Atomp(cell2 = car(cdr(cell1)))) {
		return FALSE;
	    } else {
		strcpy(Jumangram_Dirname , _Atom(cell2));
		grammar(NULL);
		katuyou(NULL);
		connect_table(NULL);
		connect_matrix(NULL);
	    }
	}
	  
	/* 辞書ファイル */
	else if (!win32_decided && !strcmp(DEF_DIC_FILE, _Atom(car(cell1)))) {
	    cell2 = cdr(cell1) ;
	    for(num = 0 ; ; num++) {
		if( Null(car(cell2)) )  break;
		else if ( !Atomp( car(cell2) ) ) {
		    return FALSE;
		}
		else if(num >= MAX_DIC_NUMBER)
		    error(ConfigError, "Too many dictionary files.", EOA);
		else {
		    strcpy(dic_file_name, _Atom(car(cell2)));
		    if ((endchar(dic_file_name)) != '/')
			strcat(dic_file_name, "/");
		    cell2 = cdr(cell2);
		      
		    /* 辞書のオープン */
		    sprintf(full_file_name, "%s%s", dic_file_name, PATFILE);
		    strcat(dic_file_name, DICFILE);
		    DicFile.dic[num] = my_fopen(dic_file_name , "r");
		    pat_init_tree_top(&DicFile.tree_top[num]);
		    com_l(full_file_name, &DicFile.tree_top[num]);
		}
	    }
	    DicFile.number = num;
	    changeDictionary(0);
	}
	/* 連接コスト重み */
	else if (!strcmp(DEF_RENSETSU_W, _Atom(car(cell1)))) { 
	    if (!Atomp(cell2 = car(cdr(cell1)))) {
		return FALSE;
	    } else 
		cost_omomi.rensetsu = 
		    (int) atoi(_Atom(cell2)) * MRPH_DEFAULT_WEIGHT;
	}
	  
	/* 形態素コスト重み */
	else if (!strcmp(DEF_KEITAISO_W, _Atom(car(cell1)))) { 
	    if (!Atomp(cell2 = car(cdr(cell1)))) {
		return FALSE;
	    } else 
		cost_omomi.keitaiso = (int) atoi(_Atom(cell2));
	}
	  
	/* コスト幅 */
	else if (!strcmp(DEF_COST_HABA, _Atom(car(cell1)))) { 
	    if (!Atomp(cell2 = car(cdr(cell1)))) {
		return FALSE;
	    } else 
		cost_omomi.cost_haba = 
		    (int) atoi(_Atom(cell2)) * MRPH_DEFAULT_WEIGHT;
	}
	  
	/* 品詞コスト*/
	else if (!strcmp(DEF_CLASS_C, _Atom(car(cell1)))) { 
	    read_class_cost(cdr(cell1));
	}

	/* 未定義語コスト (3.4以降不要)
	else if (!strcmp("未定義語品詞", _Atom(car(cell1))));
	*/
    }
    return TRUE;
}

/*
------------------------------------------------------------------------------
	PROCEDURE: <compile_patterns>
------------------------------------------------------------------------------
*/

#ifdef HAVE_REGEX_H
int compile_unkword_patterns() {
    int i, j, flag;

    /* malloc m_pattern */
    for (i = 0; *mrph_pattern[i]; i++);    
    m_pattern = (MRPH_PATTERN *)(malloc(sizeof(MRPH_PATTERN) * i));

    /* make m_pattern[i].preg */
    for (i = 0; *mrph_pattern[i]; i++) {

	/* read mrph_pattern */
	flag = 0;
	m_pattern[i].weight = DefaultWeight;
	sprintf(m_pattern[i].regex, "^");
	for (j = 0; *(mrph_pattern[i] + j); j += 2) {
	    if ((mrph_pattern[i] + j)[0] == ' ' ||
		(mrph_pattern[i] + j)[0] == '\t') {
		flag = 1;
		j -= 1;
		continue;
	    }

	    /* read weight */
	    if (flag) {
		m_pattern[i].weight = atof(mrph_pattern[i] + j);
		break;
	    }

	    /* read pattern */
	    if (strlen(m_pattern[i].regex) >= PATTERN_MAX - 3) {
		printf("too long pattern: \"%s\"\n", mrph_pattern[i]);
		exit(1);
	    }
	    if (!strncmp(mrph_pattern[i] + j, Hkey, 2)) {
		strcat(m_pattern[i].regex, Hcode);
	    }
	    else if (!strncmp(mrph_pattern[i] + j, Kkey, 2)) {
		strcat(m_pattern[i].regex, Kcode);
	    }
	    else if (!strncmp(mrph_pattern[i] + j, Ykey, 2)) {
		strcat(m_pattern[i].regex, Ycode);
	    }
	    else {
		strncat(m_pattern[i].regex, mrph_pattern[i] + j, 2);
	    }
	}

	/* compile mrph_pattern */
	if (regcomp(&(m_pattern[i].preg), m_pattern[i].regex, REG_EXTENDED) != 0) {
	    printf("regex compile failed\n");
	}
    }
    return i;
}
#endif

/*
------------------------------------------------------------------------------
        PROCEDURE: <juman_close>             >>> changed by T.Nakamura <<<
------------------------------------------------------------------------------
*/
BOOL juman_close(void)
{
  int i;

  for(i = 0 ; i < DicFile.number ; i++)
    fclose(DicFile.dic[i]);

  free(rensetu_tbl);
  free(rensetu_mtr);

  return TRUE;
}

/*
------------------------------------------------------------------------------
        PROCEDURE: <realloc_mrph_buffer>
------------------------------------------------------------------------------
*/
void	realloc_mrph_buffer(void)
{
    mrph_buffer_max += BUFFER_BLOCK_SIZE;
    m_buffer = (MRPH *)my_realloc(m_buffer, sizeof(MRPH)*mrph_buffer_max);
    m_check_buffer = (int *)my_realloc(m_check_buffer,
				       sizeof(int)*mrph_buffer_max);
}

/*
------------------------------------------------------------------------------
        PROCEDURE: <realloc_process_buffer>
------------------------------------------------------------------------------
*/
void	realloc_process_buffer(void)
{
    process_buffer_max += BUFFER_BLOCK_SIZE;
    p_buffer = (PROCESS_BUFFER *)my_realloc(p_buffer,
				    sizeof(PROCESS_BUFFER)*process_buffer_max);
    path_buffer = (int *)my_realloc(path_buffer,
				       sizeof(int)*process_buffer_max);
    match_pbuf = (int *)my_realloc(match_pbuf, 
				       sizeof(int)*process_buffer_max);
}

/*
------------------------------------------------------------------------------
        PROCEDURE: <read_class_cost>
------------------------------------------------------------------------------
*/
void read_class_cost(CELL *cell)
{
    /* 品詞コストの読み込み (後ろの設定を優先) */

    CELL *pos_cell;
    int hinsi, bunrui, cost;

    while (!Null(car(cell))) {

	pos_cell = car(car(cell)) ;
	cost = (int) atoi(_Atom(car(cdr(car(cell)))));

	if (!strcmp(_Atom(car(pos_cell)), "*")) {
	    /* 品詞が * なら全体に */
	    for (hinsi = 1 ; Class[hinsi][0].id; hinsi++)
		for (bunrui = 0 ; Class[hinsi][bunrui].id ; bunrui++)
		    Class[hinsi][bunrui].cost = cost;
	}
	else {
	    hinsi = get_hinsi_id(_Atom(car(pos_cell)));
	    if (Null(car(cdr(pos_cell))) || 
		!strcmp(_Atom(car(cdr(pos_cell))), "*")) {
		/* 細分類が * または無しなら品詞全体に */
		for (bunrui = 0; Class[hinsi][bunrui].id ; bunrui++)
		    Class[hinsi][bunrui].cost = cost;
	    }
	    else {
		/* 品詞，細分類ともに指定された場合 */
		bunrui = get_bunrui_id(_Atom(car(cdr(pos_cell))), hinsi);
		Class[hinsi][bunrui].cost = cost;
	    }
	}
	cell = cdr(cell);		
    }

    /* default */
  
    for (hinsi = 1; Class[hinsi][0].id; hinsi++) 
	for (bunrui = 0; Class[hinsi][bunrui].id; bunrui++)
	    if (Class[hinsi][bunrui].cost == 0) 
		Class[hinsi][bunrui].cost = CLASS_COST_DEFAULT;
  
    /* For 文頭 文末 added by S.Kurohashi */

    Class[0][0].cost = 0;
}

/*
------------------------------------------------------------------------------
	PROCEDURE: <katuyou_process>       >>> changed by T.Nakamura <<<
------------------------------------------------------------------------------
*/
static BOOL katuyou_process(int position, int *k, MRPH mrph, int *length, char opt)
{
     while (Form[mrph.katuyou1][*k].name) {
	  if (compare_top_str1(Form[mrph.katuyou1][*k].gobi,
			      String + position + mrph.length) ||
	      (opt & OPT_NORMALIZE) && /* 非正規表現用 */
	      compare_top_str1(Form[mrph.katuyou1][*k].gobi,
			       NormalizedString + position + mrph.length)) {
	       *length     = mrph.length + 
		    strlen(Form[mrph.katuyou1][*k].gobi);
	       return TRUE;
   	  } else {
	       (*k)++;
	  }
     }
     return FALSE;
}

/*
------------------------------------------------------------------------------
	PROCEDURE: <search_all>       >>> changed by T.Nakamura <<<
------------------------------------------------------------------------------
*/
int search_all(int position)
{
    MRPH        mrph;
    int         dic_no;
    int         jmp ;
    int		i;
    char	*pbuf;
    U_CHAR      buf[LENMAX];

    for(dic_no = 0 ; dic_no < DicFile.number ; dic_no++) {
	changeDictionary(dic_no);

	/* パトリシア木から形態素を検索 */
	pat_buffer[0] = '\0';
	pat_search(db, String + position, &DicFile.tree_top[dic_no], pat_buffer);
	pbuf = pat_buffer;
	while (*pbuf != '\0') {
	    if (take_data(position, &pbuf, 0) == FALSE) return FALSE;
	}

	if (Normalized_Opt && /* 非正規表現用の検索 */
	    strncmp(String + position, NormalizedString + position, NORMALIZED_LENGTH * 2)) {
	    pat_buffer[0] = '\0';
	    pat_search(db, NormalizedString + position, &DicFile.tree_top[dic_no], pat_buffer);
	    pbuf = pat_buffer;
	    while (*pbuf != '\0') {
		if (take_data(position, &pbuf, OPT_NORMALIZE) == FALSE) return FALSE;
	    }
	}

	if (Vocalize_Opt) { /* 濁音化した形態素の検索 */
	    pat_buffer[0] = '\0';
	    for (i = VOICED_CONSONANT_S; i < VOICED_CONSONANT_E; i++) {
		if (position != 0 && !strncmp(String + position, dakuon[i], 2) &&
		    check_code(String, position) == check_code(String, position + 2)) {
		    sprintf(buf, "%s%s", seion[i], String + position + 2);
		    pat_search(db, buf, &DicFile.tree_top[dic_no], pat_buffer);
		    pbuf = pat_buffer;
		    while (*pbuf != '\0') {
			if (take_data(position, &pbuf, OPT_DEVOICE) == FALSE) return FALSE;
		    }
		    break;
		}
	    }
	}
    }

    return TRUE;
}

/*
------------------------------------------------------------------------------
        PROCEDURE: <recognize_repetition>   >>> Added by Ryohei Sasano <<<
------------------------------------------------------------------------------
*/
int recognize_onomatopoeia(int pos)
{
    int i, len, code, next_code;
    int key_length = strlen(String + pos); /* キーの文字数を数えておく */
#ifdef HAVE_REGEX_H
    regmatch_t pmatch[1];
#endif

    /* 通常の平仮名、片仮名以外から始まるものは不可 */
    code = check_code(String, pos);
    if (code != HIRAGANA && code != KATAKANA) return FALSE;
    for (i = 0; *lowercase[i]; i++) {
	if (!strncmp(String + pos, lowercase[i], 2)) return FALSE;
    }

#ifdef HAVE_REGEX_H
    /* 非反復型オノマトペ */
    if (Onomatopoeia_Opt) {
	for (i = 0; i < Unkword_Pat_Num; i++) {
	    
	    /* マッチング */
	    if (regexec(&(m_pattern[i].preg), String + pos, 1, pmatch, 0) == 0) {
		
		m_buffer[m_buffer_num].hinsi = onomatopoeia_hinsi;
		m_buffer[m_buffer_num].bunrui = onomatopoeia_bunrui;
		m_buffer[m_buffer_num].con_tbl = onomatopoeia_con_tbl;
		
		m_buffer[m_buffer_num].katuyou1 = 0;
		m_buffer[m_buffer_num].katuyou2 = 0;
		m_buffer[m_buffer_num].length = (int)(pmatch[0].rm_eo - pmatch[0].rm_so);    
		
		strncpy(m_buffer[m_buffer_num].midasi, String+pos, m_buffer[m_buffer_num].length);
		m_buffer[m_buffer_num].midasi[m_buffer[m_buffer_num].length] = '\0';
		strncpy(m_buffer[m_buffer_num].yomi, String+pos, m_buffer[m_buffer_num].length);
		m_buffer[m_buffer_num].yomi[m_buffer[m_buffer_num].length] = '\0';
		
		m_buffer[m_buffer_num].weight = m_pattern[i].weight;
		
		strcpy(m_buffer[m_buffer_num].midasi2, m_buffer[m_buffer_num].midasi);
		strcpy(m_buffer[m_buffer_num].imis, "\"");
		strcat(m_buffer[m_buffer_num].imis, DEF_ONOMATOPOEIA_IMIS);
		strcat(m_buffer[m_buffer_num].imis, "\"");
		
		check_connect(pos, m_buffer_num, 0);
		if (++m_buffer_num == mrph_buffer_max) realloc_mrph_buffer();	
		break; /* 最初にマッチしたもののみ採用 */
	    }
	}
    }
#endif

    /* 反復型オノマトペ */
    if (Repetition_Opt) {
	for (len = 2; len < 5; len++) {
	    /* 途中で文字種が変わるものは不可 */
	    next_code = check_code(String, pos + len * 2 - 2);
	    if (next_code == CHOON) next_code = code; /* 長音は直前の文字種として扱う */
	    if (key_length < len * 4 || code != next_code) break;
	    code = next_code;
	    
	    /* 反復があるか判定 */
	    if (strncmp(String + pos, String + pos + len * 2, len * 2)) continue;
	    
	    m_buffer[m_buffer_num].hinsi = onomatopoeia_hinsi;
	    m_buffer[m_buffer_num].bunrui = onomatopoeia_bunrui;
	    m_buffer[m_buffer_num].con_tbl = onomatopoeia_con_tbl;
	    
	    m_buffer[m_buffer_num].katuyou1 = 0;
	    m_buffer[m_buffer_num].katuyou2 = 0;
	    m_buffer[m_buffer_num].length = len * 4;
	    
	    strncpy(m_buffer[m_buffer_num].midasi, String+pos, len * 4);
	    m_buffer[m_buffer_num].midasi[len * 4] = '\0';
	    strncpy(m_buffer[m_buffer_num].yomi, String+pos, len * 4);
	    m_buffer[m_buffer_num].yomi[len * 4] = '\0';
	    
	    /* weightの設定 */
	    m_buffer[m_buffer_num].weight = REPETITION_COST * len;
	    /* 拗音を含む場合 */
	    for (i = CONTRACTED_LOWERCASE_S; i < CONTRACTED_LOWERCASE_E; i++) {
		if (strstr(m_buffer[m_buffer_num].midasi, lowercase[i])) break;
	    }
	    if (i < CONTRACTED_LOWERCASE_E) {
		if (len == 2) continue; /* 1音の繰り返しは禁止 */		
		/* 1文字分マイナス+ボーナス */
		m_buffer[m_buffer_num].weight -= REPETITION_COST + CONTRACTED_BONUS;
	    }
	    /* 濁音・半濁音を含む場合 */
	    for (i = 0; *dakuon[i]; i++) {
		if (strstr(m_buffer[m_buffer_num].midasi, dakuon[i])) break;
	    }
	    if (*dakuon[i]) {
		m_buffer[m_buffer_num].weight -= DAKUON_BONUS; /* ボーナス */
		/* 先頭が濁音の場合はさらにボーナス */
		if (!strncmp(m_buffer[m_buffer_num].midasi, dakuon[i], 2)) 
		    m_buffer[m_buffer_num].weight -= DAKUON_BONUS;
	    }
	    /* カタカナである場合 */
	    if (code == KATAKANA) 
		m_buffer[m_buffer_num].weight -= KATAKANA_BONUS;
	    
	    strcpy(m_buffer[m_buffer_num].midasi2, m_buffer[m_buffer_num].midasi);
	    strcpy(m_buffer[m_buffer_num].imis, "\"");
	    strcat(m_buffer[m_buffer_num].imis, DEF_ONOMATOPOEIA_IMIS);
	    strcat(m_buffer[m_buffer_num].imis, "\"");
	    
	    check_connect(pos, m_buffer_num, 0);
	    if (++m_buffer_num == mrph_buffer_max) realloc_mrph_buffer();	
	    break; /* 最初にマッチしたもののみ採用 */
	}
    }
}

/*
------------------------------------------------------------------------------
        PROCEDURE: <take_data>                  >>> Changed by yamaji <<<
------------------------------------------------------------------------------
*/
int take_data(int pos, char **pbuf, char opt)
{
    unsigned char    *s;
    int     i, k, f, num;
    int	    rengo_con_tbl, rengo_weight;
    MRPH    mrph;
    MRPH    *new_mrph;
    int	    length, con_tbl_bak, k2, pnum_bak;
    int	    new_mrph_num;

    s = *pbuf;

    k = 0 ;

    while ((mrph.midasi[k++] = *(s++)) != '\t') {}
    mrph.midasi[k-1] = '\0';
    mrph.midasi2[0] = '\0';

    if (*s == 0xff) { /* 連語情報だった場合 */

#define 	RENGO_BUFSIZE		20

	MRPH mrph_buf[RENGO_BUFSIZE];
	int add_list[FORM_NO];
	int co, pos_bak, glen, cno;
	
	pos_bak = pos;

	s = _take_data(s, &mrph, opt);
	rengo_con_tbl = mrph.con_tbl;
	rengo_weight  = mrph.weight;
	num = mrph.bunrui;

	for (i = 0; i < num; i++) { /* まず，連語全体をバッファに読み込む */
	    new_mrph = &mrph_buf[i];

	    k = 0;
	    while ((new_mrph->midasi[k++] = *(s++)) != ' ') {}
	    new_mrph->midasi[k-1] = '\0';
	    new_mrph->midasi2[0] = '\0';

	    s = _take_data(s, new_mrph, opt);
	    if (opt) new_mrph->weight = STOP_MRPH_WEIGHT;

	    length = strlen(new_mrph->midasi);
	    if (Class[new_mrph->hinsi][new_mrph->bunrui].kt) /* 活用する */
		if (i < num-1) { /* 末尾以外の活用語 */
		    length += strlen(
			Form[new_mrph->katuyou1][new_mrph->katuyou2].gobi);
		    new_mrph->con_tbl += (new_mrph->katuyou2-1);
		}
	    new_mrph->length = length;

	    if (i < num-1) pos += length; /* 末尾形態素の位置を保存 */
/*printf(".....%d/%d   %s %d\n", i, num, new_mrph->midasi, new_mrph->con_tbl);*/
	}

	new_mrph = &mrph_buf[num-1];
	if (Class[new_mrph->hinsi][new_mrph->bunrui].kt == 0) {
	    add_list[0] = 0;
	    add_list[1] = -1;
	} else {
	    if (new_mrph->katuyou2) {
		add_list[0] = new_mrph->katuyou2;
		add_list[1] = -1;
	    } else {
		/* 末尾の形態素が活用する場合 */
		k2 = strlen(new_mrph->midasi) ? 1 : 2;
		co = 0;
		while (katuyou_process(pos, &k2, *new_mrph, &length, opt)) {
		    add_list[co++] = k2;
		    k2++;
		}
		add_list[co] = -1;
	    }
	}

	/* 活用形ごとに連語を追加していく */
	for (co = 0; add_list[co] >= 0; co++) {
	    pos = pos_bak;
	    for (i = 0; i < num; i++) {
		m_buffer[m_buffer_num] = mrph_buf[i];
		new_mrph = &m_buffer[m_buffer_num];

		if (i == 0) { /* 連語の先頭の形態素 */
		    con_tbl_bak = new_mrph->con_tbl;
/*printf("FROM %d %d\n", new_mrph->con_tbl, rengo_con_tbl);*/
		    if (rengo_con_tbl != -1) {
			if (add_list[co]) cno = add_list[co]-1; else cno = 0;
			if (check_matrix_left(rengo_con_tbl+cno) == TRUE)
			    /* 連語特有の左連接規則 */
			    new_mrph->con_tbl = rengo_con_tbl+cno;
		    }
/*printf("  TO %d\n", new_mrph->con_tbl);*/
		    pnum_bak = p_buffer_num;

		    if (Show_Opt_debug) {
			printf("\\begin{形態素→連語} %s", mrph.midasi);
			if (add_list[co])
			    printf("%s",
				   Form[mrph_buf[num-1].katuyou1][add_list[co]].gobi);
			printf("\n");
		    }
		    
		    check_connect(pos, m_buffer_num, opt);
		    if (p_buffer_num == pnum_bak) break;

		    p_buffer[pnum_bak].end = pos + new_mrph->length;
		    p_buffer[pnum_bak].connect = FALSE;
		    p_buffer[pnum_bak].score = p_buffer[pnum_bak].score +
			(Class[new_mrph->hinsi][new_mrph->bunrui].cost
			 * new_mrph->weight * cost_omomi.keitaiso 
			 * (rengo_weight-10)/10);
		    new_mrph->con_tbl = con_tbl_bak;

		    if (Show_Opt_debug) {
			printf("----- 連語内 %s %d\n", new_mrph->midasi, 
			       p_buffer[pnum_bak].score);
		    }
		} else {
		    p_buffer[p_buffer_num].score =
			p_buffer[p_buffer_num-1].score +
			(Class[new_mrph->hinsi][new_mrph->bunrui].cost
			 * new_mrph->weight * cost_omomi.keitaiso +
			 (check_matrix(m_buffer[p_buffer[p_buffer_num-1].mrph_p].con_tbl, new_mrph->con_tbl) ? 
			  check_matrix(m_buffer[p_buffer[p_buffer_num-1].mrph_p].con_tbl, new_mrph->con_tbl) : DEFAULT_C_WEIGHT)
			 /* 連語内の接続は接続表で定義されていない場合がある
			    その場合は，接続のコストはデフォルトの値とする． */
			 * cost_omomi.rensetsu)
			 * rengo_weight/10;
		    p_buffer[p_buffer_num].mrph_p = m_buffer_num;
		    p_buffer[p_buffer_num].start = pos;
		    p_buffer[p_buffer_num].end = pos + new_mrph->length;
		    p_buffer[p_buffer_num].path[0] = p_buffer_num-1;
		    p_buffer[p_buffer_num].path[1] = -1;

		    if (i < num-1) { /* 連語の中程の形態素 */
			p_buffer[p_buffer_num].connect = FALSE;
		    } else { /* 連語の末尾の形態素 */
			p_buffer[p_buffer_num].connect = TRUE;

			if (rengo_con_tbl != -1) {
			    if (add_list[co]) cno = add_list[co]-1;
			    else cno = 0;
			    if (check_matrix_right(rengo_con_tbl+cno) == TRUE)
				/* 連語特有の右連接規則 */
				new_mrph->con_tbl = rengo_con_tbl;
			}
			if (add_list[co]) {
			    new_mrph->katuyou2 = add_list[co];
			    new_mrph->con_tbl += (add_list[co]-1);
			    glen = strlen(Form[new_mrph->katuyou1]
					  [new_mrph->katuyou2].gobi);
			    new_mrph->length += glen;
			    p_buffer[p_buffer_num].end += glen;
			}
		    }
		    if (Show_Opt_debug) {
			printf("----- 連語内 %s %d\n", new_mrph->midasi, 
			       p_buffer[p_buffer_num].score);
		    }

		    if (++p_buffer_num == process_buffer_max)
			realloc_process_buffer();
		}
		pos += new_mrph->length;

		/* fixed by kuro on 01/01/19
		   以下がreallocが上のif-elseの中だたったので，new_mrphへの
		   アクセスで segmentation fault となっていた */
		if (++m_buffer_num == mrph_buffer_max)
		  realloc_mrph_buffer();
	    }
	    if (Show_Opt_debug) {
		printf("\\end{形態素→連語}\n");
	    }
	}

    } else {           /* 普通の形態素だった場合 */
	s = _take_data(s, &mrph, opt);

	/* 重みがSTOP_MRPH_WEIGHTとなっているノードは生成しない */
	if (mrph.weight == STOP_MRPH_WEIGHT) {
	    *pbuf = s;
	    return TRUE;
	}

	if ( Class[mrph.hinsi][mrph.bunrui].kt ) { /* 活用する */
	    if ( mrph.katuyou2 == 0 ) {   /* 語幹あり */
		k2 = 1;
		while (katuyou_process(pos, &k2, mrph, &length, opt)) {
		    /* 濁音化ノードは2字以上の場合のみ、 */
		    /* 正規化ノードは2字以上、かつ、length内に小文字を含む場合のみ作成 */
		    if ((opt & OPT_DEVOICE) && length == 2 ||
			(opt & OPT_NORMALIZE) &&
			(length == 2 || !strncmp(String + pos, NormalizedString + pos, length))) {
			k2++;
			continue;
		    }
		    m_buffer[m_buffer_num] = mrph;
		    m_buffer[m_buffer_num].katuyou2 = k2;
		    m_buffer[m_buffer_num].length = length;
		    m_buffer[m_buffer_num].con_tbl += (k2 - 1);
		    check_connect(pos, m_buffer_num, opt);
		    if (++m_buffer_num == mrph_buffer_max)
			realloc_mrph_buffer();
		    k2++;
		}
	    } else {                         /* 語幹なし */
		m_buffer[m_buffer_num] = mrph;
		m_buffer[m_buffer_num].midasi[0] = '\0';
		m_buffer[m_buffer_num].midasi2[0] = '\0';
		m_buffer[m_buffer_num].yomi[0]  = '\0';
		check_connect(pos, m_buffer_num, opt);
		if (++m_buffer_num == mrph_buffer_max)
		    realloc_mrph_buffer();
	    }
	} else {	                                 /* 活用しない */
	    if (!(opt & OPT_NORMALIZE) ||
		/* 正規化ノードは2字以上、かつ、length内に小文字を含む場合のみ作成 */
		strlen(mrph.midasi) > 2 && 
		strncmp(String + pos, NormalizedString + pos, strlen(mrph.midasi))) {
		
		m_buffer[m_buffer_num] = mrph;
		check_connect(pos, m_buffer_num, opt);
		new_mrph_num = m_buffer_num;
		if (++m_buffer_num == mrph_buffer_max)
		    realloc_mrph_buffer();	    

#ifdef NUMERIC_P
		if (suusi_word(pos, new_mrph_num) == FALSE)
		    return FALSE;
#endif
#ifdef THROUGH_P
		if (through_word(pos, new_mrph_num) == FALSE)
		    return FALSE;
#endif
	    }
	}
    }
    *pbuf = s;
    return TRUE;
}

/*
------------------------------------------------------------------------------
        PROCEDURE: <_take_data>                 >>> added by T.Nakamura <<<
------------------------------------------------------------------------------
*/

char *_take_data(char *s, MRPH *mrph, char opt)
{
    int		i, j, k = 0;
    char	c, *rep;
    
    mrph->hinsi    = numeral_decode(&s);
    mrph->bunrui   = numeral_decode(&s);
    mrph->katuyou1 = numeral_decode(&s);
    mrph->katuyou2 = numeral_decode(&s);
    mrph->weight   = numeral_decode(&s);
    mrph->con_tbl  = numeral_decode2(&s);
    hiragana_decode(&s, mrph->yomi);
    mrph->length   = strlen(mrph->midasi);
    
    if (*s != ' ' && *s != '\n') { /* 意味情報あり */
	j = numeral_decode(&s);
	for (i = 0; i < j; i++) mrph->imis[k++] = *(s++);
	mrph->imis[k] = '\0';
    }
    s++;
    if (k == 0) strcpy(mrph->imis, NILSYMBOL);  
    
    if (opt & OPT_DEVOICE) {

	/* 濁音化の処理はしない形態素の重みをSTOP_MRPH_WEIGHTにする */
	if (mrph->katuyou2 || /* 語幹のない語 */
 	    mrph->length == 2 && !Class[mrph->hinsi][mrph->bunrui].kt || /* 1文字の形態素 */
	    (rep = strstr(mrph->imis, DEF_RENDAKU_REP)) && *(rep+8) == ':' && check_code(rep, 9) == KATAKANA) { /* 非和語 */
	    mrph->weight = STOP_MRPH_WEIGHT;
	}
	else {
	    if (mrph->hinsi == rendaku_hinsi1) { /* 動詞 */
		mrph->weight += strncmp(mrph->midasi, DEF_RENDAKU_MIDASI_KA, 2) ? VERB_VOICED_COST : VERB_GA_VOICED_COST;
	    }
	    else if (mrph->hinsi == rendaku_hinsi2 && (mrph->bunrui == rendaku_bunrui2_1 || mrph->bunrui == rendaku_bunrui2_2)) { /* 名詞 */
		mrph->weight += strncmp(mrph->midasi, DEF_RENDAKU_MIDASI_KA, 2) ? NOUN_VOICED_COST : NOUN_GA_VOICED_COST;
	    }
	    else if (mrph->hinsi == rendaku_hinsi3) { /* 形容詞 */
		mrph->weight += ADJECTIVE_VOICED_COST;
	    }
	    /* その他の品詞でも濁音可という意味素があれば解析できるようにする */
	    else if (strstr(mrph->imis, DEF_RENDAKU_FEATURE)) {
		mrph->weight += OTHER_VOICED_COST;
	    }
	    else {
		mrph->weight = STOP_MRPH_WEIGHT;
	    }
	}

	/* ライマンの法則に該当し濁音可という意味情報のない形態素の連濁は認めない */
	if (mrph->weight != STOP_MRPH_WEIGHT) {
	    for (i = VOICED_CONSONANT_S; i < VOICED_CONSONANT_E; i++) {
		if (strstr(mrph->midasi + 2, dakuon[i])) break;
	    }
	    if (i < VOICED_CONSONANT_E && !strstr(mrph->imis, DEF_RENDAKU_FEATURE)) 
		mrph->weight = STOP_MRPH_WEIGHT;
	}

	/* 読み、意味情報を修正 */
	if (mrph->weight != STOP_MRPH_WEIGHT) {
	    for (i = VOICED_CONSONANT_S; i < VOICED_CONSONANT_E; i++) {
		if (!strncmp(mrph->yomi, seion[i], 2)) {
		    strncpy(mrph->yomi, dakuon[(i/2)*2], 2);
		    break;
		}
	    }
	    
	    if (k == 0) {
		strcpy(mrph->imis, "\"");
	    }
	    else {
		mrph->imis[strlen(mrph->imis) - 1] = ' ';
	    }
	    strcat(mrph->imis, DEF_RENDAKU_IMIS);
	    strcat(mrph->imis, "\"");
	}
    }

    /* 正規化したものにペナルティ(小文字化された表現用) */
    if (opt & OPT_NORMALIZE) {
	mrph->weight += NORMALIZED_COST;	
	if (k == 0) {
	    strcpy(mrph->imis, "\"");
	}
	else {
	    mrph->imis[strlen(mrph->imis) - 1] = ' ';
	}
	strcat(mrph->imis, DEF_LOWERCASE_IMIS);
	strcat(mrph->imis, "\"");
    }
    
    return(s);
}

int numeral_decode(char **str)
{
  unsigned char *s;

    s = *str;
    if (*s < 0xf0) {
	*str = s+1;
	return(*s-0x20);
    } else if (*s == 0xff) {
	*str = s+1;
	return(atoi(RENGO_ID));
    } else {
	*str = s+2;
	return((*s-0xf0)*(0xf0-0x20)+*(s+1)-0x20);
    }
}

int numeral_decode2(char **str)
{
    unsigned char *s;

    s = *str;
    *str = s+2;
    return((*s-0x20)*(0x100-0x20)+*(s+1)-0x20-1);
}

void hiragana_decode(char **str, char *yomi)
{
    unsigned char *s;

    s = *str;
    while (*s != 0x20) {
	if (*s < 0x80) {
	    *(yomi++) = 0xa4;
	    *(yomi++) = *s-0x21+0xa0;
	    s++;
	} else {
	    *(yomi++) = *(s++);
	    *(yomi++) = *(s++);
	}
    }
    *str = s+1;
    *yomi = '\0';
}

/*
------------------------------------------------------------------------------
  PROCEDURE: <trim_space> スペースの削除 (透過処理導入後使用せず)
------------------------------------------------------------------------------
*/
int trim_space(int pos)
{
    while (1) {
 	/* 全角スペース */
	if (String[pos] == 0xa1 && String[pos+1] == 0xa1)
	  pos += 2;
	else
	  break;
    }
    return pos;
}
    
/*
------------------------------------------------------------------------------
  PROCEDURE: <undef_word> 未定義語処理
------------------------------------------------------------------------------
*/
    
int undef_word(int pos)
{
    int i, end, code, next_code;
    
    /* 字種による切りだし
       平仮名，漢字，半角空白 → 一文字
       半角(空白以外)，片仮名(「ー」も)，アルファベット(「．」も) → 連続 */

    code = check_code(String, pos);
    if (code == HIRAGANA || code == KANJI) {
	end = (pos + 2);
    } else if (code == KUUHAKU) {
	end = (pos + 1);
    } else {
	end = pos;
	while(1) {
	    /* MIDASI_MAXを越える未定義語は作成しない */
	    if (end - pos >= MIDASI_MAX - ((code == HANKAKU) ? 1 : 2)) break;

	    end += (code == HANKAKU) ? 1 : 2;
	    next_code = check_code(String, end);
	    if (next_code == code ||
		(code == KATAKANA && next_code == CHOON) ||
		(code == ALPH     && next_code == PRIOD)) continue;
	    else break;
	}
    }

    switch (code) {
    case KUUHAKU:
	m_buffer[m_buffer_num].hinsi = kuuhaku_hinsi;
	m_buffer[m_buffer_num].bunrui = kuuhaku_bunrui;
	m_buffer[m_buffer_num].con_tbl = kuuhaku_con_tbl;
	break;
    case KATAKANA:
	m_buffer[m_buffer_num].hinsi = undef_hinsi;
	m_buffer[m_buffer_num].bunrui = undef_kata_bunrui;
	m_buffer[m_buffer_num].con_tbl = undef_kata_con_tbl;
	break;
    case ALPH:
	m_buffer[m_buffer_num].hinsi = undef_hinsi;
	m_buffer[m_buffer_num].bunrui = undef_alph_bunrui;
	m_buffer[m_buffer_num].con_tbl = undef_alph_con_tbl;
	break;
    default:
	m_buffer[m_buffer_num].hinsi = undef_hinsi;
	m_buffer[m_buffer_num].bunrui = undef_etc_bunrui;
	m_buffer[m_buffer_num].con_tbl = undef_etc_con_tbl;
	break;
    }

    m_buffer[m_buffer_num].katuyou1 = 0;
    m_buffer[m_buffer_num].katuyou2 = 0;
    m_buffer[m_buffer_num].length = end - pos;
    if (end - pos >= MIDASI_MAX) {
	fprintf(stderr, "Too long undef_word<%s>\n", String);
	return FALSE;
    }
    if (code == KUUHAKU) {
	strcpy(m_buffer[m_buffer_num].midasi, "\\ ");
	strcpy(m_buffer[m_buffer_num].yomi, "\\ ");
    } else {
	strncpy(m_buffer[m_buffer_num].midasi, String+pos, end - pos);
	m_buffer[m_buffer_num].midasi[end - pos] = '\0';
	strncpy(m_buffer[m_buffer_num].yomi, String+pos, end - pos);
	m_buffer[m_buffer_num].yomi[end - pos] = '\0';
    }
    m_buffer[m_buffer_num].weight = MRPH_DEFAULT_WEIGHT;
    strcpy(m_buffer[m_buffer_num].midasi2, m_buffer[m_buffer_num].midasi);
    strcpy(m_buffer[m_buffer_num].imis, NILSYMBOL);

    check_connect(pos, m_buffer_num, 0);
    if (++m_buffer_num == mrph_buffer_max)
	realloc_mrph_buffer();

    /* オノマトペの自動認識 */
    if (Repetition_Opt || Onomatopoeia_Opt) {
	recognize_onomatopoeia(pos);
    }

#ifdef THROUGH_P				/* 半角スペース透過 */
    if (code == KUUHAKU) {
	return (through_word(pos, m_buffer_num-1));
    }
#endif

    return TRUE;
}

int check_code(U_CHAR *cp, int pos)
{
    int	code;
    
    if ( cp[pos] == '\0')			return 0;
    else if ( cp[pos] == KUUHAKU )		return KUUHAKU;
    else if ( cp[pos] < HANKAKU )		return HANKAKU;
    
    code = cp[pos]*256 + cp[pos+1];
    
    if ( code == PRIOD ) 			return PRIOD;
    else if ( code == CHOON ) 			return CHOON;
    else if ( code < KIGOU ) 			return KIGOU;
    else if ( code < SUJI ) 			return SUJI;
    else if ( code < ALPH )			return ALPH;
    else if ( code < HIRAGANA ) 		return HIRAGANA;
    else if ( code < KATAKANA ) 		return KATAKANA;
    else if ( code < GR ) 			return GR;
    else return KANJI;
}

/*
------------------------------------------------------------------------------
  PROCEDURE: <juman_init_etc> 未定義語処理，数詞処理，透過処理等の準備
------------------------------------------------------------------------------
*/
void juman_init_etc(void)
{
    int i, flag;

    /* 未定義語処理の準備 */
    undef_hinsi = get_hinsi_id(DEF_UNDEF);
    undef_kata_bunrui = get_bunrui_id(DEF_UNDEF_KATA, undef_hinsi);
    undef_alph_bunrui = get_bunrui_id(DEF_UNDEF_ALPH, undef_hinsi);
    undef_etc_bunrui = get_bunrui_id(DEF_UNDEF_ETC, undef_hinsi);
    undef_kata_con_tbl = check_table_for_undef(undef_hinsi, undef_kata_bunrui);
    undef_alph_con_tbl = check_table_for_undef(undef_hinsi, undef_alph_bunrui);
    undef_etc_con_tbl = check_table_for_undef(undef_hinsi, undef_etc_bunrui);

    /* 数詞処理の準備 */
    suusi_hinsi = get_hinsi_id(DEF_SUUSI_HINSI);
    suusi_bunrui = get_bunrui_id(DEF_SUUSI_BUNRUI, suusi_hinsi);

    /* 透過処理の準備 */
    kakko_hinsi = get_hinsi_id(DEF_KAKKO_HINSI);
    kakko_bunrui1 = get_bunrui_id(DEF_KAKKO_BUNRUI1, kakko_hinsi);
    kakko_bunrui2 = get_bunrui_id(DEF_KAKKO_BUNRUI2, kakko_hinsi);

    kuuhaku_hinsi = get_hinsi_id(DEF_KUUHAKU_HINSI);
    kuuhaku_bunrui = get_bunrui_id(DEF_KUUHAKU_BUNRUI, kuuhaku_hinsi);
    kuuhaku_con_tbl = check_table_for_undef(kuuhaku_hinsi, kuuhaku_bunrui);

    /* オノマトペ自動認識の準備 */
    onomatopoeia_hinsi = get_hinsi_id(DEF_ONOMATOPOEIA_HINSI);
    onomatopoeia_bunrui = 0;
    onomatopoeia_con_tbl = check_table_for_undef(onomatopoeia_hinsi, onomatopoeia_bunrui);

    /* 連濁処理 */
    rendaku_hinsi1 = get_hinsi_id(DEF_RENDAKU_HINSI1);
    rendaku_renyou = get_form_id(DEF_RENDAKU_RENYOU, 1); /* 母音動詞(type=1)の基本連用形のform_idを基本連用形の汎用idとみなす */
    rendaku_hinsi2 = get_hinsi_id(DEF_RENDAKU_HINSI2);
    rendaku_bunrui2_1 = get_bunrui_id(DEF_RENDAKU_BUNRUI2_1, rendaku_hinsi2);
    rendaku_bunrui2_2 = get_bunrui_id(DEF_RENDAKU_BUNRUI2_2, rendaku_hinsi2);
    rendaku_hinsi3 = get_hinsi_id(DEF_RENDAKU_HINSI3);
    rendaku_hinsi4 = get_hinsi_id(DEF_RENDAKU_HINSI4);
    rendaku_bunrui4_1 = get_bunrui_id(DEF_RENDAKU_BUNRUI4_1, rendaku_hinsi4);
    rendaku_bunrui4_2 = get_bunrui_id(DEF_RENDAKU_BUNRUI4_2, rendaku_hinsi4);
    rendaku_bunrui4_3 = get_bunrui_id(DEF_RENDAKU_BUNRUI4_3, rendaku_hinsi4);
    rendaku_bunrui4_4 = get_bunrui_id(DEF_RENDAKU_BUNRUI4_4, rendaku_hinsi4);
}

/*
------------------------------------------------------------------------------
  PROCEDURE: <suusi_word> 数詞処理
------------------------------------------------------------------------------
*/
    
int 	suusi_word(int pos , int m_num)
{
    int i, j;
    MRPH *new_mrph , *pre_mrph;

    new_mrph = &m_buffer[m_num];
    if (new_mrph->hinsi != suusi_hinsi || new_mrph->bunrui != suusi_bunrui)
        return TRUE;

    for (j = 0; (i = match_pbuf[j]) >= 0; j++) {
	pre_mrph = &m_buffer[p_buffer[i].mrph_p];
	if (pre_mrph->hinsi == suusi_hinsi &&
	    pre_mrph->bunrui == suusi_bunrui &&
	    check_matrix(pre_mrph->con_tbl, new_mrph->con_tbl) != 0) {

	    if (strlen(pre_mrph->midasi)+strlen(new_mrph->midasi) >= MIDASI_MAX
		|| strlen(pre_mrph->yomi)+strlen(new_mrph->yomi) >= YOMI_MAX) {
		/* MIDASI_MAX、YOMI_MAXを越える数詞は分割するように変更 08/01/15 */
		/* fprintf(stderr, "Too long suusi<%s>\n", String);
		   return FALSE; */
		return TRUE;
	    }
	    m_buffer[m_buffer_num] = *pre_mrph;
	    strcat(m_buffer[m_buffer_num].midasi , new_mrph->midasi);
	    strcat(m_buffer[m_buffer_num].yomi   , new_mrph->yomi);
	    m_buffer[m_buffer_num].length += strlen(new_mrph->midasi);
	    /* コストは後ろ側の方を継承 */
	    m_buffer[m_buffer_num].weight = new_mrph->weight;
	    m_buffer[m_buffer_num].con_tbl = new_mrph->con_tbl;

	    p_buffer[p_buffer_num] = p_buffer[i];
	    p_buffer[p_buffer_num].end = pos+strlen(new_mrph->midasi);
	    p_buffer[p_buffer_num].mrph_p = m_buffer_num;
	    p_buffer[p_buffer_num].score += (new_mrph->weight-pre_mrph->weight)
		*Class[new_mrph->hinsi][new_mrph->bunrui].cost
		*cost_omomi.keitaiso;

	    if (++m_buffer_num == mrph_buffer_max) {
		realloc_mrph_buffer();
	        new_mrph = &m_buffer[m_num];	/* fixed by kuro 99/09/01 */
	    }
	    if (++p_buffer_num == process_buffer_max)
		realloc_process_buffer();
	}
    }
    return TRUE;
}

/*
------------------------------------------------------------------------------
  PROCEDURE: <through_word> 括弧，空白の透過処理
------------------------------------------------------------------------------
*/
    
int 	through_word(int pos , int m_num)
{
    int i, j, k, l, n, nn, sc, scmin;
    MRPH *now_mrph, *mrph_p;

    now_mrph = &m_buffer[m_num];
    if (!is_through(now_mrph)) return TRUE;

    for (l = 0; (i = match_pbuf[l]) >= 0; l++) {
	for (j = 0 ; j < m_buffer_num ; j++) {
	    mrph_p = &m_buffer[j];
	    if (mrph_p->hinsi   == now_mrph->hinsi   &&
		mrph_p->bunrui  == now_mrph->bunrui  &&
		mrph_p->con_tbl == m_buffer[p_buffer[i].mrph_p].con_tbl &&
		mrph_p->weight  == now_mrph->weight  &&
		strcmp(mrph_p->midasi, now_mrph->midasi) == 0 &&
		strcmp(mrph_p->yomi,   now_mrph->yomi  ) == 0) break;
	}
	if ((n = j) == m_buffer_num) {
	    /* その文の中で初めて出現したタイプの括弧であれば，直前の
	       形態素のcon_tblを透過させるようにcon_tblを変化させて，
	       m_bufferに追加する */
	    m_buffer[m_buffer_num] = *now_mrph;
	    m_buffer[m_buffer_num].con_tbl
	      = m_buffer[p_buffer[i].mrph_p].con_tbl;
	    if (++m_buffer_num == mrph_buffer_max) {
		realloc_mrph_buffer();
		now_mrph = &m_buffer[m_num];	/* fixed by kuro 99/09/01 */
	    }
	}

	/* 透過規則に基づく連接 */
	sc = (now_mrph->weight*cost_omomi.keitaiso*
	      Class[now_mrph->hinsi][now_mrph->bunrui].cost);
	for (j = 0 ; j < p_buffer_num ; j++)
	  if (p_buffer[j].mrph_p == n && p_buffer[j].start == pos) break;
	if ((nn = j) == p_buffer_num) {
	    /* 初めて現われるような透過なら，p_bufferに新たに追加する */
	    p_buffer[p_buffer_num].score = p_buffer[i].score+sc;
	    p_buffer[p_buffer_num].mrph_p = n;
	    p_buffer[p_buffer_num].start = pos;
	    p_buffer[p_buffer_num].end = pos + now_mrph->length;
	    p_buffer[p_buffer_num].path[0] = i;
	    p_buffer[p_buffer_num].path[1] = -1;
	    p_buffer[p_buffer_num].connect = TRUE;
	    if (++p_buffer_num == process_buffer_max)
		realloc_process_buffer();
	} else {
	    /* p_bufferの爆発を防ぐため，以前のp_bufferの使い回しをする */
	    for (j = 0 ; p_buffer[nn].path[j] != -1 ; j++) {}
	    p_buffer[nn].path[j]   = i;
	    p_buffer[nn].path[j+1] = -1;

	    /* コストが高いものは枝刈りする */
	    scmin = INT_MAX;
	    for (j = 0 ; p_buffer[nn].path[j] != -1 ; j++)
	      if (scmin > p_buffer[p_buffer[nn].path[j]].score)
		scmin = p_buffer[p_buffer[nn].path[j]].score;
	    for (j = 0 ; p_buffer[nn].path[j] != -1 ; j++)
	      if (p_buffer[p_buffer[nn].path[j]].score
		  > scmin + cost_omomi.cost_haba) {
		  for (k = j ; p_buffer[nn].path[k] != -1 ; k++)
		    p_buffer[nn].path[k] = p_buffer[nn].path[k+1];
		  j--;
	      }
	    p_buffer[nn].score = scmin+sc;
	}
    }
    return TRUE;
}

/*
------------------------------------------------------------------------------
  PROCEDURE: <is_through>
------------------------------------------------------------------------------
*/
    
int 	is_through(MRPH *mrph_p)
{
    if (
	/* 括弧始は透過処理をしない 2007/12/06
	(mrph_p->hinsi == kakko_hinsi && mrph_p->bunrui == kakko_bunrui1) ||
	*/
	(mrph_p->hinsi == kakko_hinsi && mrph_p->bunrui == kakko_bunrui2) ||
	(mrph_p->hinsi == kuuhaku_hinsi && mrph_p->bunrui == kuuhaku_bunrui))
	return TRUE;
    else 
	return FALSE;
}

/*
------------------------------------------------------------------------------
  PROCEDURE: <my_fprintf> (MS Windows のため に出力を SJIS にする)
                             >>> Added by Taku Kudoh <<<
------------------------------------------------------------------------------
*/
#ifdef _WIN32
void my_fprintf(FILE* output, const char *fmt, ...)
{
    va_list  ap;
    char     buf[1024];
    char     *sjisstr;

    va_start(ap,fmt);
    vsprintf(buf,fmt,ap);
    va_end(ap);

    /* SJIS に変更して出力 */
    sjisstr = toStringSJIS(buf);
    fwrite(sjisstr,sizeof(char),strlen(sjisstr),output);
    free(sjisstr);
}
#endif

char **OutputAV;
int OutputAVnum;
int OutputAVmax;

MRPH *prepare_path_mrph(int path_num , int para_flag)
{
    MRPH       	*mrph_p;
    int        	j;

    mrph_p = &(m_buffer[p_buffer[path_num].mrph_p]);

    if (para_flag != 0 && is_through(mrph_p) == TRUE) return NULL;
    
    if (para_flag)
	strcpy(kigou, "@ ");
    else
	kigou[0] = '\0';
    strcpy(midasi1, mrph_p->midasi);
    strcpy(midasi2, *mrph_p->midasi2 ? mrph_p->midasi2 : mrph_p->midasi);
    strcpy(yomi, mrph_p->yomi);
    if ( (mrph_p->katuyou1 > 0) && (mrph_p->katuyou2 > 0) ) {
	strcat(midasi1, Form[mrph_p->katuyou1][mrph_p->katuyou2].gobi);
	for(j = 1; strcmp(Form[mrph_p->katuyou1][j].name, BASIC_FORM); j++);
	strcat(midasi2, Form[mrph_p->katuyou1][j].gobi);
	strcat(yomi, Form[mrph_p->katuyou1][mrph_p->katuyou2].gobi_yomi);
    }

    /* 連濁、小文字化用 */
    if (mrph_p->hinsi != 1 && mrph_p->hinsi != 15 &&
	strncmp(midasi1, String + p_buffer[path_num].start, mrph_p->length)) {
	strncpy(midasi1, String + p_buffer[path_num].start, mrph_p->length);
    }

    return mrph_p;
}

char *get_path_mrph(int path_num , int para_flag)
{
    int len = 0;
    MRPH *mrph_p;
    char *ret;

    if ((mrph_p = prepare_path_mrph(path_num, para_flag)) == NULL) return NULL;
    len = strlen(kigou)+strlen(midasi1)+strlen(yomi)+strlen(midasi2)+strlen(Class[mrph_p->hinsi][0].id)+mrph_p->hinsi/10+1;

    if ( mrph_p->bunrui ) {
	len += strlen(Class[mrph_p->hinsi][mrph_p->bunrui].id);
    }
    else {
	len += 1;
    }

    len += mrph_p->bunrui/10+1;
	
    if ( mrph_p->katuyou1 ) { 
	len += strlen(Type[mrph_p->katuyou1].name);
    }
    else {
	len += 1;
    }

    len += mrph_p->katuyou1/10+1;
	
    if ( mrph_p->katuyou2 ) {
	len += strlen(Form[mrph_p->katuyou1][mrph_p->katuyou2].name);
    }
    else {
	len += 1;
    }

    len += mrph_p->katuyou2/10+1;

    len += 12;	/* 隙間 10, 改行 1, 終端 1 */

    switch (Show_Opt2) {
    case Op_E2:
	len += strlen(mrph_p->imis) + 1;
	ret = (char *)malloc(len);
	sprintf(ret, "%s%s %s %s %s %d %s %d %s %d %s %d %s\n", kigou, midasi1, yomi, midasi2, 
		Class[mrph_p->hinsi][0].id, mrph_p->hinsi, 
		mrph_p->bunrui ? Class[mrph_p->hinsi][mrph_p->bunrui].id : (U_CHAR *)"*", mrph_p->bunrui, 
		mrph_p->katuyou1 ? Type[mrph_p->katuyou1].name : (U_CHAR *)"*", mrph_p->katuyou1, 
		mrph_p->katuyou2 ? Form[mrph_p->katuyou1][mrph_p->katuyou2].name : (U_CHAR *)"*", mrph_p->katuyou2, 
		mrph_p->imis);
	break;
    case Op_E:
	ret = (char *)malloc(len);
	sprintf(ret, "%s%s %s %s %s %d %s %d %s %d %s %d\n", kigou, midasi1, yomi, midasi2, 
		Class[mrph_p->hinsi][0].id, mrph_p->hinsi, 
		mrph_p->bunrui ? Class[mrph_p->hinsi][mrph_p->bunrui].id : (U_CHAR *)"*", mrph_p->bunrui, 
		mrph_p->katuyou1 ? Type[mrph_p->katuyou1].name : (U_CHAR *)"*", mrph_p->katuyou1, 
		mrph_p->katuyou2 ? Form[mrph_p->katuyou1][mrph_p->katuyou2].name : (U_CHAR *)"*", mrph_p->katuyou2);
	break;
    }
    return ret;
}

int get_best_path_num()
{
    int j, last;

    j = 0;
    last = p_buffer_num-1;
    do {
	last = p_buffer[last].path[0];
	path_buffer[j] = last;
	j++;
    } while ( p_buffer[last].path[0] );

    return j;
}

/*
------------------------------------------------------------------------------
  PROCEDURE: <print_path_mrph> 形態素の表示      >>> changed by yamaji <<<
------------------------------------------------------------------------------
*/
/*
  サーバーモード対応のため、引数outputを増やして出力先の変更を可能にする。
  NACSIS 吉岡
*/
/* para_flag != 0 なら @出力 */
void print_path_mrph(FILE* output, int path_num , int para_flag)
{
    PROCESS_BUFFER	*proc_p;
    MRPH       	*mrph_p;
    int		newDicNo;
    int         now_r_buffer_num;
    MRPH        r_last_mrph;
    int		pos;
    int        	i, j, k, len;

    if ((mrph_p = prepare_path_mrph(path_num, para_flag)) == NULL) return;
    proc_p = &(p_buffer[path_num]);
    pos = proc_p->start;

    fputs(kigou, output);

    switch (Show_Opt2) {
    case Op_F: 
	len = strlen(yomi); yomi[len] = ')'; yomi[len+1] = '\0';
	my_fprintf(output, "%-12.12s(%-12.12s%-10.10s %-14.14s",
		midasi1, yomi, midasi2,
		Class[mrph_p->hinsi][mrph_p->bunrui].id);
	if (mrph_p->katuyou1)
	    my_fprintf(output, " %-14.14s %-12.12s",
		    Type[mrph_p->katuyou1].name,
		    Form[mrph_p->katuyou1][mrph_p->katuyou2].name);
	fputc('\n', output);
	break;
	
    case Op_C:
	my_fprintf(output, "%s %s %s %d %d %d %d\n", midasi1, yomi, midasi2,
		mrph_p->hinsi, mrph_p->bunrui,
		mrph_p->katuyou1, mrph_p->katuyou2);
	break;

    case Op_EE:
	/* ラティスのつながりを表示 */
	fprintf(output, "%d ", path_num);
	for (i = 0; proc_p->path[i] != -1; i++) {
	    if (i) fprintf(output, ";");
	    fprintf(output, "%d", proc_p->path[i]);
	}
	fprintf(output, " ");

	fprintf(output, "%d ", pos);
	if (!strcmp(midasi1, "\\ ")) pos++; else pos += strlen(midasi1);
	fprintf(output, "%d ", pos);
	/* -E オプションはこの部分だけ追加,
	   その後は -e -e2 オプションと同様の出力をするので break しない */

    case Op_E:
    case Op_E2:
	my_fprintf(output, "%s %s %s ", midasi1, yomi, midasi2);
	
	my_fprintf(output, "%s ", Class[mrph_p->hinsi][0].id);
	fprintf(output, "%d ", mrph_p->hinsi);
	
	if ( mrph_p->bunrui ) 
	  my_fprintf(output, "%s ", Class[mrph_p->hinsi][mrph_p->bunrui].id);
	else
	  my_fprintf(output, "* ");
	fprintf(output, "%d ", mrph_p->bunrui);
	
	if ( mrph_p->katuyou1 ) 
	  my_fprintf(output, "%s ", Type[mrph_p->katuyou1].name);
	else                    
	  fprintf(output, "* ");
	fprintf(output, "%d ", mrph_p->katuyou1);
	
	if ( mrph_p->katuyou2 ) 
	  my_fprintf(output, "%s ", Form[mrph_p->katuyou1][mrph_p->katuyou2].name);
	else 
	  fprintf(output, "* ");
	fprintf(output, "%d", mrph_p->katuyou2);

	if (Show_Opt2 == Op_E) {
	    fprintf(output, "\n");	/* -e では imis は出力しない */
	    break;
	}

	/* for SRI
	   fprintf(stdout, "\n");
	   if (para_flag) fprintf(stdout , "@ ");
	   */

	my_fprintf(output, " %s\n", mrph_p->imis);
	break;
    }
}

void process_path_mrph(FILE* output, int path_num , int para_flag) {
    if (output) {
	print_path_mrph(output, path_num, para_flag);
    }
    else {
	char *p;
	if (OutputAVnum == 0) {
	    OutputAVmax = 10;
	    OutputAV = (char **)malloc(sizeof(char *)*OutputAVmax);
	}
	else if (OutputAVnum >= OutputAVmax-1) {
	    OutputAV = (char **)realloc(OutputAV, sizeof(char *)*(OutputAVmax <<= 1));
	}
	p =  get_path_mrph(path_num, para_flag);
	if (p != NULL) {
	    *(OutputAV+OutputAVnum++) = p;
	    *(OutputAV+OutputAVnum) =  NULL;
	}
    }
}

/*
------------------------------------------------------------------------------
  PROCEDURE: <print_best_path> 後方最長一致のPATHを調べる
------------------------------------------------------------------------------
*/
/*
  サーバーモード対応のため、引数outputを増やして出力先の変更を可能にする。
  NACSIS 吉岡
*/
char **print_best_path(FILE* output)
{
    int i, j, last;
    MRPH *mrph_p,*mrph_p1;

    j = 0;
    last = p_buffer_num-1;
    do {
	last = p_buffer[last].path[0];
	path_buffer[j] = last;
	j++;
    } while ( p_buffer[last].path[0] );

    /* 結果を buffer に入れる場合 */
    if (!output) {
	OutputAVnum = 0;
	OutputAVmax = 0;
    }

    for ( i=j-1; i>=0; i-- ) {
	process_path_mrph(output, path_buffer[i] , 0);
    }
    return OutputAV;	/* 後でちゃんと free すること */
}

/*
------------------------------------------------------------------------------
  PROCEDURE: <print_all_mrph> 正しい解析結果に含まれる全ての形態素
------------------------------------------------------------------------------
*/
/*
  サーバーモード対応のため、引数outputを増やして出力先の変更を可能にする。
  NACSIS 吉岡
*/
char **print_all_mrph(FILE* output)
{
    int  i, j, k;
    MRPH mrph;

    for (i = 0 ; i < m_buffer_num ; i++)
	m_check_buffer[i] = 0;
    
    _print_all_mrph(output, p_buffer_num-1);
    m_check_buffer[0] = 0;

    /* 結果を buffer に入れる場合 */
    if (!output) {
	OutputAVnum = 0;
	OutputAVmax = 0;
    }

    for (i = 0 ; i < m_buffer_num ; i++)
	if (m_check_buffer[i])
	    process_path_mrph(output, i , 0);

    return OutputAV;	/* 後でちゃんと free すること */
}

void _print_all_mrph(FILE* output, int path_num)
{
    int i;
    
    for (i = 0 ; p_buffer[path_num].path[i] != -1 ; i++) {
	if (!m_check_buffer[p_buffer[path_num].path[i]]) {
	    m_check_buffer[p_buffer[path_num].path[i]] = 1;
	    _print_all_mrph(output, p_buffer[path_num].path[i]);
	}
    }
}
/*
------------------------------------------------------------------------------
  PROCEDURE: <print_all_path> 正しい解析結果に含まれる全てのPATH
------------------------------------------------------------------------------
*/
/*
  サーバーモード対応のため、引数outputを増やして出力先の変更を可能にする。
  NACSIS 吉岡
*/
char **print_all_path(FILE* output)
{
/*  int i,j;
    for (i = 0 ; i < p_buffer_num ; i++) {
	printf("%d %s %d %d --- " , i , m_buffer[p_buffer[i].mrph_p].midasi ,
	       p_buffer[i].start , p_buffer[i].end);
	for (j = 0 ; p_buffer[i].path[j] != -1 ; j++)
	  printf("%d ",p_buffer[i].path[j]);
	printf("\n");
    }*/

    /* 結果を buffer に入れる場合 */
    if (!output) {
	OutputAVnum = 0;
	OutputAVmax = 0;
    }

    _print_all_path(output, p_buffer_num-1, 0);
    return OutputAV;	/* 後でちゃんと free すること */   
}

void _print_all_path(FILE* output, int path_num, int pathes)
{
    int i, j;
    
    for ( i=0; p_buffer[path_num].path[i] != -1; i++ ) {
	if ( p_buffer[path_num].path[0] == 0 ) {
	    for ( j=pathes-1; j>=0; j-- )
	      process_path_mrph(output, path_buffer[j] , 0);
	    if (output) fprintf(output, "EOP\n");
	} else {
	    path_buffer[pathes] = p_buffer[path_num].path[i];
	    _print_all_path(output, p_buffer[path_num].path[i], pathes+1);
	}
    }
}

/*
------------------------------------------------------------------------------
  PROCEDURE: <print_homograph_path> 複数の候補をとれる形態素は列挙する
------------------------------------------------------------------------------
*/
/*
  サーバーモード対応のため、引数outputを増やして出力先の変更を可能にする。
  NACSIS 吉岡
*/
char **print_homograph_path(FILE* output)
{
    path_buffer[0] = p_buffer_num-1;
    path_buffer[1] = -1;

    /* 結果を buffer に入れる場合 */
    if (!output) {
	OutputAVnum = 0;
	OutputAVmax = 0;
    }

    _print_homograph_path(output, 0, 2);
    return OutputAV;	/* 後でちゃんと free すること */
}

int _print_homograph_path(FILE* output, int pbuf_start, int new_p)
{
    int i, j, k, l, now_pos, len, ll, pt, pt2, f;

    if (p_buffer[path_buffer[pbuf_start]].path[0] == 0) {
	/* 先頭までリンクをたどり終わった */
	for (j = new_p-2; j >= 1; j--) {
	    /* 同音異義語群を一気に出力 */
	    for ( ; path_buffer[j] >= 0; j--) {}
	    for (k = j+1, l = 0; path_buffer[k] >= 0; k++) {
		process_path_mrph(output, path_buffer[k] , l++);
	    }
	}
	if (Show_Opt1 == Op_BB) return(1);
	if (output) fprintf(output, "EOP\n");
	return(0);
    }

    /* 特殊優先規則
           3文字の複合語は 2-1
	   4文字の複合語は 2-2
	   5文字の複合語は 3-2
       と分割されることが多いので，この分割を優先的に出力するようにする．
       そのため，
       ・2文字語の左に2,3,4文字語が連接している場合は2文字語を優先
       ・それ以外ならば文字数の少ない語を優先
       とする． */
    f = 0;
    now_pos = p_buffer[path_buffer[pbuf_start]].start;
    for (j = pbuf_start; path_buffer[j] >= 0; j++)
	for (i = 0; (pt = p_buffer[path_buffer[j]].path[i]) != -1; i++)
	    /* 2文字語を探す */
	    if (p_buffer[pt].start == now_pos-2*2) {
		for (k = 0; (pt2 = p_buffer[pt].path[k]) != -1; k++)
		    /* 2文字語の左に連接している語が2〜4文字語か？ */
		    if (p_buffer[pt2].start <= now_pos-(2+2)*2 &&
			p_buffer[pt2].start >= now_pos-(2+4)*2) f = 1;
	    }

    for (ll = 1; ll <= now_pos; ll++) { /* 半角文字のため，1byteごとに処理 */
	len = ll;
	if (f)
	    /* 1文字語と2文字語の優先順位を入れ替える */
	    if (ll == 2) len = 4; else if (ll == 4) len = 2;

	/* 同じ長さの形態素を全てリストアップする */
	l = new_p;
	for (j = pbuf_start; path_buffer[j] >= 0; j++) {
	    for (i = 0; (pt = p_buffer[path_buffer[j]].path[i]) != -1; i++) {
		if (p_buffer[pt].start == now_pos-len) {
		    /* 同音異義語群を求める(但し重複しないようにする) */
		    for (k = new_p; k < l; k++)
			if (path_buffer[k] == pt) break;
		    if (k == l) path_buffer[l++] = pt;
		}
	    }
	}
	path_buffer[l] = -1; /* 同音異義語群のエンドマーク */
	if (l != new_p)
	    if (_print_homograph_path(output, new_p, l+1)) return(1);
    }
    return(0);
}

/*
------------------------------------------------------------------------------
  PROCEDURE: <pos_match_process> <pos_right_process>
------------------------------------------------------------------------------
*/
int pos_match_process(int pos, int p_start)
{
    int i, j;

    /* 位置がマッチする p_buffer を抽出して，match_pbuf に書き並べる */
    j = 0;
    for (i = p_start; i < p_buffer_num; i++) {
	if (p_buffer[i].end <= pos || p_buffer[i].connect == FALSE) {
	    if (i == p_start)
	      /* p_start 以前の p_buffer は十分 pos が小さいので，探索を省略 */
	      p_start++;
	    if (p_buffer[i].end == pos && p_buffer[i].connect == TRUE)
	      match_pbuf[j++] = i;
	}
    }
    match_pbuf[j] = -1;

    return p_start;
}

int pos_right_process(int pos)
{
    int	i;
    
    for ( i=0; i<p_buffer_num; i++ )
      if ( p_buffer[i].end > pos )
	return 1;
    
    return 0;
}

/*
------------------------------------------------------------------------------
  PROCEDURE: <check_connect>			Changed by yamaji
------------------------------------------------------------------------------
*/

int check_connect(int pos, int m_num, char opt)
{
    static CHK_CONNECT_WK chk_connect[MAX_PATHES_WK];
    int chk_con_num;
    int i, j, pathes;
    int	score, best_score, haba_score, best_score_num;
    int c_score, class_score;
    MRPH *new_mrph;
    MRPH *pre_mrph;
    int left_con, right_con;
    CONNECT_COST *c_cache;

    new_mrph = &m_buffer[m_num];
    best_score = INT_MAX;  /* maximam value of int */
    chk_con_num = 0;
    score = class_score = best_score_num = 0;
    class_score = Class[new_mrph->hinsi][new_mrph->bunrui].cost 
	* new_mrph->weight * cost_omomi.keitaiso;

    /* 連接キャッシュにヒットすれば，探索を行わない */
    c_cache = &connect_cache[rensetu_tbl[new_mrph->con_tbl].j_pos];
    if (Show_Opt_debug == 0) {
	if (c_cache->pos == pos && c_cache->p_no > 0 && c_cache->opt == opt) {
	    p_buffer[p_buffer_num].score = c_cache->cost + class_score;
	    p_buffer[p_buffer_num].mrph_p = m_num;
	    p_buffer[p_buffer_num].start = pos;
	    p_buffer[p_buffer_num].end = pos + new_mrph->length;
	    for (i = 0; (p_buffer[p_buffer_num].path[i] = p_buffer[c_cache->p_no].path[i]) >= 0; i++) {}
	    p_buffer[p_buffer_num].path[i] = -1;
	    p_buffer[p_buffer_num].connect = TRUE;
	    if (++p_buffer_num == process_buffer_max)
		realloc_process_buffer();
	    return TRUE;
	}
    }

    for (i = 0; (j = match_pbuf[i]) >= 0; i++) {

	/* 以前の形態素のcon_tblを取り出す */
	left_con = m_buffer[p_buffer[j].mrph_p].con_tbl;
	/* 新しい形態素のcon_tblを取り出す */
	right_con = new_mrph->con_tbl;

	c_score = check_matrix(left_con , right_con);
	
	/* 濁音化するのは直前の形態素が名詞、または動詞の連用形、名詞性接尾辞の場合のみ */
	/* 接尾辞の場合を除き直前の形態素が平仮名1文字となるものは不可 */
	if ((opt & OPT_DEVOICE) &&
	    (!(m_buffer[p_buffer[j].mrph_p].hinsi == rendaku_hinsi1 &&
	       m_buffer[p_buffer[j].mrph_p].katuyou2 == rendaku_renyou ||
	       m_buffer[p_buffer[j].mrph_p].hinsi == rendaku_hinsi2 ||
	       m_buffer[p_buffer[j].mrph_p].hinsi == rendaku_hinsi4 &&
	       (m_buffer[p_buffer[j].mrph_p].bunrui == rendaku_bunrui4_1 ||
		m_buffer[p_buffer[j].mrph_p].bunrui == rendaku_bunrui4_2 ||
		m_buffer[p_buffer[j].mrph_p].bunrui == rendaku_bunrui4_3 ||
		m_buffer[p_buffer[j].mrph_p].bunrui == rendaku_bunrui4_4)) ||
	     (m_buffer[p_buffer[j].mrph_p].hinsi != rendaku_hinsi4 &&
	      check_code(m_buffer[p_buffer[j].mrph_p].midasi, 0) == HIRAGANA &&
	      m_buffer[p_buffer[j].mrph_p].length == 2))) c_score = 0;

	if (c_score) {
	    chk_connect[chk_con_num].pre_p = j;

	    /* calculate the score */
	    score = p_buffer[j].score + c_score * cost_omomi.rensetsu;

	    chk_connect[chk_con_num].score = score;
	    if (score < best_score) {
		best_score = score;
		best_score_num = chk_con_num;
	    }
	    chk_con_num++;
	}

	/* デバグ用コスト表示 */

	if (Show_Opt_debug == 2 || (Show_Opt_debug == 1 && c_score)) {

	    fprintf(stderr, "%3d " , pos);

	    pre_mrph = &m_buffer[p_buffer[j].mrph_p];
	    fprintf(stderr, "%s" , pre_mrph->midasi);
	    if (Class[pre_mrph->hinsi][pre_mrph->bunrui].kt)
		fprintf(stderr, "%s", Form[pre_mrph->katuyou1][pre_mrph->katuyou2].gobi);
	    if (Class[pre_mrph->hinsi][0].id) {
		fprintf(stderr, "(%s" , Class[pre_mrph->hinsi][0].id);
		if (pre_mrph->bunrui)
		    fprintf(stderr, "-%s", Class[pre_mrph->hinsi][pre_mrph->bunrui].id);
		if (pre_mrph->katuyou1)
		    fprintf(stderr, "-%s", Type[pre_mrph->katuyou1].name);
		if (pre_mrph->katuyou2)
		    fprintf(stderr, "-%s", Form[pre_mrph->katuyou1][pre_mrph->katuyou2].name);
		fprintf(stderr, ")");
	    }
	    fprintf(stderr, "[= %d]", p_buffer[j].score);

	    if (c_score) {
		fprintf(stderr, "--[+%d*%d]--", c_score, cost_omomi.rensetsu);
	    } else {
		fprintf(stderr, "--XXX--");
	    }

	    fprintf(stderr, "%s" , new_mrph->midasi);
	    if (Class[new_mrph->hinsi][new_mrph->bunrui].kt)
		fprintf(stderr, "%s", Form[new_mrph->katuyou1][new_mrph->katuyou2].gobi);
	    if (Class[new_mrph->hinsi][0].id) {
		fprintf(stderr, "(%s" , Class[new_mrph->hinsi][0].id);
		if (new_mrph->bunrui)
		    fprintf(stderr, "-%s",Class[new_mrph->hinsi][new_mrph->bunrui].id);
		if (new_mrph->katuyou1)
		    fprintf(stderr, "-%s", Type[new_mrph->katuyou1].name);
		if (new_mrph->katuyou2)
		    fprintf(stderr, "-%s", Form[new_mrph->katuyou1][new_mrph->katuyou2].name);
		fprintf(stderr, ")");
	    }

	    if (c_score == 0) {
		fprintf(stderr, "\n");
	    } else {
		fprintf(stderr, "[+%d*%d.%d*%d = %d]\n", 
			Class[new_mrph->hinsi][new_mrph->bunrui].cost,
			new_mrph->weight/10, new_mrph->weight%10, 
			cost_omomi.keitaiso*10, 
			/* = class_score */
			p_buffer[j].score + c_score * cost_omomi.rensetsu + class_score);
	    }
	}
    }

    /* return immidiately, because if best_score is
       INT_MAX then no path exists. */
    if (best_score == INT_MAX) return TRUE;

    /* 今回の連接の様子を連接キャッシュに入れる */
    c_cache->p_no = p_buffer_num;
    c_cache->cost = best_score;
    c_cache->pos = pos;
    c_cache->opt = opt;

    /* 取り敢えずベストパスの1つを0番に登録 */
    p_buffer[p_buffer_num].path[0] = chk_connect[best_score_num].pre_p;
    pathes = 1;
    haba_score = best_score + cost_omomi.cost_haba;
    for (j = 0; j < chk_con_num; j++) /* それ以外のパスも追加 */
      if (chk_connect[j].score <= haba_score && j != best_score_num)
	p_buffer[p_buffer_num].path[pathes++] = chk_connect[j].pre_p;
    p_buffer[p_buffer_num].path[pathes] = -1;

    p_buffer[p_buffer_num].score = best_score+class_score;
    p_buffer[p_buffer_num].mrph_p = m_num;
    p_buffer[p_buffer_num].start = pos;
    p_buffer[p_buffer_num].end = pos + new_mrph->length;
    p_buffer[p_buffer_num].connect = TRUE;
    if (++p_buffer_num == process_buffer_max)
	realloc_process_buffer();
    return TRUE;
}

/*
------------------------------------------------------------------------------
  PROCEDURE: <juman_sent> 一文を形態素解析する    by T.Utsuro
------------------------------------------------------------------------------
*/
int juman_sent(void)
{
    int        i, j, flag;
    int        pos_end, length;
    int        pre_m_buffer_num;
    int        pre_p_buffer_num;
    int        pos, next_pos = 0;
    int	       p_start = 0;

    if (mrph_buffer_max == 0) {
	m_buffer = (MRPH *)my_alloc(sizeof(MRPH)*BUFFER_BLOCK_SIZE);
	m_check_buffer = (int *)my_alloc(sizeof(int)*BUFFER_BLOCK_SIZE);
	mrph_buffer_max += BUFFER_BLOCK_SIZE;
    }
    if (process_buffer_max == 0) {
	p_buffer = (PROCESS_BUFFER *)
	    my_alloc(sizeof(PROCESS_BUFFER)*BUFFER_BLOCK_SIZE);
	path_buffer = (int *)my_alloc(sizeof(int)*BUFFER_BLOCK_SIZE);
	match_pbuf = (int *)my_alloc(sizeof(int)*BUFFER_BLOCK_SIZE);
	process_buffer_max += BUFFER_BLOCK_SIZE;
    }

    length = strlen(String);

    if (length == 0) return FALSE;	/* 空行はスキップ */

    for (i = 0; i < CONNECT_MATRIX_MAX; i++) connect_cache[i].p_no = 0;

    /* 文頭処理 */
    p_buffer[0].end = 0;		
    p_buffer[0].path[0] = -1;
    p_buffer[0].score = 0;	
    p_buffer[0].mrph_p = 0;
    p_buffer[0].connect = TRUE;
    m_buffer[0].hinsi = 0;
    m_buffer[0].bunrui = 0;
    m_buffer[0].con_tbl = 0;
    m_buffer[0].weight = MRPH_DEFAULT_WEIGHT;
    strcpy(m_buffer[0].midasi , "(文頭)");
    m_buffer_num = 1;
    p_buffer_num = 1;
    
    /* 非正規表記への対応 */
    strcpy(NormalizedString, String);
    for (pos = 0; pos < length; pos+=next_pos) {
	if (String[pos]&0x80) { /* 全角の場合 */
	    for (i = NORMALIZED_LOWERCASE_S; i < NORMALIZED_LOWERCASE_E; i++) {
		if (!strncmp(String + pos, lowercase[i], 2)) {
		    NormalizedString[pos] = uppercase[i][0];
		    NormalizedString[pos+1] = uppercase[i][1];
		}
	    }
	    next_pos = 2;
	} else {
	    next_pos = 1;
	}
    }

    for (pos = 0; pos < length; pos+=next_pos) {

	p_start = pos_match_process(pos, p_start);
	if (match_pbuf[0] >= 0) {
	
	    pre_m_buffer_num = m_buffer_num;
	    pre_p_buffer_num = p_buffer_num;
	
	    if (String[pos]&0x80) { /* 全角の場合，辞書をひく */
		if (search_all(pos) == FALSE) return FALSE;
		next_pos = 2;
	    } else {
		next_pos = 1;
	    }

	    if (undef_word(pos) == FALSE) return FALSE;
	}
    }
    
    /* 文末処理 */
    strcpy(m_buffer[m_buffer_num].midasi , "(文末)");
    m_buffer[m_buffer_num].hinsi = 0;
    m_buffer[m_buffer_num].bunrui = 0;
    m_buffer[m_buffer_num].con_tbl = 0;
    m_buffer[m_buffer_num].weight = MRPH_DEFAULT_WEIGHT;
    if (++m_buffer_num == mrph_buffer_max)
	realloc_mrph_buffer();

    pos_match_process(pos, p_start);
    if (check_connect(length, m_buffer_num-1, 0) == FALSE)
    return FALSE;

    return TRUE;
}
