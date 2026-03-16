#ifndef FRAMEWORK_PUBSUB_BRIDGE_H
#define FRAMEWORK_PUBSUB_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * C bridge for the C++ PubSub singleton.
 * Allows C drivers to publish messages by topic name.
 * topic: NUL-terminated C string topic name.
 */
void pubsub_c_publish(const char* topic, const void* data, unsigned short len);
// register a PubSub instance for C publishers (call from C++ init)
void pubsub_register_for_c(void* inst);
void pubsub_unregister_for_c(void);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_PUBSUB_BRIDGE_H
