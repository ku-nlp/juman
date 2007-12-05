#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

/*#define HAVE_MMAP*/
#ifdef HAVE_MMAP
#undef USE_HASH
#include <sys/mman.h>

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#endif

#if defined _WIN32 && ! defined __CYGWIN__
typedef char *	caddr_t;
#endif

#include "juman_pat.h"

/* �ϥå���ơ��֥����� */
#ifdef USE_HASH
th_hash_node hash_array[HASH_SIZE];
#endif

pat_node tree_top[MAX_DIC_NUMBER]; /* �ڤΤͤä��� ����ο������Ȥ� */
FILE *dic_file[MAX_DIC_NUMBER]; /* �ڤΤ�ȥǡ���(����ե�����) */

static struct _dic_t {
  int used;
  int fd;
  off_t size;
  caddr_t addr;
} dicinfo[MAX_DIC_NUMBER];

/******************************************************
* pat_strcmp_prefix --- �ץ�ե������ޥå�
*
* �ѥ�᡼��
*   s1 --- Prefix String
*   s2 --- Ĵ�٤��� String
*
* �֤���  ���� 1������ 0
******************************************************/
static int pat_strcmp_prefix(char *s1, char *s2)
{
  for (;;) {
    if (*s2 == '\t') return 1;
    if (*s1++ != *s2++) return 0;
  }
}

/******************************************************
* pat_strcpy --- ʸ���󥳥ԡ�
*
* �ѥ�᡼��
*   s1, s2
*
* �֤���
******************************************************/
static char *pat_strcpy(char *s1, char *s2)
{
    while (*s1++ = *s2++);
    return s1 - 1;
}


/******************************************************
* pat_init_tree_top --- �ѥȥꥷ���ڤκ��ν����
*
* �ѥ�᡼��
*   ptr --- ����������ڤκ��ؤΥݥ���
******************************************************/
void pat_init_tree_top(pat_node *ptr) {
  (ptr->il).index = -1; /* ����ǥå����Υꥹ�� */
  ptr->checkbit = -1;
  ptr->right = ptr;
  ptr->left = ptr;
}

/****************************************************
* pat_search --- �ѥȥꥷ���ڤ򸡺�
* 
* �ѥ�᡼��
*   key --- ��������
*   x_ptr --- �������ϰ���(�ݥ���)
*   rslt --- ��̤�����롥
* 
* �֤���
*   ������λ����(�ݥ���)
*
****************************************************/
pat_node *pat_search(FILE *f, char *key, pat_node *x_ptr, char *rslt)
{
  pat_node *ptr,*tmp_ptr,*top_ptr = x_ptr,*tmp_x_ptr = x_ptr;
  int in_hash = 0;
  pat_index_list *tmp_l_ptr;
  int i;
  int key_length = strlen(key); /* ������ʸ����������Ƥ��� */
  char buffer[50000]; /* ���ѥХåե� */
  int totyu_match_len = 0; /* ����ǥޥå�����Prefix��ʸ���� */
  char *r;

  rslt += strlen(rslt);
  r = rslt;

/*  rslt[0] = '\0';*/

  /* OL(pat_search:\n); */
  do {
    ptr = x_ptr;
    /* �ߵ�ӥåȤʤ�� */
    OL(checkbit:)OI(ptr->checkbit);
    if(ptr->checkbit%SIKII_BIT==0 && ptr->checkbit!=0){ /* ����ñ���õ�� */
      tmp_x_ptr = ptr;
      do { /* ����ʬ�ڤΰ��ֺ��ΥΡ��ɤ�Ĵ�٤롥 */
	tmp_ptr = tmp_x_ptr;
	tmp_x_ptr = tmp_x_ptr->left;
      } while(tmp_ptr->checkbit < tmp_x_ptr->checkbit);

      /* �ϥå��������å� */
      in_hash = hash_check_proc(f,(tmp_x_ptr->il).index,buffer);
      strtok(buffer,"\t"); /* �ǽ�� '\t' �� '\0' �ˤ��롥*/
      /* buffer����Ƭ�Ρָ��Ф������ʬ�����ǥޥå��󥰤�Ԥʤ� */
      if(strncmp(key,buffer,ptr->checkbit/8) == 0) { /* ���Ĥ��� */
	totyu_match_len = ptr->checkbit/8; /* ����ǥޥå�����Prefix��ʸ���� */
	tmp_l_ptr = &(tmp_x_ptr->il); /* ���ꥹ�����Ǥμ��Ф� */
	while(tmp_l_ptr != NULL){
	  in_hash = hash_check_proc(f,tmp_l_ptr->index,buffer);
	  r = pat_strcpy(r, buffer);
	  *r++ = '\n';
	  *r = '\0';

	  tmp_l_ptr = tmp_l_ptr->next;
	}
      } else { /* ����Ǽ��Ԥ�ȯ�� */
	return x_ptr;
      }
    }

    /* key �� checkbit�ӥå��ܤǺ����˿���ʬ�� */
    if(pat_bits(key,x_ptr->checkbit,key_length)==1){x_ptr = x_ptr->right;}
    else {x_ptr = x_ptr->left;}

  } while(ptr->checkbit < x_ptr->checkbit);
  

  if(tmp_x_ptr != x_ptr || top_ptr == x_ptr) { /* ��λ�Ρ��ɤ�����å����� */
    char *s;
    int tmp_len;
    /* �ϥå��������å� */
    in_hash = hash_check_proc(f,(x_ptr->il).index,buffer);

    s = strchr(buffer,'\t'); /* �ǽ�� '\t' �� '\0' �ˤ��롥*/
    *s = '\0';
    tmp_len = s - buffer;/*���� */

    /* buffer����Ƭ�Ρָ��Ф������ʬ�����ǥޥå��󥰤�Ԥʤ� */
    if(strncmp(key,buffer,tmp_len) == 0){ /* �����ɤޤ�ñ���Prefix�����å� */
      if(totyu_match_len != key_length){ /* ���о��ñ�줫�ݤ��Υ����å� */
	tmp_l_ptr = &(x_ptr->il); /* ���ꥹ�����Ǥμ��Ф� */
	while(tmp_l_ptr != NULL){
	  in_hash = hash_check_proc(f,tmp_l_ptr->index,buffer);
	  r = pat_strcpy(r, buffer);
	  *r++ = '\n';
	  *r = '\0';

	  tmp_l_ptr = tmp_l_ptr->next;
	}
      }
    }
  }

  return x_ptr;
}


/****************************************************
* pat_search_exact --- �ѥȥꥷ���ڤ򸡺�(exact match)
* 
* �ѥ�᡼��
*   key --- ��������
*   x_ptr --- �������ϰ���(�ݥ���)
*   rslt --- ��̤�����롥
* 
* �֤���
*   ������λ����(�ݥ���)
****************************************************/
pat_node *pat_search_exact(FILE *f, char *key, pat_node *x_ptr, char *rslt)
{
  pat_node *ptr,*tmp_ptr,*top_ptr = x_ptr,*tmp_x_ptr = x_ptr;
  pat_index_list *tmp_l_ptr;
  int in_hash;
  int i;
  int key_length = strlen(key); /* ������ʸ����������Ƥ��� */
  char buffer[50000]; /* ���ѥХåե� */
  char *r;

  rslt += strlen(rslt);
  r = rslt;

  /*  printf("##");*/
  do {
    ptr = x_ptr;
    /* key �� checkbit�ӥå��ܤǺ����˿���ʬ�� */
    if(pat_bits(key,x_ptr->checkbit,key_length)==1){x_ptr = x_ptr->right;}
    else {x_ptr = x_ptr->left;}

  } while(ptr->checkbit < x_ptr->checkbit);

  /* �ե����뤫���ä���� */
  in_hash = hash_check_proc(f,(x_ptr->il).index,buffer);
  /*buffer = get_line(f,x_ptr->il_ptr->index);*/

  strtok(buffer,"\t"); /* �ǽ�� '\t' �� '\0' �ˤ��롥*/

  /* buffer����Ƭ�Ρָ��Ф������ʬ�����ǥޥå��󥰤�Ԥʤ� */
  if(strcmp(key,buffer) == 0){ /* �����ɤޤ�ñ��Υ����å� */
    tmp_l_ptr = &(x_ptr->il); /* ���ꥹ�����Ǥμ��Ф� */
    while(tmp_l_ptr != NULL){
      in_hash = hash_check_proc(f,tmp_l_ptr->index,buffer);
      r = pat_strcpy(r, buffer);
      *r++ = '\n';
      *r = '\0';

      tmp_l_ptr = tmp_l_ptr->next;
    }
  }

  return x_ptr;
}

/****************************************************
* pat_search4insert --- �����Ѥ˸���
* 
* �ѥ�᡼��
*   key --- ��������
*   x_ptr --- �������ϰ���(�ݥ���)
* 
* �֤���
*   ������λ����(�ݥ���)
*
* ���
*   ����ѿ� prefix_str �λؤ���˥ץ�ե��å���ʸ���������롣
****************************************************/
pat_node *pat_search4insert(char *key, pat_node *x_ptr)
{
  pat_node *ptr,*tmp_ptr,*tmp_x_ptr;
  int checked_char = 0; /* ��ʸ���ܤޤǥ����å������� patrie 960919 */
  int key_length = strlen(key); /* ������ʸ����������Ƥ��� */

  do {
    ptr = x_ptr;
    /* key �� checkbit�ӥå��ܤǺ����˿���ʬ�� */
    if(pat_bits(key,x_ptr->checkbit,key_length)==1){
      x_ptr = x_ptr->right; OL(R);}
    else {x_ptr = x_ptr->left; OL(L);}
  } while(ptr->checkbit < x_ptr->checkbit);
  OL(\n);
  return x_ptr;
}


/****************************************************
* pat_insert --- �ѥȥꥷ���ڤ˥ǡ���������
* 
* �ѥ�᡼��
*   f --- �ե�����
*   line --- �ǡ���(�������������Ƥ����ڤ�ʸ���Ƕ��ڤ��Ƥ��빽¤)
*   index --- �ǡ����Υե������Υ���ǥå���
*   x_ptr --- �����Τ���θ����γ��ϰ���
*   kugiri --- ���������Ƥζ��ڤ�ʸ��
* 
* �֤���
*   ̵��!
****************************************************/
void pat_insert(FILE *f,char *line, long index, pat_node *x_ptr, char *kugiri)
{
  pat_node *t_ptr, *p_ptr, *new_ptr;
  int diff_bit;
  int i;
  pat_index_list *new_l_ptr, *tmp_l_ptr, *mae_wo_sasu_ptr = NULL;
  int in_hash;
  int buffer_length;
  int key_length;
  char key[1000];
  char buffer[50000]; /* ���ѥХåե� */

  OL(line:)OS(line);
  strcpy(key,line);
  strtok(key,kugiri);  /* �ǽ�ζ��ڤ�ʸ���� '\0' �ˤ��롥*/
  key_length = strlen(key); /* ������ʸ����������Ƥ��� */

  OL(key:)OS(key);

  /* ������õ�� */
  t_ptr = (pat_node*)pat_search4insert(key,x_ptr);

  if((t_ptr->il).index >= 0) {
    /* �ϥå��������å� */
    in_hash = hash_check_proc(f,(t_ptr->il).index,buffer);

    if(strncmp(key,buffer,strlen(key)) == 0){ /* ���������� */
      /* printf("%s: ���������פ����Τ�����\n",buffer);fflush(stdout); */

      tmp_l_ptr = &(t_ptr->il);

      while(tmp_l_ptr !=NULL){
	in_hash = hash_check_proc(f,tmp_l_ptr->index,buffer);
	if(strcmp(buffer,line)==0){
	  /* ����Ʊ���Τ�����Τ����������˥꥿���� */
/*	  printf("%s: ����Ʊ���Τ�����Τ�̵��\n",buffer);*/
	  return;
	}
	mae_wo_sasu_ptr = tmp_l_ptr;
	tmp_l_ptr = tmp_l_ptr->next;
      }  /* ���λ����� tmp_l_ptr �ϥꥹ�Ȥ�������ؤ� */

      /* ���ˤ��륭�������Ƥ򤵤���������� */
      new_l_ptr = (pat_index_list*)malloc_pat_index_list(); /* index��list */
      new_l_ptr->index = index;
      new_l_ptr->next = NULL;
      mae_wo_sasu_ptr->next = new_l_ptr;

      return;
    } else { /* ���������פ��ʤ��ä���� buffer �ˤ��ΰ��פ��ʤ��ä����� */
    }
  } else { /* �ǡ�����̵���Ρ��ɤ���������: �ǽ�˥ǡ����򤤤줿�Ȥ� */
    *(buffer) = 0;*(buffer+1) = '\0';
  }


  /* ���������Ⱦ��ͤ��륭���Ȥδ֤Ǻǽ�˰ۤʤ� bit �ΰ���(diff_bit)����� */
  buffer_length = strlen(buffer);
  for(diff_bit=0; pat_bits(key,diff_bit,key_length) == pat_bits(buffer,diff_bit,buffer_length); diff_bit++)
    ;/* ��ʸ */

  OL(diff_bit:)OI(diff_bit);

  /* �������֤�����(x_ptr)����롣 */
  do {
    p_ptr = x_ptr;
    /* key �� checkbit�ӥå��ܤǺ����˿���ʬ�� */
    if(pat_bits(key,x_ptr->checkbit,key_length)==1) {x_ptr = x_ptr->right;}
    else {x_ptr = x_ptr->left;}
  } while((x_ptr->checkbit < diff_bit)&&(p_ptr->checkbit < x_ptr->checkbit));

  /* ��������Ρ��ɤ������������������ӥå��������ꤹ�롣 */
  new_ptr = (pat_node*)malloc_pat_node(); /* �Ρ������� */
  new_ptr->checkbit = diff_bit; /* �����å��ӥå� */
  (new_ptr->il).index = index;
  (new_ptr->il).next = NULL;

  /* ����ȿ�������ꤹ�롣 */
  /* �ӥåȤ�1�ʤ鱦��󥯤������Τ�����֤�ؤ���0�ʤ麸��󥯡� */
  if(pat_bits(key,new_ptr->checkbit,key_length)==1){
    new_ptr->right = new_ptr; new_ptr->left = x_ptr;
  } else {new_ptr->left = new_ptr; new_ptr->right = x_ptr;}
  /* �ӥåȤ�1�ʤ顢�Ƥα��ˤĤʤ���0�ʤ麸�� */
  if(pat_bits(key,p_ptr->checkbit,key_length)==1) p_ptr->right = new_ptr;
  else p_ptr->left = new_ptr;

  return;
}


/****************************************************
* pat_bits --- ʸ������λ��ꤵ�줿���֤ΥӥåȤ��֤�
* 
* �ѥ�᡼��
*   string --- ʸ����
*   cbit --- ���ꤵ�줿���֡�ʸ�������Τ��ĤΥӥå���ȹͤ���
*           ��Ƭ(��)bit���� 0,1,2,3... �ǻ��ꤹ�롣
*   len --- ʸ�����Ĺ����strlen�򤤤�������äƤ��󤸤����Ѥ����� 900918
*
* �֤���
*   0,1(�ӥå�),2(ʸ�����Ĺ�������ꤵ�줿���֤���礭���Ȥ�)
****************************************************/
int pat_bits(char *string, int cbit, int len)
{
  int moji_idx = cbit / 8; /* ���ꤵ�줿���֤���ʸ���ܤ� (for DEBUG)*/
  char moji = *(string+moji_idx); /* ����ʸ�� */
  int idx_in_moji = cbit % 8; /* ����ʸ���β��ӥå��ܤ� */
  if(cbit == -1) return 1; /* �ȥåץΡ��ɤΤȤ���1���֤�(top�����ɬ����) */
  if(len-1 < moji_idx) return 0;  /* ʸ�����Ĺ�� < ���ꤵ�줿���֤Υ����å� */
  return(((moji << idx_in_moji) & 0x80) >> 7); /* 0 or 1 ���֤��� */
}



/****************************************************
* hash_check_proc --- ����ǥå����ǥϥå�������
* 
* �ѥ�᡼��
*   index --- ����ǥå���
* 
* �֤���  �ϥå���ˤʤ���Хե����뤫���롥
*         ���ä���ʸ������Ƭ�ݥ��󥿡��ʤ���� NULL ( ���פ�? )
****************************************************/
int hash_check_proc(FILE *f, long index, char *buf) {
  char *data,key[40];
  long num_of_deleted = 0; /* �ä��줿�� */
  int i;

  /* ����å���̵���ξ�� */
#ifndef USE_HASH
  strcpy(buf, get_line(f,index));
  return(0);
#else
  if((data = th_hash_out( hash_array, HASH_SIZE, index, f)) == NULL) {
    strcpy(buf, get_line(f,index)); /* �ʤ���Хե����뤫���� */

    th_hash_in(hash_array,HASH_SIZE,index,buf,f);

    return(0);
  } else {
    strcpy(buf,data); /* ������Ѥ��� */
    return(1);
  }
#endif
}


/****************************************************
* get_line --- �ե������ pos ʸ���ܤ��� \n �ޤ��ɤ�
* 
* �ѥ�᡼��
*   f --- �ɤ�ե�����
*   pos --- �ɤ߹��߻Ϥ�����
*   buf --- �ɤ߹���Хåե�
* 
* �֤���
*   ʸ����(strlen����) 
*   -1 : ����
****************************************************/
char *get_line(FILE *f, long pos){
  int i = 0, j = 0, ch, ffd = fileno(f);
#ifdef HAVE_MMAP
  static int oldf = -1;
  static caddr_t addr;
  static off_t size;
  struct stat st;
#endif

#ifdef HAVE_MMAP
  if (oldf != ffd){
    for (i = 0; i < MAX_DIC_NUMBER; i++){
      if (ffd == dicinfo[i].fd && dicinfo[i].used){
	oldf = dicinfo[i].fd;
	addr = dicinfo[i].addr;
	size = dicinfo[i].size;
	break;
      }
      if (dicinfo[i].used == 0){
	dicinfo[i].fd   = ffd;
	dicinfo[i].used = 1;
	fstat(dicinfo[i].fd, &st);
	dicinfo[i].size = size = st.st_size;
	dicinfo[i].addr = addr = mmap(NULL, dicinfo[i].size, PROT_READ,
				      MAP_PRIVATE, dicinfo[i].fd, 0);
	break;
      }
    }
    if (i == MAX_DIC_NUMBER){
      exit(1);
    }
    oldf = ffd;
  }

  if (pos >= size)
    return NULL;

#if 1
  return addr + pos;
#else
#if 1
  {
      char *b = buf;
      char *a = addr + pos;
      i = 0;
      while (*a && *a != '\n') {
	  *b++ = *a++;
	  i++;
      }
      *b = '\0';
  }
#else
  for (i = 0; addr[pos+i] && addr[pos+i] != '\n'; i++)
    buf[i] = addr[pos+i];
  buf[i] = 0;
#endif

  return i+1;
#endif

#else
    if(fseek(f, pos, 0) == 0){
      static char buf[2000];
      if(NULL == fgets(buf,sizeof(buf),f))
	return NULL;
      return buf;
    }
    else return NULL; /* seek ���� */
#endif
}


/****************************************************
* show_pat --- �ѥȥꥷ���ڥǡ��������
*
* �ѥ�᡼��
*   top_ptr --- �������ϥΡ��ɤΰ���(�ݥ���)
*   out_to --- ������(stdout��ե�����)
* 
* �֤���
*   ̵�����ѥȥꥷ���ڥǡ�������ϡ�
****************************************************/
void show_pat(pat_node *top_ptr, FILE *out_to, char *prefix)
{
#if 0
  long idx = -1;
  pat_index_list *t_ptr;
  char word[200];
  char pftmp[200];
  char prefix_keep[200];

  word[0] = '\0';

  strcpy(prefix_keep,prefix);

  OL(-------\n);
  OL(prefix:)OS(prefix);
  OL(<checkbit>)OI(top_ptr->checkbit);

  OL(## ��\n)
  /* �ߵ�ӥåȤΤȤ� */
  if(top_ptr->checkbit % SIKII_BIT == 0 && top_ptr->checkbit != 0){
    strcpy(word, get_line(dic_file[0],top_ptr->left->il_ptr->index));
    strtok(word,"\t");
    OL(SIKIIbitProcess\n)
    OL(SIKIIword:)OS(word);
    strcpy(pftmp,(word+strlen(prefix)));
    OL(keep:)OS(pftmp);

/*
    printf("#@# %i\n",strlen(word));
    printf("### %i\n",strlen(pftmp));

    top_ptr->left->str = (char*)malloc(strlen(word)+1);
    strcpy(top_ptr->left->str,word);
*/
    top_ptr->left->str = (char*)malloc(strlen(pftmp)+1);
    strcpy(top_ptr->left->str,pftmp);

    strcat(prefix,pftmp);

    OS(pftmp);

  } else {
    /* ������ Subtree �ν������դäѤǤʤ���кƵ���*/
    if(top_ptr->checkbit < top_ptr->left->checkbit){
      show_pat(top_ptr->left,out_to,prefix);}
    else {
      if(top_ptr->left->il_ptr != NULL) {
	strcpy(word, get_line(dic_file[0],top_ptr->left->il_ptr->index));
	strtok(word,"\t");
	OL(word:)OS(word);
	strcpy(pftmp,(word+strlen(prefix)));
	OL(keep:)OS(pftmp);

/*
    printf("#@# %i\n",strlen(word));
    printf("### %i\n",strlen(pftmp));

    top_ptr->left->str = (char*)malloc(strlen(word)+1);
    strcpy(top_ptr->left->str,word);
*/
	top_ptr->left->str = (char*)malloc(strlen(pftmp)+1);
	strcpy(top_ptr->left->str,pftmp);

	OS(word);
      }
    }

  }

  OL(## ��\n)
  if(top_ptr->checkbit < top_ptr->right->checkbit){
    show_pat(top_ptr->right,out_to,prefix);}
  else {
    if(top_ptr->right->il_ptr != NULL) {
      strcpy(word, get_line(dic_file[0],top_ptr->right->il_ptr->index));
      strtok(word,"\t");
      OL(word:)OS(word);
      strcpy(pftmp,(word+strlen(prefix)));
      OL(keep:)OS(pftmp);

/*
    printf("#@# %i\n",strlen(word));
    printf("### %i\n",strlen(pftmp));

    top_ptr->left->str = (char*)malloc(strlen(word)+1);
    strcpy(top_ptr->left->str,word);
*/
      top_ptr->right->str = (char*)malloc(strlen(pftmp)+1);
      strcpy(top_ptr->right->str,pftmp);

      OS(word);
    }
  }

  OL(---------back-------\n);

  strcpy(prefix,prefix_keep);
  return;
#endif
}

