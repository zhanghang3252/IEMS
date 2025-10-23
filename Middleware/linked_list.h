#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>

//是否使用循环链表
#define USE_CIRCULAR_LIST 0;
/*节点结构体*/
typedef struct linked_list{
	int id;//节点ID
	struct linked_list *next;//下一个节点
	struct linked_list *on;//上一个节点
	void* data;//节点内容
} linked_list;


linked_list *link_list_creat(void);//创建链表
linked_list *link_list_inserted(linked_list *head_list,void *new_data);//插入新节点
#endif



