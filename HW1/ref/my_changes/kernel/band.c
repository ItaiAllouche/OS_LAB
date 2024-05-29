#include <linux/band.h>

struct list_head g_list_band;
int first_iteration = 1;

//  initialize a Band
void initializeBand(struct Band *band, int instrument, int pid)
{
    if (first_iteration)
    {
        first_iteration = 0;
        INIT_LIST_HEAD(&g_list_band);
    }

    if (band)
    {
        int i;
        for (i = 0; i < 4; i++)
        {
            band->curr_pids[i] = -1;
            band->chord[i] = 0; // like null in terms of char
        }
        band->curr_pids[instrument] = pid;
        INIT_LIST_HEAD(&band->list);
        band->all_played = 0;
        band->counter_notes = 0;
    }
}

struct Band *findBand(pid_t member)
{
    if (find_task_by_pid(member) == NULL)
    {
        return NULL;
    }

    struct list_head *cursor;
    struct Band *band;

    list_for_each(cursor, &g_list_band)
    {

        band = list_entry(cursor, struct Band, list);
        int i;
        for (i = 0; i < 4; i++)
        {
            if(band != NULL){

            if (band->curr_pids[i] == member)
            {

                return band;
            }
            }
        }
    }

    return NULL; // Return NULL only if no matching band was found in the entire list.
}

int band_join(pid_t member, int instrument)
{
    if (instrument > 3 || instrument < 0)
    {
        return -EINVAL;
    }

    struct Band *band;
    struct Band *band_of_curr_process;
    if (member == 0)
    {
        band = findBand(current->pid);
        if (band != NULL)
        {
            if (band->curr_pids[instrument] == -1)
            {
                band->curr_pids[instrument] = current->pid;
                return 0;
            }
            else
            {
                return -ENOSPC;
            }
        }
    }

    // Check if the member process exists
    if (find_task_by_pid(member) == NULL)
    {
        return -EINVAL;
    }

    // Find the band that the member belongs to
    band = findBand(member);
    band_of_curr_process = findBand(current->pid);
    if (band != NULL)
    {
        int k;
        // Check if the instrument is available
        if (band->curr_pids[instrument] == -1)
        {
            if (band_of_curr_process != NULL) // firstable the process have to quit from its current band
            {
                int i;
                for (i = 0; i < 4; i++)
                {

                    k = 0;
                    if (band_of_curr_process->curr_pids[i] == current->pid)
                    {
                        band_of_curr_process->curr_pids[i] = -1;
                        band_of_curr_process->counter_notes--;
                    }
                    if (band_of_curr_process->curr_pids[i] == -1)
                    {
                        ++k;
                    }
                }

                if (k == 4) // if this process was the last member in this band
                {
                    list_del(&band_of_curr_process->list);
                    kfree(band_of_curr_process);
                }
            }

            band->curr_pids[instrument] = current->pid;
            return 0; // Successfully joined the band
        }
        else
        {
            return -ENOSPC; // Instrument is already allocated
        }
    }
    else
    {
        return -EINVAL; // Member is not in a band
    }
}

int band_create(int instrument)
{

    if (instrument > 3 || instrument < 0)
    {
        return -EINVAL;
    }
    struct Band *new_band = kmalloc(sizeof(struct Band), GFP_KERNEL);
    if (!new_band)
    {

        // memory allocation failure
        return -ENOMEM;
    }

    initializeBand(new_band, instrument, current->pid);

    list_add_tail(&new_band->list, &g_list_band);

    return 0; // my add
}

int band_play(int instrument, unsigned char note)
{
    struct Band *band = findBand(current->pid);
    if (instrument > 3 || instrument < 0)
    {

        return -EINVAL;
    }
    if (band == NULL)
    {

        return -ENOENT;
    }

    if (band->curr_pids[instrument] != current->pid)
    {

        return -EACCES;
    }
    if (band->chord[instrument] != 0)
    {

        return -EBUSY;
    }
    if (band->chord[instrument] == 0 && band->all_played == 0)
    {

        band->chord[instrument] = note;
        band->counter_notes++;
        if (band->counter_notes == 4)
        {

            band->all_played = 1;
        }
        return 0; // my add
    }

    return -1; // my add
}
int band_listen(pid_t member, unsigned char *chord)
{
    struct Band *band;
    int k, i;
    if (member == 0)
    {
        band = findBand(current->pid);
        if (band == NULL)
        {
            return -EINVAL;
        }
        else
        {
            if (band->all_played == 1)
            {
                i = 0;
                for (i = 0; i < 4; i++)
                {
                    chord[i] = band->chord[i];
                }

                k = 0;
                for (k = 0; k < 4; k++)
                {
                    band->chord[k] = 0;
                }
                band->counter_notes = 0;
                band->all_played = 0;
                return 0;
            }
        }
    }
    struct task *task = find_task_by_pid(member); // returns NULL if failed, else it returns task object
    if (task == NULL)
    {
        return -ESRCH;
    }
    band = findBand(member);
    if (chord == NULL)
    {
        return -EFAULT;
    }
    if (band == NULL)
    {
        return -EINVAL;
    }
    if (band->all_played == 1)
    {
        i = 0;
        for (i = 0; i < 4; i++)
        {
            chord[i] = band->chord[i];
        }

        k = 0;
        for (k = 0; k < 4; k++)
        {
            band->chord[k] = 0;
        }
        band->counter_notes = 0;
        band->all_played = 0;
        return 0;
    }
    else
    {
        return -EAGAIN;
    }
    return -1; // my add
}

void delete_process_done(pid_t curr_process)
{
    struct Band *band;
    band = findBand(curr_process);
    if (band == NULL)
    {
        return;
    }
    else
    {
        int i;
        for (i = 0; i < 4; i++)
        {
            if (band->curr_pids[i] == curr_process)
            {
                band->curr_pids[i] = -1;
                band->counter_notes--;
                if (band->counter_notes == 0)
                {

                    list_del(&band->list);
                    kfree(band);
                }
            }
        }
    }
    return;
}
