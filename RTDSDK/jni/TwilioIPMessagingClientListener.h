//
//  TwilioIPMessagingClientListener.hpp
//  Twilio IP Messaging Client
//
//  Copyright © 2015 Twilio. All rights reserved.
//

#ifndef TwilioIPMessagingClientListener_cpp
#define TwilioIPMessagingClientListener_cpp

#include <Poco/Foundation.h>

#include <Common/TLLogger.h>

#include <android/log.h>

#include "ITMClient.h"
#include "ITMChannel.h"
#include "ITMMember.h"
#include "ITMessages.h"
#include "ITMessage.h"

using namespace rtd;

#define TAG  "RTD_TESTS"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))


class TwilioIPMessagingClientListener : public ITMClientListener
{
public:
    TwilioIPMessagingClientListener(JNIEnv* env,jobject obj,jobject j_ipmessagingclient);
    virtual ~TwilioIPMessagingClientListener();

    void onMessage(TMAction action, ITMessagePtr message);
    void onChannel(TMAction action, ITMChannelPtr channel);
    void onMember(TMAction action, ITMMemberPtr member);
    void onError(const std::string& error);

    void onToastNotification(const std::string& channelId, const std::string& messageId);
    void onToastSubscribed();
    void onToastFailed(TNError code);

private:
    jobject j_ipmessagingclient_;
	jmethodID j_start_listening_id_;
	jmethodID j_stop_listening_id_;
	jmethodID j_failed_to_start_id_;
	jmethodID j_receive_conv_id_;
	JNIEnv* env_;
};

#endif /* TwilioIPMessagingClientListener_cpp */
