/*
==============================================================================
        const.h
                2007/09/03  Ryohei Sasano
		2011/11/29  Last Modified
==============================================================================
*/

/*
   連濁認識("ガニ")、非正規表記("ぉはよぅ")の処理、オノマトペ("もっさり"、"ピタっと")等の
   自動認識処理で使用している定数、コスト、パターンなどを記述      
*/

/* 定数 */
#define STOP_MRPH_WEIGHT       255 /* このWeigthのときは形態素候補から除く */
#define OPT_NORMAL             1
#define OPT_NORMALIZE          2
#define OPT_DEVOICE            4
#define OPT_PROLONG_DEL        8
#define OPT_PROLONG_REPLACE    16
#define NORMALIZED_LENGTH      8   /* 非正規表記の処理で考慮する最大形態素長 */

/* 連濁処理等で使用する品詞や表記情報 */
#define         DEF_REP                  "代表表記"
#define         DEF_ONOMATOPOEIA_HINSI   "副詞"
#define         DEF_ONOMATOPOEIA_IMIS    "自動認識"
#define         DEF_RENDAKU_HINSI1       "動詞"
#define         DEF_RENDAKU_RENYOU       "基本連用形"
#define         DEF_RENDAKU_HINSI2       "名詞"
#define         DEF_RENDAKU_BUNRUI2_1    "普通名詞"
#define         DEF_RENDAKU_BUNRUI2_2    "サ変名詞"
#define         DEF_RENDAKU_BUNRUI2_3    "形式名詞"
#define         DEF_RENDAKU_HINSI3       "形容詞"
#define         DEF_RENDAKU_HINSI4       "接尾辞"
#define         DEF_RENDAKU_BUNRUI4_1    "名詞性述語接尾辞"
#define         DEF_RENDAKU_BUNRUI4_2    "名詞性名詞接尾辞"
#define         DEF_RENDAKU_BUNRUI4_3    "名詞性名詞助数辞"
#define         DEF_RENDAKU_BUNRUI4_4    "名詞性特殊接尾辞"
#define         DEF_RENDAKU_OK_FEATURE   "濁音可"
#define         DEF_RENDAKU_MIDASI_KA    "か"
#define         DEF_RENDAKU_IMIS         "濁音化"
#define         DEF_PROLONG_IMIS         "長音挿入"
#define         DEF_ABNORMAL_IMIS        "非標準表記"
#define         DEF_PROLONG_SYMBOL1      "ー"
#define         DEF_PROLONG_SYMBOL2      "〜"
#define         DEF_PROLONG_SYMBOL3      "っ"
#define         DEF_PROLONG_OK_FEATURE   "長音挿入可"
#define         DEF_PROLONG_INTERJECTION "感動詞"
#define         DEF_PROLONG_COPULA       "判定詞"
#define         DEF_PROLONG_NG_HINSI1    "動詞"
#define         DEF_PROLONG_NG_HINSI2    "名詞"
#define         DEF_PROLONG_NG_HINSI3    "接頭辞"
#define         DEF_PROLONG_NG_HINSI4    "助詞"
#define         DEF_PROLONG_NG_BUNRUI4_1 "格助詞"
#define         DEF_PROLONG_NG_BUNRUI4_2 "副助詞"
#define         DEF_PROLONG_NG_BUNRUI4_3 "接続助詞"

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

/* 小書き文字・拗音(+"ん","ン")、小書き文字に対応する大文字の一覧
   非正規表記の処理(小書き文字・対応する大文字)、
   オノマトペ認識(開始文字チェック、拗音(cf. CONTRACTED_BONUS))で利用 */
U_CHAR *lowercase[] = {"ぁ", "ぃ", "ぅ", "ぇ", "ぉ", "ゎ", "ヵ",
		       "ァ", "ィ", "ゥ", "ェ", "ォ", "ヮ", "っ", "ッ", "ん", "ン",
		       "ゃ", "ャ", "ゅ", "ュ", "ょ", "ョ", "\0"};
U_CHAR *uppercase[] = {"あ", "い", "う", "え", "お", "わ", "か", "\0"};

/* 長音置換のルールで利用 */
/* 長音記号直前の文字が pre_prolonged[] だった場合、長音記号を prolonged2chr[] に置換 */
U_CHAR *pre_prolonged[] = {"か", "ば", "ま", "ゃ", /* あ */
			   "い", "き", "し", "ち", "に", "ひ", "じ", "け", "せ", /* い */
			   "へ", "め", "れ", "げ", "ぜ", "で", "べ", "ぺ",
			   "く", "す", "つ", "ふ", "ゆ", "ぐ", "ず", "ぷ", "ゅ", /* う */
			   "お", "こ", "そ", "と", "の", "ほ", "も", "よ", "ろ",
			   "ご", "ぞ", "ど", "ぼ", "ぽ", "ょ", "え", "ね", "\0"}; /* え(ね) */
U_CHAR *prolonged2chr[] = {"あ", "あ", "あ", "あ", /* あ */
			   "い", "い", "い", "い", "い", "い", "い", "い", "い", /* い */
			   "い", "い", "い", "い", "い", "い", "い", "い",
			   "う", "う", "う", "う", "う", "う", "う", "う", "う", /* う */
			   "う", "う", "う", "う", "う", "う", "う", "う", "う",
			   "う", "う", "う", "う", "う", "う", "え", "え", "\0"}; /* え(ね) */

/* 小書き文字削除のルールで利用 */
/* 長音記号直前の文字が pre_lower[] だった場合、小書き文字を削除 */
#define DELETE_LOWERCASE_S 0   /* 削除するlowercase[]の範囲 */
#define DELETE_LOWERCASE_E 5   /*   (0,5→"ぁ"から"ぉ"までが対象) */
int pre_lower_start[DELETE_LOWERCASE_E] = {0,  14, 23, 30, 37};
int pre_lower_end[DELETE_LOWERCASE_E]   = {14, 23, 30, 37, 45};
U_CHAR *pre_lower[] = {"か", "さ", "た", "な", "は", "ま", "や", "ら", "わ", 
		       "が", "ざ", "だ", "ば", "ぱ",                          /* ぁ:14 */
 		       "い", "し", "に", "り", "ぎ", "じ", "ね", "れ", "ぜ",  /* ぃ: 9 */
		       "う", "く", "す", "ふ", "む", "る", "よ",              /* ぅ: 7 */
		       "け", "せ", "て", "め", "れ", "ぜ", "で",              /* ぇ: 7 */
		       "こ", "そ", "の", "も", "よ", "ろ", "ぞ", "ど", "\0"}; /* ぉ: 8 */

/* 処理ごとに使用する範囲(指定がない場合、全てを使用) */
#define VOICED_CONSONANT_S      0  /* 連濁認識で使用するdakuon[]の範囲 */
#define VOICED_CONSONANT_E     40  /*   (0,40→"が"から"ボ"までが対象) */
#define NORMALIZED_LOWERCASE_S  0  /* 正規化するlowercase[]の範囲 */
#define NORMALIZED_LOWERCASE_E  7  /*   (0,7→"ぁ"から"ヵ"までが対象) */
#define NORMALIZED_LOWERCASE_KA 6  /* "ヵ"のみ1字からなる形態素を認める(接続助詞「か」) */
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

/* 小書き文字を大文字化、平仮名を長音記号に置換する際の追加コスト */
#define NORMALIZED_COST        6

/* 長音を削除する際の追加コスト */
#define PROLONG_DEL_COST1      6  /* 感動詞 */
#define PROLONG_DEL_COST2     50  /* 判定詞 */
#define PROLONG_DEL_COST3      9  /* その他 (9より小さいと"あーあ"を指示詞と解析する) */

/* 反復型オノマトペのコスト */

/* 副詞と認識されるもの
   「ちょろりちょろり」、「ガンガン」、「すべすべ」、「ごうごう」、
   「スゴイスゴイ」、「しゃくしゃく」、「はいはい」、「たらたら」、
   「ぎゅうぎゅう」、「でんでん」、「ギューギュー」、「ガラガラ」、

   副詞と認識されないもの
   「むかしむかし」、「またかまたか」、「もりもり」、「ミニミニ」、
   「さくらさくら」、「おるおる」、「いるいる」、「あったあった」、
   「とべとべ」、「ごめんごめん」、「とぎれとぎれ」、「ジャジャ」 
   「ぜひぜひ」 */
   

#define REPETITION_COST       13  /* 繰り返し1音あたりのコスト */
#define DAKUON_BONUS           1  /* 濁音があった場合のボーナス */
#define CONTRACTED_BONUS       4  /* 拗音があった場合のボーナス */
#define KATAKANA_BONUS         2  /* カタカナであった場合のボーナス */

/* 非反復型オノマトペ認識用の定数 */
#define PATTERN_MAX      64

#ifdef IO_ENCODING_EUC
#define Hcode            "\xA4[\xA2-\xEF]"
#define Kcode            "\xA5[\xA1-\xF4]"
#define Ycode            "[\xA4\xA5][\xE3\xE5\xE7]"
#else /* UTF-8 */
#define Hcode            "\xE3(\x81[\x82-\xBF]|\x82[\x80-\x8F])"
#define Kcode            "\xE3(\x82[\xA0-\xBF]|\x83[\x80-\xBA])"
#define Ycode            "\xE3(\x82[\x83\x85\x87]|\x83[\xA3\xA5\xA7])"
#endif

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
   平仮名[あーわ]にマッチさせる場合は"Ｈ", ("ぁ", "ゐ", "ゑ", "を", "ん"は含まないので注意)
     片仮名[ァーヴ]にマッチさせる場合は"Ｋ", 
     [ゃゅょャュョ]にマッチさせる場合は"Ｙ"と記述
   高速化のため、
     通常の平仮名、片仮名以外から始まるもの、
     1文字目と2文字目の文字種が同じものに限定 */
char *mrph_pattern[]  = {
    "ＨっＨり    30", /* もっさり */
    "ＨっＨＹり  30", /* ぐっちょり */
    "ＫッＫリ    30", /* モッサリ */
    "ＫッＫＹリ  30", /* ズッチョリ */
    "ＨＨっと    24", /* かりっと */
    "ＫＫっと    20", /* ピタっと */
    "ＫＫッと    20", /* ピタッと */
    "\0"};
#endif

/* 半角カタカナと全角カタカナの対応表 (in Unicode) */
#define KATAKANA_HAN2ZEN_UNICODE_NUM 63
int katakana_han2zen_unicode_table[KATAKANA_HAN2ZEN_UNICODE_NUM][2] = {
    {0xff61, 0x3002}, /* 。 */
    {0xff62, 0x300c}, /* 「 */
    {0xff63, 0x300d}, /* 」 */
    {0xff64, 0x3001}, /* 、 */
    {0xff65, 0x30fb}, /* ・ */
    {0xff66, 0x30f2}, /* ヲ */
    {0xff67, 0x30a1}, /* ァ */
    {0xff68, 0x30a3}, /* ィ */
    {0xff69, 0x30a5}, /* ゥ */
    {0xff6a, 0x30a7}, /* ェ */
    {0xff6b, 0x30a9}, /* ォ */
    {0xff6c, 0x30e3}, /* ャ */
    {0xff6d, 0x30e5}, /* ュ */
    {0xff6e, 0x30e7}, /* ョ */
    {0xff6f, 0x30c3}, /* ッ */
    {0xff70, 0x30fc}, /* − */
    {0xff71, 0x30a2}, /* ア */
    {0xff72, 0x30a4}, /* イ */
    {0xff73, 0x30a6}, /* ウ */
    {0xff74, 0x30a8}, /* エ */
    {0xff75, 0x30aa}, /* オ */
    {0xff76, 0x30ab}, /* カ */
    {0xff77, 0x30ad}, /* キ */
    {0xff78, 0x30af}, /* ク */
    {0xff79, 0x30b1}, /* ケ */
    {0xff7a, 0x30b3}, /* コ */
    {0xff7b, 0x30b5}, /* サ */
    {0xff7c, 0x30b7}, /* シ */
    {0xff7d, 0x30b9}, /* ス */
    {0xff7e, 0x30bb}, /* セ */
    {0xff7f, 0x30bd}, /* ソ */
    {0xff80, 0x30bf}, /* タ */
    {0xff81, 0x30c1}, /* チ */
    {0xff82, 0x30c4}, /* ツ */
    {0xff83, 0x30c6}, /* テ */
    {0xff84, 0x30c8}, /* ト */
    {0xff85, 0x30ca}, /* ナ */
    {0xff86, 0x30cb}, /* ニ */
    {0xff87, 0x30cc}, /* ヌ */
    {0xff88, 0x30cd}, /* ネ */
    {0xff89, 0x30ce}, /* ノ */
    {0xff8a, 0x30cf}, /* ハ */
    {0xff8b, 0x30d2}, /* ヒ */
    {0xff8c, 0x30d5}, /* フ */
    {0xff8d, 0x30d8}, /* ヘ */
    {0xff8e, 0x30db}, /* ホ */
    {0xff8f, 0x30de}, /* マ */
    {0xff90, 0x30df}, /* ミ */
    {0xff91, 0x30e0}, /* ム */
    {0xff92, 0x30e1}, /* メ */
    {0xff93, 0x30e2}, /* モ */
    {0xff94, 0x30e4}, /* ヤ */
    {0xff95, 0x30e6}, /* ユ */
    {0xff96, 0x30e8}, /* ヨ */
    {0xff97, 0x30e9}, /* ラ */
    {0xff98, 0x30ea}, /* リ */
    {0xff99, 0x30eb}, /* ル */
    {0xff9a, 0x30ec}, /* レ */
    {0xff9b, 0x30ed}, /* ロ */
    {0xff9c, 0x30ef}, /* ワ */
    {0xff9d, 0x30f3}, /* ン */
    {0xff9e, 0x309b}, /* ゛ */
    {0xff9f, 0x309c}, /* ゜ */
};

#define KATAKANA_VARIATION_ABSORB_LENGTH (4 * BYTES4CHAR) /* 4文字以上でカタカナ末尾の長音符を吸収 */
#define KATAKANA_HIRAGANA_PROLONGED_SOUND_MARK_UNICODE 0x30fc

/* カタカナに後続する場合に、KATAKANA_HIRAGANA_PROLONGED_SOUND_MARK_UNICODE に正規化する長音記号類似文字のリスト (in Unicode) */
int prolong_characters_unicode_list[] = {
    0x2d,   /* HYPHEN-MINUS: -; 0x+ff0dは0x2dに自動変換 */
    0x7e,   /* TILDE: ~; 0xff5eは0x7eに自動変換 */
    0x2010, /* HYPHEN: ‐ */
    0x2011, /* NON-BREAKING HYPHEN: ‑ */
    0x2012, /* FIGURE DASH: ‒ */
    0x2013, /* EN DASH: – */
    0x2014, /* EM DASH: — */
    0x2015, /* HORIZONTAL BAR: ― */
    0x2053, /* SWANG DASH: ⁓ */
    0x2212, /* MINUS SIGN: − */
    0x2500, /* BOX DRAWINGS LIGHT HORIZONTAL: ─ */
    0x2501, /* BOX DRAWINGS HEAVY HORIZONTAL: ━ */
    0x301c, /* WAVE DASH: 〜 */
    0xff70, /* HALFWIDTH KATAKANA-HIRAGANA PROLONGED SOUND MARK: ｰ */
    0 /* sentinel */
};

#define QUANTITY_STRING_FEATURE "数量表現:"
#define QUANTITY_A_FEATURE "数量A:"
#define QUANTITY_B_FEATURE "数量B:"
#define QUANTITY_C_FEATURE "数量C:"
#define DENOMINATOR_FEATURE "分母:"
#define FRACTION_STRING "分の"	/* Noun.suusi.dic中の「分の」の数量表現に一致している必要がある */
