#ifndef BUCKET_H
#define BUCKET_H

#include <raylib.h>
#include "frame.h"

#define MAX_SLOTS 1 << 16 
#define NUM_BUCKETS 8

typedef int (*BucketCallback)(Rectangle rect, FrameAttr attr, void *ctx);

typedef struct Bucket {
    int index;
    int len;
    BucketCallback query;
    void *context;
} Bucket;

void init_buckets();
Bucket *register_bucket(BucketCallback query, void *context);
frame_h* get_bucket(Bucket b);
void update_buckets();
void reset_buckets();
#endif