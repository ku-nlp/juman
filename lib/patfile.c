#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#ifdef HAVE_FCNTL_H
#include 	<fcntl.h>
#endif

#ifdef HAVE_STDLIB_H
#include        <stdlib.h>
#endif

#include "juman_pat.h"
#ifndef _WIN32
#define O_BINARY (0)
#endif

int fd_pat; /* �ѥ��ڤΥ�����/�����ѥե�����ǥ�������ץ� */

/******************************************************
* com_l --- �ڤΥ���
*  by �����û�(keiji-y@is.aist-nara.ac.jp)
*
* �ѥ�᡼�����֤���
*   �ʤ�
******************************************************/
void com_l(char *fname_pat, pat_node *ptr){
/*  fprintf(stderr, "# Loading pat-tree \"%s\" ... ",fname_pat); */
  if ((fd_pat = open(fname_pat, O_RDONLY|O_BINARY)) == -1) {
    fprintf(stderr, "�ե����� %s �������ץ����ޤ���\n",fname_pat);
    exit(1);
  }
  OL(fd_pat)OI(fd_pat)
  ptr->right = load_anode(ptr);
  close(fd_pat);
/*  fprintf(stderr,"done.\n"); */
}


/****************************************************
* load_anode --- �ѥȥꥷ���ڤ����
*  by �����û�(keiji-y@is.aist-nara.ac.jp)
*
* �ѥ�᡼��
*   in --- ������ե�����
*   p_ptr --- ���ΥΡ��ɤ����������Ǥ��ä����˥���ǥå������Ǽ������
*             ���������Ǥ��ä��Ȥ��ϡ����Υݥ��󥿤ϱ��λҤ��Ϥ���롣
*
* ���르�ꥺ��
*   �����å��ӥåȤ��ɤ߹�����顢������������������鿷�����Ρ��ɤ���
*     ����ʬ�ڡ�����ʬ�ڤν�˺Ƶ�����
*     ���Ƶ��λ��Ͽ�������ä����������Υݥ��󥿤�
*     ���Ƶ��λ��� p_ptr �򥤥�ǥå����γ�Ǽ���Ȥ����Ϥ���
*   ����ǥå������ɤ߹�����顢����ϳ������������顢p_ptr->index �˳�Ǽ
*
* ���
*   ����ǥå����γ�Ǽ��꤬���Ȱ㤦�����ä�����ʤ���
*************************************************************************/
pat_index_list *malloc_pat_index_list()
{
    static int  idx = 1024;
    static char *ptr;

    if (idx == 1024) {
	ptr = malloc(sizeof(pat_index_list) * idx);
	idx = 0;
    }

    return (pat_index_list *)(ptr + sizeof(pat_index_list) * idx++);
}

pat_node *malloc_pat_node()
{
    static int  idx = 1024;
    static char *ptr;

    if (idx == 1024) {
	ptr = malloc(sizeof(pat_node) * idx);
	idx = 0;
    }

    return (pat_node *)(ptr + sizeof(pat_node) * idx++);
}

pat_node *load_anode(pat_node *p_ptr){
  unsigned char c;
  pat_node *new_ptr; /* ��������ä��Ρ���(==���ΥΡ���)��ؤ��ݥ��� */
  long tmp_idx;
  pat_index_list *new_l_ptr,*t_ptr=NULL;

  if ((c = egetc(fd_pat)) & 0x80) { /* �դäѤν���������ǥå������ɤ߹��� */
    while(c & 0x80) {
      tmp_idx = (c & 0x3f) << 24;
      tmp_idx |= egetc(fd_pat) << 16;
      tmp_idx |= egetc(fd_pat) << 8;
      tmp_idx |= egetc(fd_pat);

      if((p_ptr->il).index < 0)
	new_l_ptr = &(p_ptr->il);
      else {
	new_l_ptr = malloc_pat_index_list();
	t_ptr->next = new_l_ptr;
      }
      new_l_ptr->index = tmp_idx;
      new_l_ptr->next = NULL;
      t_ptr = new_l_ptr;

      if(c & 0x40) break;
      c = egetc(fd_pat);
    }

    return (p_ptr);
  }
  else { /* ���������ν������Ƶ����� */
    new_ptr = malloc_pat_node();
    new_ptr->checkbit = ((c << 8) | egetc(fd_pat)) - 1; /* �����å��ӥå� */
/*    printf("#cb %d\n",new_ptr->checkbit);*/
    (new_ptr->il).index = -1;
    new_ptr->left = load_anode(new_ptr);
    new_ptr->right = load_anode(p_ptr);
    return (new_ptr);
  }
}

unsigned char egetc(int file_discripter){
  static int fd_pat_check = -1;
  static char buf[BUFSIZ];
  static int ctr = sizeof(buf) - 1;

  if(file_discripter != fd_pat_check) { /* �Хåե��ν���� */
    fd_pat_check = file_discripter;
    ctr = sizeof(buf) - 1;
  }

  if(++ctr == sizeof(buf)){
    ctr = 0;
    read(file_discripter, buf, sizeof(buf));
/* OL(.);fflush(stdout);*/
  }

  return(buf[ctr]);
}


/*****************************************************
* com_s --- �ڤΥ����� 
*  by �����û�(keiji-y@is.aist-nara.ac.jp)
*
* �ѥ�᡼�����֤���
*   �ʤ�
*****************************************************/
void com_s(char *fname_pat, pat_node *ptr){
  int i;

  printf("Saving pat-tree \"%s\" ...\n",fname_pat);
  if ((fd_pat = open(fname_pat, O_WRONLY|O_CREAT|O_BINARY, 0644)) == -1) {
    fprintf(stderr, "�ե����� %s �������ץ����ޤ���\n", fname_pat);
    exit(1);
  }; 
  save_pat(ptr->right); /* �ե�������� */
  for(i = 0; i < BUFSIZ; i++)
    eputc(0, fd_pat); /* flush */
  close(fd_pat);
}


/****************************************************
* save_pat --- �ѥȥꥷ���ڥǡ����򥻡��� 
*  by �����û�(keiji-y@is.aist-nara.ac.jp)
*
* �ѥ�᡼��
*   top_ptr --- �������ϥΡ��ɤΰ���(�ݥ���)
*   out_to --- ������(stdout��ե�����)
* 
* �֤���
*   ̵�����ѥȥꥷ���ڥǡ�������ϡ�
*
* ���ϥե����ޥå� --- 8�ӥåȤ˶��ڤäƥХ��ʥ����
*   ��ͥ��õ�������������ϥ����å��ӥåȡ����������ϥ���ǥå��������
*   �����å��ӥå� --- ����Ū�ˤ��Τޤ� (�� 0 �ӥåȤ� 0)
*     ������ -1 �ΤȤ�����Τ� 1 ��­��
*   ����ǥå��� --- �� 0 �ӥåȤ� 1 �ˤ���
****************************************************/
void save_pat(pat_node *top_ptr)
{
  pat_index_list *ptr;
  long out_idx;
  /* ���������ν����������å��ӥåȤ���� */
  eputc (((top_ptr->checkbit + 1)>> 8) & 0x7f, fd_pat);
  eputc ((top_ptr->checkbit + 1)& 0xff, fd_pat);

  /* ������ Subtree �ν������դäѤʤ饤��ǥå�������ϡ�
     �դäѤǤʤ���кƵ���*/
  if(top_ptr->checkbit < top_ptr->left->checkbit)
    save_pat(top_ptr->left);
  else {
    ptr = &(top_ptr->left->il);
    if(ptr->index < 0) dummy();
    else {
      while(ptr != NULL) {
	if(ptr->next == NULL) eputc (((ptr->index >> 24) & 0x3f) | 0xc0, fd_pat);
	else eputc (((ptr->index >> 24) & 0x3f) | 0x80, fd_pat);
	eputc ((ptr->index >> 16) & 0xff, fd_pat);
	eputc ((ptr->index >> 8) & 0xff, fd_pat);
	eputc ((ptr->index) & 0xff, fd_pat);
	ptr = ptr->next;
      }
    }
  }
  if(top_ptr->checkbit < top_ptr->right->checkbit)
    save_pat(top_ptr->right);
  else {
    ptr = &(top_ptr->right->il);
    if(ptr->index < 0) dummy();
    else {
      while(ptr != NULL) {
	if(ptr->next == NULL) eputc (((ptr->index >> 24) & 0x3f) | 0xc0, fd_pat);
	else eputc (((ptr->index >> 24) & 0x3f) | 0x80, fd_pat);
	eputc ((ptr->index >> 16) & 0xff, fd_pat);
	eputc ((ptr->index >> 8) & 0xff, fd_pat);
	eputc ((ptr->index) & 0xff, fd_pat);
	ptr = ptr->next;
      }
    }
  }

  return;
}

void dummy() {
  eputc(0xff,fd_pat);eputc(0xff,fd_pat);eputc(0xff,fd_pat);eputc(0xff,fd_pat);
}

void eputc(unsigned char c, int file_discripter){
  static int ctr = 0;
  static unsigned char buf[BUFSIZ];

  buf[ctr] = (char) c;
  ctr++;

  if(ctr == BUFSIZ){
    ctr = 0;
    write(file_discripter, buf, BUFSIZ);
  }

  return;
}
