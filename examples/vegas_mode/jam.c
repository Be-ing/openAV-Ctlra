
#include "global.h"

#include <string.h>

#include "ctlra.h"
#include "devices/ni_maschine_jam.h"

void jam_update_state(struct ctlra_dev_t *dev, void *ud)
{
	struct dummy_data *d = ud;
	uint32_t i;

	for(i = 0; i < VEGAS_BTN_COUNT; i++)
		ctlra_dev_light_set(dev, i, UINT32_MAX * d->buttons[i]);

	ctlra_dev_light_flush(dev, 0);
	return;
}

void jam_func(struct ctlra_dev_t* dev,
		     uint32_t num_events,
		     struct ctlra_event_t** events,
		     void *userdata)
{
	struct dummy_data *d = (void *)userdata;
	(void)dev;
	(void)userdata;
	for(uint32_t i = 0; i < num_events; i++) {
		char *pressed = 0;
		struct ctlra_event_t *e = events[i];
		switch(e->type) {
		case CTLRA_EVENT_BUTTON:
			d->buttons[e->button.id] = e->button.pressed;
			if(e->button.id == 10) {
				memset(&d->buttons, e->button.pressed,
				       sizeof(d->buttons));
			}
			break;

		case CTLRA_EVENT_ENCODER:
		case CTLRA_EVENT_SLIDER:
		case CTLRA_EVENT_GRID:
		default:
			break;
		};
	}
	d->revision++;
}

