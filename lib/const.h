/* 定数 */
#define STOP_MRPH_WEIGHT       255 /* このWeigthのときは形態素候補から除く */
#define OPT_NORMALIZE          1
#define OPT_DEVOICE            2

/* 濁音化している形態素を検索するために用いる */
/* 半濁音は副詞の自動認識のみで用いる */
/* 奇数番目が平仮名、続く偶数番目が対応する片仮名である必要あり */
U_CHAR          *dakuon[] = {"ぱ", "パ", "ぴ", "ピ", "ぷ", "プ", "ぺ", "ペ", "ぽ", "ポ",
			     "が", "ガ", "ぎ", "ギ", "ぐ", "グ", "げ", "ゲ", "ご", "ゴ",
			     "ざ", "ザ", "じ", "ジ", "ず", "ズ", "ぜ", "ゼ", "ぞ", "ゾ",
			     "だ", "ダ", "ぢ", "ヂ", "づ", "ヅ", "で", "デ", "ど", "ド",
			     "ば", "バ", "び", "ビ", "ぶ", "ブ", "べ", "ベ", "ぼ", "ボ",
			     "\0"};
U_CHAR          *seion[]  = {"は", "ハ", "ひ", "ヒ", "ふ", "フ", "へ", "ヘ", "ほ", "ホ",
			     "か", "カ", "き", "キ", "く", "ク", "け", "ケ", "こ", "コ",
			     "さ", "サ", "し", "シ", "す", "ス", "せ", "セ", "そ", "ソ",
			     "た", "タ", "ち", "チ", "つ", "ツ", "て", "テ", "と", "ト",
			     "は", "ハ", "ひ", "ヒ", "ふ", "フ", "へ", "ヘ", "ほ", "ホ",
			     "\0"};	     
U_CHAR *lowercase[] = {"ぁ", "ぃ", "ぅ", "ぇ", "ぉ", "ゎ", "ヵ",
		       "ァ", "ィ", "ゥ", "ェ", "ォ", "ヮ",
		       "っ", "ッ", "ゃ", "ャ", "ゅ", "ュ", "ょ", "ョ", "\0"};
U_CHAR *uppercase[] = {"あ", "い", "う", "え", "お", "わ", "か",
		       "ア", "イ", "ウ", "エ", "オ", "ワ", "\0"};
int normalized_lowercase = 7; /* 正規化するlowercase[]の範囲(7→"ヵ"までが対象) */
int contracted_lowercase = 15; /* 拗音として扱うlowercase[]の範囲((15→"ゃ"からが対象)) */

/* 非正規表記 */
#define NORMALIZED_COST        1  /* 小文字を大文字化するコスト */
#define NORMALIZED_LENGTH      6  /* 大文字化を考慮する最大形態素長 */

/* 改善される例
   「ぁつい」、「ちぃさい」、「ヵゎぃぃ」 */

/* 連濁 */
#define VERB_VOICED_COST       7  /* "が"から始まる動詞を除く動詞の連濁化のコスト */
#define VERB_GA_VOICED_COST    9  /* "が"から始まる動詞の連濁化のコスト */
#define NOUN_VOICED_COST       8  /* "が"から始まる名詞を除く名詞の連濁化のコスト */
#define NOUN_GA_VOICED_COST   11  /* "が"から始まる名詞の連濁化のコスト */
#define ADJECTIVE_VOICED_COST  9  /* 形容詞の連濁化のコスト */
#define OTHER_VOICED_COST      5  /* 上記以外の連濁化のコスト */

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

/* 反復型オノマトペ */
#define REPETITION_COST       13  /* 繰り返し1音あたりのコスト */
#define CONTRACTED_BONUS       4  /* 拗音があった場合のボーナス */
#define DAKUON_BONUS           2  /* 濁音があった場合のボーナス */
#define KATAKANA_BONUS         2  /* カタカナであった場合のボーナス */

/* 以上のweightで基本的に副詞と認識されるもの
   「ちょろりちょろり」、「ガンガン」、「すべすべ」、「ごうごう」、
   「スゴイスゴイ」、「しゃくしゃく」、「はいはい」、「たらたら」、
   「ぎゅうぎゅう」、「でんでん」、「ギューギュー」、「ガラガラ」、

   以上のweightで基本的に副詞と認識されないもの
   「むかしむかし」、「またかまたか」、「もりもり」、「ミニミニ」、
   「さくらさくら」、「おるおる」、「いるいる」、「あったあった」、
   「とべとべ」、「ごめんごめん」、「とぎれとぎれ」、「ジャジャ」 */
