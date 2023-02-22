#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);

    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    struct list_head *li, *safe;

    list_for_each_safe (li, safe, l)
        free(li);
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *first = malloc(sizeof(element_t));
    if (!first)
        return false;

    first->value = strdup(s);
    if (!first->value) {
        free(first);
        return false;
    }
    list_add(&first->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *last = malloc(sizeof(element_t));
    if (!last)
        return false;

    last->value = strdup(s);
    if (!last->value) {
        free(last);
        return false;
    }
    list_add_tail(&last->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_first_entry(head, element_t, list);

    list_del(&node->list);
    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_last_entry(head, element_t, list);
    list_del(&node->list);
    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *fast = head->next;
    struct list_head *slow = fast;

    while (fast != head->prev && fast->next != head->prev) {
        fast = fast->next->next;
        slow = slow->next;
    }

    list_del(slow);
    q_release_element(container_of(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    element_t *cur, *next = NULL;
    bool dup = false;  // for last duplicate string

    list_for_each_entry_safe (cur, next, head, list) {
        // strcmp: if two string equal return 0
        // current value equal to next value
        if (!strcmp(cur->value, next->value)) {
            list_del(&cur->list);
            q_release_element(cur);
            dup = true;
        }
        // delete last duplicate node
        else if (dup) {
            list_del(&cur->list);
            q_release_element(cur);
            dup = false;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;

    struct list_head *cur, *next, *first = NULL;

    list_for_each_safe (cur, next, head) {
        if (first) {
            // add first behind second
            list_add(first, cur);
            first = NULL;
        } else {
            // remove first one
            first = cur;
            list_del(cur);
        }
    }
    // last node
    if (first)
        list_add(first, head->prev);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *cur, *next = NULL;

    list_for_each_safe (cur, next, head) {
        list_del(cur);
        list_add(cur, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;

    int count = 0;
    struct list_head *cur, *next = NULL;

    list_for_each_safe (cur, next, head) {
        count++;
        if (count % k == 0) {
            count--;
            struct list_head *tmp = cur->prev;
            struct list_head *tmp_prev;
            while ((count--) > 0) {
                tmp_prev = tmp->prev;
                list_del(tmp);
                list_add(tmp, cur);

                cur = cur->next;
                tmp = tmp_prev;
            }
            count = 0;
        }
    }
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_is_singular(head))
        return;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    struct list_head *cur = head->prev;
    struct list_head *prev = cur->prev;
    char *max = NULL;

    // iterate list reversly
    for (; cur != head; cur = prev) {
        element_t *entry = list_entry(cur, element_t, list);
        prev = cur->prev;
        if (!max || strcmp(entry->value, max) > 0)
            max = entry->value;
        else {
            list_del(&entry->list);
            q_release_element(entry);
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    /*int size = q_size(head);

    if (size <=1)
    return size;
    */
    return 0;
}
