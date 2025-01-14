/*
 * Copyright 2025 Nuo Shen, Nanjing University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <common.h>

#define QUEUE_SIZE 262144

// [front, rear)
typedef struct {
    volatile uint32_t data[QUEUE_SIZE];
    volatile int front;
    volatile int rear;
} queue32_t;

__attribute__((always_inline))
static inline void queue32_init(volatile queue32_t *q) {
    q->front = 0;
    q->rear = 0;
}

__attribute__((always_inline))
static inline bool queue32_is_empty(volatile queue32_t *q) {
    return q->front == q->rear;
}

__attribute__((always_inline))
static inline bool queue32_is_full(volatile queue32_t *q) {
    return (q->rear + 1) % QUEUE_SIZE == q->front;
}

__attribute__((always_inline))
static inline bool queue32_enqueue(volatile queue32_t *q, uint32_t value) {
    if (queue32_is_full(q)) {
        Panic("queue is full!");
        return false;
    }
    for (int i = q->front; i <= q->rear; i++) {
        if (q->data[i] == value) {
            return false;
        }
    }
    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % QUEUE_SIZE;
    return true;
}

__attribute__((always_inline))
static inline uint32_t queue32_dequeue(volatile queue32_t *q) {
    if (queue32_is_empty(q)) {
        return -1;
    }
    uint32_t ret = q->data[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    return ret;
}

#endif
