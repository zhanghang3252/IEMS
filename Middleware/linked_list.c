#include "linked_list.h"

/*
 *创建普通节点默认创建头节点和尾节点
 *输入：无
 *输出：头节点地址
 */
 
linked_list *link_list_creat(void){
    linked_list *head, *end;//定义头节点，尾部节点；
    head = (linked_list*)	malloc(sizeof(linked_list));//分配地址
    end = (linked_list*)	malloc(sizeof(linked_list));//分配地址
	
    head->id=1;//头节点id为1
    head->on = end;//头节点的上一个为尾节点
		head->data = 0;
	
    end->id=0;//尾节点的id为普通节点个数0
    end->next=head;//尾节点的下一个为头节点
		end->data = 0;
    return head;//返回头节点地址
}

/*
 * 插入新节点节点
 * 输入：
 * head_list：节点
 * new_list：要插入的节点数据
 * 输出：新节点的地址
 */
linked_list * link_list_inserted(linked_list *head_list,void *new_data) 
{
	linked_list *new_list = (linked_list*)	malloc(sizeof(linked_list));//分配地址
	head_list->next = new_list;
	new_list->id = head_list->id+1;
	new_list->data = new_data;
	return new_list;
}


