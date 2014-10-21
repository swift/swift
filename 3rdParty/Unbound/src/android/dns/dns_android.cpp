/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "dns_android.h"
#include <vector>
#include <string>
#include <jni.h>

JavaVM* javaVM = 0;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*aReserved*/) {
  javaVM = vm;
  return JNI_VERSION_1_6;
}

std::vector<std::string> get_servers() {
  std::vector<std::string> servers;
  if (javaVM == 0) {
    return servers;
  }

  JNIEnv* env = 0;
  int result = javaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
  if (result == JNI_EDETACHED) {
    if (javaVM->AttachCurrentThread(&env, 0) != 0) {
      return servers;
    }
  } else if (result != JNI_OK) {
    return servers;
  }

  jclass systemProperties = env->FindClass("android.os.SystemProperties");
  if (!systemProperties) {
    return servers;
  }

  jmethodID method = env->GetStaticMethodID(systemProperties, "get", "(Ljava/lang/String;)Ljava/lang/String;");
  if (!method) {
    return servers;
  }

  const char* parameters[] = {
    "net.dns1",
    "net.dns2",
    "net.dns3",
    "net.dns4"
  };

  for (size_t i = 0; i < sizeof(parameters)/sizeof(*parameters); ++i) {
    jstring parameter = env->NewStringUTF(parameters[i]);
    jstring value = jstring(env->CallStaticObjectMethod(systemProperties, method, parameter));
    const char* server = env->GetStringUTFChars(value, JNI_FALSE);
    if (strcmp(server, "")) {
      servers.push_back(server);
    }
    env->ReleaseStringUTFChars(value, server);
  }

  return servers;
}

size_t dns_android_server_count() {
  std::vector<std::string> servers = get_servers();
  return servers.size();
}

dns_android_result dns_android_server_get(size_t index, char* buffer, size_t length) {
  std::vector<std::string> servers = get_servers();
  if (index >= servers.size()) {
    return DNS_ANDROID_FAILURE; /* invalid server index */
  }

  if (length < servers[index].length() + 1) {
    return DNS_ANDROID_FAILURE; /* insufficient buffer space */
  }

  strncpy(buffer, servers[index].c_str(), length);
  return DNS_ANDROID_SUCCESS;
}
