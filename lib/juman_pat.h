#define DEBUGn

#ifdef DEBUG
#define OI(x) {printf("< %d >\n",x);fflush(stdout);} /* Output Integer */ /*�ǥХ�������*/
#define OS(x) {printf("[ %s ]\n",x);fflush(stdout);} /* Output String */
#define OS2(x) {printf("%s\n",x);fflush(stdout);} /* Output String */
#define OM(x) {printf("Message: " #x "\n");fflush(stdout);} /* Output Message */
#define OL(x) {printf(#x);fflush(stdout);} /* Output Label */
#else
#define OI(x)  /* */
#define OS(x)  /* */
#define OS2(x) /* */
#define OM(x)  /* */
#define OL(x)  /* */
#endif

/* ���Ĥ�ϥå��� */
#ifdef USE_HASH
#include "t-hash.h"
#endif

/* ����ǥå����ѤΥꥹ�ȷ������ */
typedef struct pat_index_list {
  struct pat_index_list *next; /* �Ĥ� */
  long index; /* �ե�����Υ���ǥå��� */
} pat_index_list;

/* �Ρ��ɤΥǡ�����¤����� */
typedef struct pat_node {
  pat_index_list il; /* ����ǥå����Υꥹ�� */
  short checkbit; /* �����å�����ӥåȤλ��ꡣ(�����ܤΥӥå�?) */
#if 0
  char *str; /* �ȥ饤�Ȥ��ƤĤ�������˺����ɬ�פ�ʸ�������¸ 960919 */
#endif
  struct pat_node *right; /* ���ؤޤ���ޡ��� */
  struct pat_node *left; /* ���ؤޤ���ޡ��� */
} pat_node;


#define HASH_SIZE 131071 /* 107653  �ϥå���ơ��֥�Υ����� 1162213*/


#ifndef MAX_DIC_NUMBER 
#define MAX_DIC_NUMBER 5 /* Ʊ���˻Ȥ��뼭��ο��ξ�� (JUMAN) */
#endif
extern pat_node tree_top[MAX_DIC_NUMBER]; /* �ڤΤͤä��� ����ο������Ȥ� */
extern FILE *dic_file[MAX_DIC_NUMBER]; /* �ڤΤ�ȥǡ���(����ե�����) */

#define SIKII_BIT 16  /* ʸ����ʸ���ζ��ڤ�ϲ��ӥå���? (8 or 16) */

extern char line[50000]; /* ���Ϲ� */
extern FILE *out_file, *in_file; /* �����֥ե����롦���ɥե����� */
extern char  inkey[10000]; /* �������������� */

/*** JUMAN���������Ϣ ***/
extern int number_of_tree; /* ���Ѥ��뼭��(�ѥ���)�ο� */

/*** get_item()�� ***/
extern char partition_char; /* ���ڤ�ʸ�� */
extern int column; /* ��������ܤ� */

/**************************
 * �ؿ��Υץ�ȥ�������� *
 **************************/ 
/* pat.c */
extern void pat_init_tree_top(pat_node*); /* �ѥȥꥷ���ڤκ��ν���� */
extern pat_node *pat_search(FILE*,char*,pat_node*,char*); /* �ѥȥꥷ���ڤǸ��� */
extern pat_node *pat_search4insert(char*,pat_node*); /* �����Ѹ��� */
extern void pat_insert(FILE*,char*,long,pat_node*,char*); /* �ѥȥꥷ���ڤ����� */
extern int pat_bits(char*,int,int); 
         /* ʸ������λ��ꤵ�줿���֤ΥӥåȤ��֤� */
         /* 960918  ������strlrn()����Τ�̵�̤Ǥ��뤳�Ȥ�Ƚ�� */
extern void show_pat(pat_node*,FILE*,char*); /* �ѥȥꥷ���ڥǡ�������� */
extern char *get_line(FILE*,long); /* ���ꤵ�줿��꤫��'\n'�ޤ��ɤ� */

/* file.c */
extern void com_s(char*,pat_node*); /* �����ִ�Ϣ */
extern void save_pat(pat_node*);
extern void eputc(unsigned char, int);
extern void com_l(char*,pat_node*); /* ���ɴ�Ϣ */
extern pat_node *load_anode(pat_node*);
extern unsigned char egetc(int);
extern void dummy(void);
extern pat_node *malloc_pat_node(void); /* Matomete malloc */
extern pat_index_list *malloc_pat_index_list(void); /* Matomete malloc */

/* morph.c */
extern void jisyohiki(char*,pat_node*); /* ������� */
extern void insert_dic_data(FILE*,pat_node*,char*); /* ����ǡ���������*/

/************************************************************************
* 
* pat --- �ѥȥꥷ���ڤ�õ��������
* 
* ���: ���Ĥ�(tatuo-y@is.aist-nara.ac.jp)
* 
* ��Ū: �ѥȥꥷ���ڤ�õ����������Ԥ�
* 
* ����ʸ��: 
*   ���르�ꥺ�������Τ����ʸ��[1]�򻲾Ȥ�����C����Ǥμ�����
*   ʸ��[2]�Υץ����򻲹ͤˤ�����
* [1] R. Sedgewick �� ���ʿ�����顢��ƣ�ϡ��ĸ��� ����
*     ���르�ꥺ�� (Algorithms) ������2�� ��2�� õ����ʸ���󡦷׻�����
*     ����ʳؼ�,1992. (B195-2,pp.68-72)
* [2] �����졢ͭ߷�������ʿ���������ѡ�������§ �Խ��Ѱ�
*     ���르�ꥺ�༭ŵ
*     ��Ω���ǳ������,1994. (D74,pp.624-625)
* 
* ����:
*   1996/04/09  ư��! (������������ǡ����κ���Ĺ��8bit��[2]�����)
*           10  ���ϥ롼�����Ƶ��˲��ɡ�ʸ����ǡ����б�(����Ĺ̵����)��
*           30  ������/���ɵ�ǽ���Ρ��ɤΥǡ�����¤��ID�ֹ���ɲ�(��)��
*         5/06  ��ʬ�ڤ����ǡ������Ͻ�����
*         6/11  JUMAN�μ�������Ѥ˲�¤��
*           21  Ϣ�������Ƴ��(INDEX�򥭥�å��夹��)
*         7/01  ʣ���μ���ե�����(�ѥ���)���鸡���Ǥ���褦�ˤ�����
* 
* ���: JUMAN�μ�����������Ѥ���
* 
************************************************************************/
