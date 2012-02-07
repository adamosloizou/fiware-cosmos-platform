
#ifndef _H_NotificationMessages
#define _H_NotificationMessages

// Definition of the Notification messages ( const char * )


// Notification to report status-update to all connected delilahs
#define notification_update_status                      "notification_update_status"

#define notification_samson_worker_check_finish_tasks   "notification_samson_worker_check_finish_tasks"

#define notification_samson_worker_send_packet          "notification_samson_worker_send_packet"


// Notification to review worker_task ( in Stream manager )
#define notification_review_stream_manager                "notification_review_stream_manager"
#define notification_review_stream_manager_fast           "notification_review_stream_manager_fast"
#define notification_review_stream_manager_save_state     "notification_review_stream_manager_save_state"


// Notification that a network element has been disconnected
#define notification_network_diconnected                    "notification_network_diconnected"

// Notification to review repeat commands in delilha
#define notification_delilah_review_repeat_tasks            "notification_delilah_review_repeat_tasks"
#define notification_delilah_stop_repeat_tasks            "notification_delilah_stop_repeat_tasks"

#endif
