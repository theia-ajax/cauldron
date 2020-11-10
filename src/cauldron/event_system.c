#include "event_system.h"

#include "event_messages.h"
#include "stb_ds.h"

enum {
    MAX_EVENT_MESSAGE_SIZE_IN_BYTES = 64,
    MAX_EVENT_MESSAGES_IN_QUEUE = 128,
};

typedef struct event_subscription {
    event_message_type msg_type;
    event_receiver_proc* subscribers; // stbds_arr
} event_subscription;

typedef struct message_type_meta {
    event_message_type message_type;
    size_t message_size;
} message_type_meta;

typedef uint8_t opqaue_message_storage[MAX_EVENT_MESSAGE_SIZE_IN_BYTES];

// update with new message types as new types are added
message_type_meta message_meta_data[EventMessage_Count] = {
    {.message_type = EventMessage_None, .message_size = 0},
    {.message_type = EventMessage_ChangeLevel, .message_size = sizeof(change_level_event)},
    {.message_type = EventMessage_OnEntitySpawned, .message_size = sizeof(on_entity_spawned_event)},
};

// system state
event_subscription subscriptions[EventMessage_Count];
opqaue_message_storage message_queue[MAX_EVENT_MESSAGES_IN_QUEUE];
uint32_t queue_tail = 0;

tx_result event_system_init(game_settings* settings)
{
    // get some type safety by ensuring that valid sizing data has been provided for all messages
    // skip the none message by starting at 1
    for (int i = 1; i < (int)EventMessage_Count; ++i) {
        const message_type_meta* meta = &message_meta_data[i];
        TX_ASSERT(
            meta->message_type > EventMessage_None && meta->message_type < EventMessage_Count);
        TX_ASSERT(meta->message_size > 0 && meta->message_size <= MAX_EVENT_MESSAGE_SIZE_IN_BYTES);
    }

    for (int i = 0; i < EventMessage_Count; ++i) {
        subscriptions[i] = (event_subscription){
            .msg_type = (event_message_type)i,
            .subscribers = NULL,
        };
        arrsetcap(subscriptions[i].subscribers, 32);
    }

    return TX_SUCCESS;
}

void event_system_term(void)
{
    for (int i = 0; i < EventMessage_Count; ++i) {
        arrfree(subscriptions[i].subscribers);
    }
    memset(message_queue, 0, sizeof(message_queue));
    queue_tail = 0;
}

void event_system_process_queue(float dt)
{
    uint32_t head = 0;
    while (head < queue_tail) {
        event_message* event = (event_message*)&message_queue[head];
        event_receiver_proc* receivers = subscriptions[event->msg_type].subscribers;
        for (int i = 0; i < arrlen(receivers); ++i) {
            receivers[i](event);
        }
        ++head;
    }
    queue_tail = 0;
}

void event_system_subscribe(event_message_type msg_type, event_receiver_proc receiver)
{
    TX_ASSERT(VALID_INDEX(msg_type, EventMessage_Count) && msg_type > EventMessage_None);

    event_receiver_proc* receivers = subscriptions[msg_type].subscribers;

    // deduplication is achieved through a linear search which is not ideal but it is not expected
    // that systems will be subscribing/unsubscribing that often.
    for (int i = 0; i < arrlen(receivers); ++i) {
        if (receivers[i] == receiver) {
            // duplicate found
            return;
        }
    }
    arrput(receivers, receiver);
}

void event_system_unsubscribe(event_message_type msg_type, event_receiver_proc receiver)
{
    TX_ASSERT(VALID_INDEX(msg_type, EventMessage_Count) && msg_type > EventMessage_None);

    event_receiver_proc* receivers = subscriptions[msg_type].subscribers;

    int index = -1;

    // again a linear search for the existing receiver
    for (int i = 0; i < arrlen(receivers); ++i) {
        if (receivers[i] == receiver) {
            index = i;
            break;
        }
    }

    if (index >= 0) {
        arrdel(receivers, index);
    }
}

void event_send(event_message* message)
{
    TX_ASSERT(message);
    TX_ASSERT(queue_tail < MAX_EVENT_MESSAGES_IN_QUEUE);

    size_t msg_size = message_meta_data[message->msg_type].message_size;
    memcpy(&message_queue[queue_tail], message, msg_size);

    ++queue_tail;
}