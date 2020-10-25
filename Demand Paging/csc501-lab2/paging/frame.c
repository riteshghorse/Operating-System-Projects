/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


fr_map_t frm_tab[NFRAMES];	
/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
// kprintf("To be implemented!\n");
	STATWORD ps;
	int i;
	disable(ps);
	for (i = 0; i < NFRAMES; ++i) {
		frm_tab[i].fr_status = FRM_UNMAPPED;
		frm_tab[i].fr_pid = BADPID;
		frm_tab[i].fr_vpno = 4096;
		frm_tab[i].fr_refcnt = 0;
		frm_tab[i].fr_type = FR_TBL;
		frm_tab[i].fr_dirty = FR_CLEAN;
	} 
	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
//  kprintf("To be implemented!\n");
 	STATWORD ps;
	int i;
	disable(ps);
	for (i = 0; i < NFRAMES; ++i) {
		if (frm_tab[i].fr_status == FRM_UNMAPPED) {
			*avail = i;
			restore(ps);
			return(OK);
		}
	}

	restore(ps);
	return(SYSERR);
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
	
	return OK;
}

void init_frame_tab (int frame_id)
{
	frm_tab[frame_id].fr_status = FRM_UNMAPPED;
	frm_tab[frame_id].fr_pid = BADPID;
	frm_tab[frame_id].fr_vpno = -1;
	frm_tab[frame_id].fr_refcnt = 0;
	frm_tab[frame_id].fr_type = FR_TBL;
	frm_tab[frame_id].fr_dirty = FR_CLEAN;
}


void frm_map (int pframe, int pid, int type)
{
	frm_tab[pframe].fr_status = FRM_MAPPED;
	frm_tab[pframe].fr_pid = pid;
	frm_tab[pframe].fr_type = type;	
	frm_tab[pframe].fr_refcnt += 1;
}