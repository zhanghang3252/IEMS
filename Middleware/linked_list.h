#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>

//�Ƿ�ʹ��ѭ������
#define USE_CIRCULAR_LIST 0;
/*�ڵ�ṹ��*/
typedef struct linked_list{
	int id;//�ڵ�ID
	struct linked_list *next;//��һ���ڵ�
	struct linked_list *on;//��һ���ڵ�
	void* data;//�ڵ�����
} linked_list;


linked_list *link_list_creat(void);//��������
linked_list *link_list_inserted(linked_list *head_list,void *new_data);//�����½ڵ�
#endif



