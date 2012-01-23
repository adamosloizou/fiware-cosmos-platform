//
// author: dmicol
//

#include "es_tid_ps_dynamicprofile_dictionary_CSDictionaryJNIInterface.h"

#include <iostream>

#include <jni.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Interface.h"

JNIEXPORT jboolean JNICALL Java_es_tid_ps_dynamicprofile_dictionary_CSDictionaryJNIInterface_InitFromTermsInDomainFlatFile
  (JNIEnv *env, jobject obj, jint iMode, jstring szTermsInDomainFlatFileName) {
  bool retVal = false;
  const char *nativeTermsInDomainFlatFileName =
    (*env).GetStringUTFChars(szTermsInDomainFlatFileName, 0);
  retVal = InitFromTermsInDomainFlatFile(iMode,
                                         nativeTermsInDomainFlatFileName);
  (*env).ReleaseStringUTFChars(szTermsInDomainFlatFileName,
                               nativeTermsInDomainFlatFileName);
  return retVal;
}

JNIEXPORT jboolean JNICALL Java_es_tid_ps_dynamicprofile_dictionary_CSDictionaryJNIInterface_LoadCSDictionary
  (JNIEnv *env, jobject obj, jint iMode,
  jstring szTermsInDomainFlatFileName, jstring szDictionaryName) {
  bool retVal = false;
  const char *nativeTermsInDomainFlatFileName =
    (*env).GetStringUTFChars(szTermsInDomainFlatFileName, 0);
  const char *nativeDictionaryName = 
    (*env).GetStringUTFChars(szDictionaryName, 0);
  LoadCSDictionary(iMode,
                   nativeTermsInDomainFlatFileName,
                   nativeDictionaryName);
  (*env).ReleaseStringUTFChars(szTermsInDomainFlatFileName,
                               nativeTermsInDomainFlatFileName);
  (*env).ReleaseStringUTFChars(szDictionaryName, nativeDictionaryName);
  return retVal;
}

JNIEXPORT jlong JNICALL Java_es_tid_ps_dynamicprofile_dictionary_CSDictionaryJNIInterface_ApplyDictionaryUsingUrl
  (JNIEnv *env, jobject jobj, jstring szURL) {
  long retVal;
  const char *nativeURL = (*env).GetStringUTFChars(szURL, 0);
  long lPatternID;
  if (ApplyDictionaryUsingUrl(nativeURL, &lPatternID)) {
    // If successful, return the pattern ID.
    retVal = lPatternID;
  } else {
    // Return 0 for failure.
    retVal = 0;
  }
  (*env).ReleaseStringUTFChars(szURL, nativeURL);
  return retVal;
}