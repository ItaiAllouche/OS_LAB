
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/list.h>
#ifndef _BAND_h

struct Band
{
    pid_t curr_pids[4]; // each index in this array represent an instrument
    unsigned char chord[4];
    int counter_notes;
    int all_played;
    struct list_head list;
};
void initializeBand(struct Band *band, int instrument, int pid);
struct Band *findBand(pid_t member);
int band_join(pid_t member, int instrument);
int band_create(int instrument);
int band_play(int instrument, unsigned char note);
int band_listen(pid_t member, unsigned char *chord);
void delete_process_done(pid_t curr_process);
#define _BAND_h
#endif
