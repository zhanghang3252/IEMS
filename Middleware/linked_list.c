#include "linked_list.h"

/*
 *������ͨ�ڵ�Ĭ�ϴ���ͷ�ڵ��β�ڵ�
 *���룺��
 *�����ͷ�ڵ��ַ
 */
 
linked_list *link_list_creat(void){
    linked_list *head, *end;//����ͷ�ڵ㣬β���ڵ㣻
    head = (linked_list*)	malloc(sizeof(linked_list));//�����ַ
    end = (linked_list*)	malloc(sizeof(linked_list));//�����ַ
	
    head->id=1;//ͷ�ڵ�idΪ1
    head->on = end;//ͷ�ڵ����һ��Ϊβ�ڵ�
		head->data = 0;
	
    end->id=0;//β�ڵ��idΪ��ͨ�ڵ����0
    end->next=head;//β�ڵ����һ��Ϊͷ�ڵ�
		end->data = 0;
    return head;//����ͷ�ڵ��ַ
}

/*
 * �����½ڵ�ڵ�
 * ���룺
 * head_list���ڵ�
 * new_list��Ҫ����Ľڵ�����
 * ������½ڵ�ĵ�ַ
 */
linked_list * link_list_inserted(linked_list *head_list,void *new_data) 
{
	linked_list *new_list = (linked_list*)	malloc(sizeof(linked_list));//�����ַ
	head_list->next = new_list;
	new_list->id = head_list->id+1;
	new_list->data = new_data;
	return new_list;
}


