/* 
 *  MS Windows は入出力を SJIS <=> EUC に変換する必要があるため
 *  文字コード変換用の汎用関数が必要
 *
 *  Added by Taku Kudoh (taku@pine.kuee.kyoto-u.ac.jp)
 *
 * japanese.h
 */

#define NO_HANKAKU_SJIS

#define ESC           6
#define LF            10
#define CR            13

#define CHAROUT(ch) *str2 = (unsigned char)(ch); str2++;
#define HANKATA(a)  (a >= 161 && a <= 223)
#define ISMARU(a)   (a >= 202 && a <= 206)
#define ISNIGORI(a) ((a >= 182 && a <= 196) || (a >= 202 && a <= 206) || (a == 179))
#define SJIS1(A)    ((A >= 129 && A <= 159) || (A >= 224 && A <= 239))
#define SJIS2(A)    (A >= 64 && A <= 252)
#define ISEUC(A)    (A >= 161 && A <= 254)
#define ISLF(A)     (A == LF)
#define ISCR(A)     (A == CR)

extern char *toStringEUC(char *str);
extern char *toStringSJIS(char *str);
