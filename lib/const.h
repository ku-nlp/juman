/*
==============================================================================
        const.h
                2007/09/03  Ryohei Sasano
		2010/08/09  Last Modified
==============================================================================
*/

/*
   連濁認識("ガニ")、非正規表記("ぉはよぅ")の処理、オノマトペ("もっさり"、"ピタっと")の
   自動認識処理で使用している定数、コスト、パターンなどを記述      
*/

/* 定数 */
#define STOP_MRPH_WEIGHT       255 /* このWeigthのときは形態素候補から除く */
#define OPT_NORMALIZE          1
#define OPT_DEVOICE            2
#define OPT_MACRON             4
#define NORMALIZED_LENGTH      6   /* 非正規表記の処理で考慮する最大形態素長 */
#define MACRON_SEARCH_LENGTH  10   /* 長音化の処理で長音を探す最大バイト数 */

/* 濁音・半濁音、濁音と対応する清音の一覧 
   連濁認識(濁音・対応する清音)、オノマトペ認識(濁音・半濁音(cf. DAKUON_BONUS))で利用 
   (奇数番目を平仮名、続く要素を対応する片仮名とすること) */
U_CHAR *dakuon[] = {"が", "ガ", "ぎ", "ギ", "ぐ", "グ", "げ", "ゲ", "ご", "ゴ",
		    "ざ", "ザ", "じ", "ジ", "ず", "ズ", "ぜ", "ゼ", "ぞ", "ゾ",
		    "だ", "ダ", "ぢ", "ヂ", "づ", "ヅ", "で", "デ", "ど", "ド",
		    "ば", "バ", "び", "ビ", "ぶ", "ブ", "べ", "ベ", "ぼ", "ボ",
		    "ぱ", "パ", "ぴ", "ピ", "ぷ", "プ", "ぺ", "ペ", "ぽ", "ポ", "\0"};
U_CHAR *seion[]  = {"か", "カ", "き", "キ", "く", "ク", "け", "ケ", "こ", "コ",
		    "さ", "サ", "し", "シ", "す", "ス", "せ", "セ", "そ", "ソ",
		    "た", "タ", "ち", "チ", "つ", "ツ", "て", "テ", "と", "ト",
		    "は", "ハ", "ひ", "ヒ", "ふ", "フ", "へ", "ヘ", "ほ", "ホ", "\0"};

/* 小文字・拗音(+"ん","ン")、小文字に対応する大文字の一覧
   非正規表記の処理(小文字・対応する大文字)、
   オノマトペ認識(開始文字チェック、拗音(cf. CONTRACTED_BONUS))で利用 */
U_CHAR *lowercase[] = {"ぁ", "ぃ", "ぅ", "ぇ", "ぉ", "ゎ", "ヵ",
		       "ァ", "ィ", "ゥ", "ェ", "ォ", "ヮ", "っ", "ッ", "ん", "ン",
		       "ゃ", "ャ", "ゅ", "ュ", "ょ", "ョ", "\0"};
U_CHAR *uppercase[] = {"あ", "い", "う", "え", "お", "わ", "か", "\0"};

/* 長音置換のルールで利用 */
U_CHAR *i_gyo[] = {"い", "き", "し", "ち", "に", "ひ", "み", "り", 
		   "ぎ", "じ", "ぢ", "び", "\0"};
U_CHAR *u_gyo[] = {"う", "く", "す", "つ", "ぬ", "ふ", "む", "ゆ", "る", 
		   "ぐ", "ず", "づ", "ぶ", "\0"};
U_CHAR *e_gyo[] = {"え", "け", "せ", "て", "ね", "へ", "め", "れ", 
		   "げ", "ぜ", "で", "べ", "\0"};
U_CHAR *o_gyo[] = {"お", "こ", "そ", "と", "の", "ほ", "も", "よ", "ろ", 
		   "ご", "ぞ", "ど", "ぼ", "\0"};

/* 処理ごとに使用する範囲(指定がない場合、全てを使用) */
#define VOICED_CONSONANT_S     0   /* 連濁認識で使用するdakuon[]の範囲 */
#define VOICED_CONSONANT_E     40  /*   (0,40→"が"から"ボ"までが対象) */
#define NORMALIZED_LOWERCASE_S 0   /* 正規化するlowercase[]の範囲 */
#define NORMALIZED_LOWERCASE_E 7   /*   (0,7→"ぁ"から"ヵ"までが対象) */
#define CONTRACTED_LOWERCASE_S 17  /* 拗音として扱うlowercase[]の範囲 */
#define CONTRACTED_LOWERCASE_E 23  /*   (17,23→"ゃ"から"ョ"までが対象) */

/* 連濁認識用の追加コスト */

/* "が"から始まる動詞を除く動詞の連濁化のコスト
      4以下だと、"盛りだくさん"が正しく解析できない(061031)
      5以下だと、"とこしずめのまつり"が正しく解析できない(070123)
      6以下だと、"カネづるになる"の解釈に不要な曖昧性が生じる(070123)

   "が"から始まる動詞の連濁化のコスト
      7以下だと、"きりがない"が正しく解析できない(060928)
      8以下だと、"疲れがたまる"が正しく解析できない(060928)
     10以上だと、"ひっくりがえす"が正しく解析できない(060928)

   "が"から始まる名詞を除く名詞の連濁化のコスト
      7以下だと、"変わりばえが"の解釈に不要な曖昧性が生じる(060928)
      9以上だと、"ものごころ"が正しく解析できない(060928)

   "が"から始まる名詞の連濁化のコスト
      6以下だと、"右下がりの状態"が正しく解析できない(060928)
     10以下だと、"男がなたで"が正しく解析できない(070123)

   形容詞の連濁化のコスト
     10以上だと、"盛りだくさん"が解析できない(061031) */

#define VERB_VOICED_COST       7  /* "が"から始まる動詞を除く動詞の連濁化のコスト */
#define VERB_GA_VOICED_COST    9  /* "が"から始まる動詞の連濁化のコスト */
#define NOUN_VOICED_COST       8  /* "が"から始まる名詞を除く名詞の連濁化のコスト */
#define NOUN_GA_VOICED_COST   11  /* "が"から始まる名詞の連濁化のコスト */
#define ADJECTIVE_VOICED_COST  9  /* 形容詞の連濁化のコスト */
#define OTHER_VOICED_COST      5  /* 上記以外の連濁化のコスト */

/* 小文字を大文字化する際の追加コスト */
#define NORMALIZED_COST        1

/* 長音を削除・置換する際の追加コスト */
#define MACRON_COST	2

/* 反復型オノマトペのコスト */

/* 副詞と認識されるもの
   「ちょろりちょろり」、「ガンガン」、「すべすべ」、「ごうごう」、
   「スゴイスゴイ」、「しゃくしゃく」、「はいはい」、「たらたら」、
   「ぎゅうぎゅう」、「でんでん」、「ギューギュー」、「ガラガラ」、

   副詞と認識されないもの
   「むかしむかし」、「またかまたか」、「もりもり」、「ミニミニ」、
   「さくらさくら」、「おるおる」、「いるいる」、「あったあった」、
   「とべとべ」、「ごめんごめん」、「とぎれとぎれ」、「ジャジャ」 */

#define REPETITION_COST       13  /* 繰り返し1音あたりのコスト */
#define DAKUON_BONUS           2  /* 濁音があった場合のボーナス */
#define CONTRACTED_BONUS       4  /* 拗音があった場合のボーナス */
#define KATAKANA_BONUS         2  /* カタカナであった場合のボーナス */

/* 非反復型オノマトペ認識用の定数 */
#define PATTERN_MAX      64
#define Hcode            "\xA4[\xA1-\xF3]"
#define Kcode            "\xA5[\xA1-\xF4]"
#define Ycode            "[\xA4\xA5][\xE3\xE5\xE7]"
#define Hkey             "Ｈ"
#define Ykey             "Ｙ"
#define Kkey             "Ｋ"
#define DefaultWeight    10

#ifdef HAVE_REGEX_H
/* 非反復型オノマトペのパターンを保持するための構造体 */
typedef struct {
    char        regex[PATTERN_MAX];
    regex_t     preg;
    double      weight;
} MRPH_PATTERN;
MRPH_PATTERN *m_pattern;

/* 非反復型オノマトペのパターンとコスト */
/* 書式: 【パターン コスト】
     平仮名[ぁーん]にマッチさせる場合は"Ｈ", 
     片仮名[ァーヴ]にマッチさせる場合は"Ｋ", 
     [ゃゅょャュョ]にマッチさせる場合は"Ｙ"と記述 */
char *mrph_pattern[]  = {
    "ＨっＨり    30", /* もっさり */
    "ＨっＨＹり  30", /* ぐっちょり */
    "ＫッＫリ    30", /* モッサリ */
    "ＫッＫＹリ  30", /* ズッチョリ */
    "ＫＫっと    20", /* ピタっと */
    "ＫＫッと    20", /* ピタッと */
    "\0"};
#endif
