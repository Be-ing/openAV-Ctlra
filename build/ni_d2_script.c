//#include <stdint.h>

#include "event.h"

/* Tell the host application what USB device this script is for */
void script_get_vid_pid(int *out_vid, int *out_pid)
{
	*out_vid = 0x17cc;
	*out_pid = 0x1400;
}

void script_event_func(struct ctlra_dev_t* dev,
                       unsigned int num_events,
                       struct ctlra_event_t** events,
                       void *userdata)
{
	//printf("script func, num events %d\n", num_events);
	for(uint32_t i = 0; i < num_events; i++) {
		struct ctlra_event_t *e = events[i];
		if(e->type == 0) { // button 
			printf("scrpt: button id %d\n", e->button.id);
			if(e->button.id == 53) printf("script DECK\n");
			if(e->button.id == 57) printf("script PLAY\n");
			if(e->button.id == 56) printf("script CUE\n");
		}
	}
#if 0
		switch(e->type) {
		case CTLRA_EVENT_BUTTON:
			break;
		}
	}
#endif
}
