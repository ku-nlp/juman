#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <juman.h>

/* �ϥå���ơ��֥����� */
#ifdef USE_HASH
th_hash_node hash_array[HASH_SIZE];
#endif

pat_node tree_top[MAX_DIC_NUMBER]; /* �ڤΤͤä��� ����ο������Ȥ� */
FILE *dic_file[MAX_DIC_NUMBER]; /* �ڤΤ�ȥǡ���(����ե�����) */

char line[50000]; /* ���Ϲ� */
FILE *out_file, *in_file; /* �����֥ե����롦���ɥե����� */
char  inkey[10000]; /* �������������� */
char            *ProgName;

/*** JUMAN���������Ϣ ***/
int number_of_tree = 0; /* ���Ѥ��뼭��(�ѥ���)�ο� */


/****************************************************
*                      �ᥤ��                       *
****************************************************/
main(int argc, char *argv[])
{
  char comm;
  int i;
  pat_node *tmp;
  char kugiri[2]; /* ���ڤ�ʸ�� */
  char rslt[50000];
  char	CurPath[FILENAME_MAX];
  char	JumanPath[FILENAME_MAX];

  ProgName = argv[0];

#ifdef USE_HASH
  th_hash_initialize(hash_array,HASH_SIZE);
#endif

  sprintf(kugiri,"\t"); /* ���ڤ�ʸ���Υǥե���Ȥϥ��� */

  /* �ե����뤫������ */
  getpath(CurPath, JumanPath);
  sprintf(inkey, "%s%s", CurPath, DICFILE);

  printf("File Name \"%s\"\n",inkey);
#ifdef _WIN32   
  dic_file[number_of_tree] = fopen(inkey,"rb");
#else
  dic_file[number_of_tree] = fopen(inkey,"r");
#endif   
  OL(Tree No.);OI(number_of_tree);
  (void)pat_init_tree_top(&tree_top[number_of_tree]);
  (void)insert_dic_data(dic_file[number_of_tree],&tree_top[number_of_tree],kugiri);
  number_of_tree++;

  /* �ڤΥ����� */
  sprintf(inkey, "%s%s", CurPath, PATFILE);
  (void)com_s(inkey,&tree_top[0]);

  /* ��λ */
/*      th_show_hash(hash_array,HASH_SIZE);*/
  printf("QUIT\n");
  exit(0);
}

/****************************************************
* insert_dic_data
* 
* �ѥ�᡼��
*   string --- ʸ����
****************************************************/
void insert_dic_data(FILE *f, pat_node *x_ptr, char *kugiri)
{
  long i = 0; long entry_ctr = 0;
  int len = 0;

  char corpus_buffer[50000]; /* �����ѥ�����Υǡ������Ǽ����Хåե� */
  char *c;

  while ((c = get_line(f, i)) != NULL) {
    strcpy(corpus_buffer, c);
    len = strlen(corpus_buffer) + 1;
    OL(---------------------\n)
    OL(INSERT:)OI(i)OS(corpus_buffer);
    (void)pat_insert(f,corpus_buffer, i, x_ptr, kugiri); /* ����*/
    i += len;
    entry_ctr++;
    if(entry_ctr % 1000 == 0){
      printf(".");
      if(entry_ctr % 20000 == 0) printf(" %ld\n",entry_ctr);
      fflush(stdout);
    }
  }
  printf("\n");
  printf("## %d entry  %ld th char\n",entry_ctr,i);

  return;
}
