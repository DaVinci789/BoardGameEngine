#include "bucket.h"
#include "frame.h"
#include <stdlib.h>
#include <string.h>

#define MAX_SLOTS 1 << 16 
#define NUM_BUCKETS 8

static frame_h **buckets = NULL;
static Bucket *buckets_update = NULL;
static int buckets_update_len = 0;

void init_buckets()
{
    buckets = calloc(NUM_BUCKETS, sizeof(*buckets));
    for (int i = 0; i < NUM_BUCKETS; i++) {
        buckets[i] = calloc(MAX_SLOTS, sizeof(**buckets));
    }

    buckets_update = calloc(MAX_SLOTS, sizeof(*buckets_update));
}

Bucket* register_bucket(BucketCallback query, void *context)
{
    Bucket *b = &buckets_update[buckets_update_len];
    buckets_update_len += 1;

    b->index = buckets_update_len - 1;
    b->len = 0;
    b->query = query;
    b->context = context;
    return b;
}

void update_buckets()
{
    for (int i = 0; i < MAX_FRAMES; i++) {
        Rectangle r = rectangle_list()[i];
        FrameAttr a = attr_list()[i];
        for (int u = 0; u < buckets_update_len; u++) {
            Bucket *update = &buckets_update[u];
            if (update->query(r, a, update->context)) {
                buckets[update->index][(update->index % NUM_BUCKETS) + update->len - 1] = a.id;
                update->len += 1;
            }
        }
    }
}

frame_h* get_bucket(Bucket b)
{
    return buckets[b.index];
}

void reset_buckets()
{
    memset(buckets, 0, MAX_SLOTS * NUM_BUCKETS * sizeof(**buckets));
    for (int i = 0; i < buckets_update_len; i++) {
        buckets_update[i].len = 0;
    }
}