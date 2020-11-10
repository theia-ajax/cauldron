#pragma once

#include "event_messages.h"

typedef enum event_message_type event_message_type;
typedef struct event_message event_message;
typedef void (*event_receiver_proc)(event_message*);

tx_result event_system_init(game_settings* settings);
void event_system_term(void);
void event_system_process_queue(float dt);

void event_system_subscribe(event_message_type msg_type, event_receiver_proc receiver);
void event_system_unsubscribe(event_message_type msg_type, event_receiver_proc receiver);
void event_send(event_message* message);