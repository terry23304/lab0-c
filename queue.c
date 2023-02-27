#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    if (!l)
        return;
    element_t *li, *safe;

    list_for_each_entry_safe (li, safe, l, list) {
        list_del(&li->list);
        q_release_element(li);
    }

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

    struct list_head *cur, *next = NULL;
    bool dup = false;  // for last duplicate string

    list_for_each_safe (cur, next, head) {
        // current value equal to next value
        element_t *cur_node = list_entry(cur, element_t, list);

        if (cur->next == head) {
            if (dup) {
                list_del(&cur_node->list);
                q_release_element(cur_node);
            }
            break;
        }
        element_t *next_node = list_entry(cur->next, element_t, list);

        if (!strcmp(cur_node->value, next_node->value)) {
            list_del(&cur_node->list);
            q_release_element(cur_node);
            dup = true;
        }
        // delete last duplicate node
        else if (dup) {
            list_del(&cur_node->list);
            q_release_element(cur_node);
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
    if (!head || list_empty(head))
        return;

    int count = 0;
    struct list_head *cur, *next = NULL;

    list_for_each_safe (cur, next, head) {
        count++;
        if (count == k) {
            count--;
            struct list_head *tmp = cur->prev;
            struct list_head *tmp_prev;
            while ((count--) > 0) {
                tmp_prev = tmp->prev;
                list_move(tmp, cur);

                cur = cur->next;
                tmp = tmp_prev;
            }
            count = 0;
        }
    }
}

struct list_head *mergeTwoList(struct list_head *L1, struct list_head *L2)
{
    struct list_head *head = NULL, **ptr = &head, **node;

    for (node = NULL; L1 && L2; *node = (*node)->next) {
        element_t *L1_node = list_entry(L1, element_t, list);
        element_t *L2_node = list_entry(L2, element_t, list);
        if (strcmp(L1_node->value, L2_node->value) < 0)
            node = &L1;
        else
            node = &L2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) L1 | (uintptr_t) L2);
    return head;
}

struct list_head *mergesort_list(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *slow = head, *fast = head->next;

    for (; fast && fast->next; fast = fast->next->next)
        slow = slow->next;
    struct list_head *mid = slow->next;
    slow->next = NULL;

    struct list_head *left = mergesort_list(head), *right = mergesort_list(mid);
    return mergeTwoList(left, right);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    head->prev->next = NULL;
    head->next = mergesort_list(head->next);

    struct list_head *prev = head, *cur = head->next;

    while (cur != NULL) {
        cur->prev = prev;
        cur = cur->next;
        prev = prev->next;
    }

    prev->next = head;
    head->prev = prev;
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
            list_del(cur);
            q_release_element(entry);
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return list_entry(head, queue_contex_t, chain)->size;

    queue_contex_t *first = list_entry(head->next, queue_contex_t, chain);
    queue_contex_t *cur = NULL;
    list_for_each_entry (cur, head, chain) {
        if (cur == first)
            continue;
        list_splice_init(cur->q, first->q);
        first->size = first->size + cur->size;
        cur->size = 0;
    }
    q_sort(first->q);

    return first->size;
}
