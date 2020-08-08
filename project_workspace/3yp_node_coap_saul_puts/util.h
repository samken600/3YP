#ifndef UTIL_H
#define UTIL_H

#include "phydat.h"
#include "xtimer.h"
#include <stdlib.h>

#define MAX_SENDS_PER_PERIOD  8
#define MAX_READING_LIST_SIZE 128
#define PHYSICAL_NETIF 6
#define COAP_ADDR {0xfd, 0x00, 0xde, 0xad, 0xbe, 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}

extern int32_t epoch_offset;

/* structures for singly linked list */
typedef struct reading_node {
    phydat_t temperature;
    uint32_t time;
    struct reading_node *next;
} reading_node_t;

typedef struct {
    reading_node_t *head;
    uint16_t length;
} reading_list_t;

/* remove reading "node" from singly linked list "list"  */
static inline void reading_remove(reading_list_t *list, reading_node_t *node) {
    reading_node_t *head = list->head;

    /* return if queue empty */
    if(list->head == NULL) return;

    if(list->head == node) {
        /* move head to next node */
        list->head = list->head->next;
        /* free the head */
        free(head);
        return;
    }
    
    /* find node in list */
    while(head->next != node && head!=NULL) 
        head = head->next;

    /* return if not found */
    if(head==NULL) return;

    /* store pointer to element after node */
    reading_node_t *temp = head->next->next;
    
    /* free node */
    free(head->next);

    /* join up list again */
    head->next = temp;
    
    list->length = list->length - 1;
    return;
}

/* add reading to singly linked list "list" with temperature "temp" and timestamp "time" */
static inline void reading_add(reading_list_t *list, phydat_t temp, uint32_t time) {
    reading_node_t *new_node;
    reading_node_t *last_node = list->head;
    
    /* allocate memory */
    new_node = malloc(sizeof(reading_node_t));

    /* fill data */
    new_node->temperature = temp;
    new_node->time = time;
    
    /* node is being appended */
    new_node->next = NULL;

    /* if list empty, add node */
    if(list->head == NULL) {
        list->head = new_node;
        list->length = 1;
        return;
    }

    /* find end of list */
    while(last_node->next != NULL) {
        last_node = last_node->next;
    }

    /* add in new node */
    last_node->next = new_node;

    /* cap size of list */
    list->length++;
    if(list->length > MAX_READING_LIST_SIZE) {
        reading_remove(list, list->head);
    }
    return;
}

/* send get request and use time returned to update epoch offset */
ssize_t update_epoch(void);

/* make temperature reading, add to queue, and post to server */
ssize_t get_reading(void);

/* set period of xtimer xt. restarts timer */
void set_period(xtimer_t *xt, const uint32_t offset, uint32_t *period);

#endif
