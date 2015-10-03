#include <jni.h>
#include <iostream>
#include <string.h>
#include <map>
#include <utility>
#include <android/log.h>
#include <array>
#include <Notification/ITNNotificationClient.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/PrivateKeyPassphraseHandler.h>
#include <Poco/UUIDGenerator.h>

#include <Common/TLLoggerInitializer.h>
#include <twilio-jni/twilio-jni.h>

#include "ITMClient.h"
#include "ITMChannels.h"
#include "ITMChannel.h"
#include "ITMessages.h"
#include "ITMessage.h"
#include "ITMMembers.h"
#include "ITMMember.h"
#include "IPMessagingClientImpl.h"
#include "TwilioIPMessagingClientContextDefines.h"


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))
#define TAG  "IPMessagingClient(native)"
#define PRODUCTION 1
#define WITH_SSL 1


using namespace rtd;

std::map<std::string, std::string> configMap;

#if PRODUCTION
#define TEST_IPMESSAGING_SERVICE "https://aim.twilio.com"
#define TEST_REGISTRATION_SERVICE "https://ers.twilio.com"
#define TEST_TWILSOCK_SERVICE "https://tsock.twilio.com"
#define TEST_DATA_SERVICE "https://cds.twilio.com"
#define TEST_SUBSCRIPTIONS_SERVICE "https://cds.twilio.com"
#else
#define TEST_IPMESSAGING_SERVICE "https://aim.dev-us1.twilio.com"
#define TEST_REGISTRATION_SERVICE "https://ers.dev-us1.twilio.com"
#define TEST_TWILSOCK_SERVICE "https://tsock.dev-us1.twilio.com"
#define TEST_DATA_SERVICE "https://cds.dev-us1.twilio.com"
#define TEST_SUBSCRIPTIONS_SERVICE "https://cds.dev-us1.twilio.com"
#endif


/*
 * Class:     com_twilio_ipmessaging_impl_TwilioIPMessagingClientImpl
 * Method:    create
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_twilio_ipmessaging_impl_TwilioIPMessagingClientImpl_create
  (JNIEnv *env, jobject obj) {

	LOG_D(TAG, "Entered TwilioIPMessagingClientImpl_create()");

	static const char *class_names[] = {
			"com/twilio/ipmessaging/impl/MessageImpl",
			"com/twilio/ipmessaging/impl/ChannelImpl",
			NULL
		};
	static bool classes_precached = false;

	if (!classes_precached) {
		for (int i = 0; class_names[i]; ++i) {
			tw_jni_find_class(env, class_names[i]);
			if (env->ExceptionOccurred()) {
				env->ExceptionClear();
			}
		}
		classes_precached = true;
	}
}



JNIEXPORT jlong JNICALL Java_com_twilio_ipmessaging_impl_TwilioIPMessagingClientImpl_initNative
  (JNIEnv *env, jobject obj, jstring token, jobject listener) {

	LOG_D( TAG, "Entered IPMessagingClientImpl_initNative()");

	rtd::ITDLogger::initialize(true);

	LOG_D(TAG, "Set ITDLogger to true.");

#if WITH_SSL
	Poco::Net::initializeSSL();
	auto context = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_NONE, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
	Poco::Net::SSLManager::instance().initializeClient(nullptr, nullptr, context);
#endif

	if (token == nullptr) {
		LOGE("token is null");
		return 0;
	}

	const char *tokenStr = env->GetStringUTFChars(token, 0);
	IPMessagingClientContext *clientParams_ = new IPMessagingClientContext();
	auto messagingListener = std::make_shared<TwilioIPMessagingClientListener>(env, obj, listener);

	LOG_D(TAG,"Creating messagingListener");
	jlong nativeObserverHandle = tw_jni_fetch_long(env, obj, "nativeObserverHandle");

	LOG_D(TAG,"Creating  config map");
	configMap.insert(std::make_pair("RTDIPMessagingServiceAddr", TEST_IPMESSAGING_SERVICE));
	configMap.insert(std::make_pair("RTDRegistrationServiceAddr", TEST_REGISTRATION_SERVICE));
	configMap.insert(std::make_pair("RTDTwilsockServiceAddr", TEST_TWILSOCK_SERVICE));
	configMap.insert(std::make_pair("RTDDataServiceAddr", TEST_DATA_SERVICE));
	configMap.insert(std::make_pair("RTDSubscriptionServiceAddr", TEST_SUBSCRIPTIONS_SERVICE));

	LOG_D(TAG,"Creating configurationProvider");
	auto configurationProvider = std::make_shared<TwilioIPMessagingConfigurationProvider>(configMap);
	if(configurationProvider == nullptr) {
		LOG_D( TAG, "configurationProvider is NULL");
	}

	LOG_D( TAG, "Creating notificationClientObserver");
	auto  notificationClientObserver = std::make_shared<TwilioIPMessagingNotificationClientListener>();
	if(notificationClientObserver == nullptr) {
		LOG_D( TAG, "notificationClientObserver is NULL");
	}


	LOG_D( TAG,"Creating ITNNotificationClientPtr");

	ITNNotificationClientPtr notificationClientPtr;
	notificationClientPtr = TNNotificationClientFactory::CreateNotificationClient(tokenStr,configurationProvider);
	notificationClientPtr->Init(notificationClientObserver);

	if(notificationClientPtr == nullptr) {
		LOG_D(TAG, "notificationClientPtr is NULL");
	}

	clientParams_->messagingListener = messagingListener;
	clientParams_->configurationProvider = configurationProvider;
	clientParams_->notificationClientObserver = notificationClientObserver;
	clientParams_->notificationClient = notificationClientPtr;

	LOG_D( TAG, "Setting nativeClientParam");
	tw_jni_set_long(env, obj, "nativeClientParam", (jlong)clientParams_);

	return reinterpret_cast<jlong>(clientParams_);

}

JNIEXPORT jlong JNICALL Java_com_twilio_ipmessaging_impl_TwilioIPMessagingClientImpl_createMessagingClient
  (JNIEnv *env, jobject obj, jstring token) {

	LOG_D( TAG,"Checking token validity.");

	if (token == nullptr) {
		LOG_E(TAG,"token is null");
		return 0;
	}

	const char *tokenStr = env->GetStringUTFChars(token, 0);
	jlong nativeClientContext = tw_jni_fetch_long(env, obj, "nativeClientParam");

	LOG_D(TAG,"Checking nativeClientParam.");

	if (nativeClientContext == 0) {
			LOG_W(TAG, "client context is null");
			return 0;
	} else {

		IPMessagingClientContext *clientParamsRecreate = reinterpret_cast<IPMessagingClientContext *>(nativeClientContext);
		LOG_D(TAG,"client context is recreated.");

		if(clientParamsRecreate->messagingListener == nullptr) {
			LOG_W( TAG, "messagingListener is NULL.");
			return 0;
		}

		if(clientParamsRecreate->configurationProvider == nullptr) {
			LOG_W(TAG, "configurationProvider is NULL.");
			return 0;
		}

		if( clientParamsRecreate->notificationClient == nullptr) {
			LOG_W(TAG, "notificationClient is NULL.");
			return 0;
		}

		LOG_D( TAG,"Creating the msgClient.");

		ITMClientPtr messagingClient = ITMClient::createClient(tokenStr,
				clientParamsRecreate->messagingListener,
				clientParamsRecreate->configurationProvider,
				clientParamsRecreate->notificationClient,
				([](TMResult result) { LOG_W( TAG,"Created the msgClient."); }));
														   //([](TMResult result) { LOGW( "Java_com_twilio_example_TestRTDJNI_getMessagingClient : Client init.");}));

		clientParamsRecreate->messagingClient = messagingClient;

	}

	return 0;
}



/*
 * Class:     com_twilio_ipmessaging_impl_TwilioIPMessagingClientImpl
 * Method:    getChannelsNative
 * Signature: ()Lcom/twilio/ipmessaging/Channels;
 */
JNIEXPORT jobject JNICALL Java_com_twilio_ipmessaging_impl_TwilioIPMessagingClientImpl_getChannelsNative
  (JNIEnv *env, jobject obj) {
	jobject channels = nullptr;

	jlong nativeClientContext = tw_jni_fetch_long(env, obj, "nativeClientParam");

	LOGW( "Java_com_twilio_ipmessaging_impl_TwilioIPMessagingClientImpl_getChannelsNative : Checking nativeClientContext.");

	if (nativeClientContext == 0) {
			LOGW("client context is null");
			return 0;
	} else {

		IPMessagingClientContext *clientParamsRecreate = reinterpret_cast<IPMessagingClientContext *>(nativeClientContext);

		if(clientParamsRecreate != nullptr) {
			if(clientParamsRecreate->messagingClient != nullptr) {
				//get channels object//
				ITMChannelsPtr channelsLocal = clientParamsRecreate->messagingClient->getChannels();
				LOGW("clientParamsRecreate->messagingClient is NOT null");
				while (channelsLocal == nullptr)
				{
					LOGW("Messaging lib not ready, retrying...");
					Poco::Thread::sleep(1000);
					channelsLocal = clientParamsRecreate->messagingClient->getChannels();
				}

				ChannelsContext* channelsContext_ = new ChannelsContext();
				channelsContext_->channels = channelsLocal;
				jlong channelsContextHandle = reinterpret_cast<jlong>(channelsContext_);
				clientParamsRecreate->channels = channelsLocal;

				jclass java_channels_impl_cls = tw_jni_find_class(env, "com/twilio/ipmessaging/impl/ChannelsImpl");
				if(java_channels_impl_cls != nullptr) {
					LOGW("Found java_channels_impl_cls class" );
				}

				jmethodID construct = tw_jni_get_method_by_class(env, java_channels_impl_cls, "<init>", "(J)V");
				channels = tw_jni_new_object(env, java_channels_impl_cls, construct, channelsContextHandle);

			}
		}
	}

	LOGW("*************** SHOULD NEVER PRINT THIS *********");
	return channels;

}