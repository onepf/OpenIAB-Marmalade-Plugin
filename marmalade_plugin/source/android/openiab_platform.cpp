/*
 * android-specific implementation of the openiab extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "openiab_internal.h"

#include "s3eEdk.h"
#include "s3eEdk_android.h"
#include <jni.h>
#include "IwDebug.h"

static jobject g_Obj;
static jmethodID g_mapSku;
static jmethodID g_getSkuDetails;
static jmethodID g_getPurchases;
static jmethodID g_init;
static jmethodID g_purchaseProduct;
static jmethodID g_purchaseSubscription;
static jmethodID g_consume;
static OpenIabStoreNames* g_openiabStoreNames = new OpenIabStoreNames();

void JNICALL native_OPENIAB_INIT_CALLBACK(JNIEnv* env, jobject obj, jint status, jstring error);
void JNICALL native_OPENIAB_PURCHASE_CALLBACK(JNIEnv* env, jobject obj, jint status, jstring error, jobject purchaseData);
void JNICALL native_OPENIAB_CONSUME_CALLBACK(JNIEnv* env, jobject obj, jint status, jstring error);

void ConvertSkuDetails(JNIEnv* env, jobject data, OpenIabSkuDetails* sd);
void ConvertPurchase(JNIEnv* env, jobject purchaseData, OpenIabPurchase* p);

static char* JStringToChar(JNIEnv* env, jstring str)
{
    if (str)
    {
        jsize len = env->GetStringUTFLength(str);

        const char* utf=env->GetStringUTFChars(str, NULL);
        char* retval = new char[len + 1];
        memcpy(retval, utf, len + 1);
        env->ReleaseStringUTFChars(str, utf);
        
        return retval;
    }
    return NULL;
}

static char* GetStaticJavaString(JNIEnv* env, jclass cls, const char* name)
{
    jfieldID fid = env->GetStaticFieldID(cls, name, "Ljava/lang/String;");
    if (fid == 0)
        return NULL;
    jstring jstr = (jstring) env->GetStaticObjectField(cls, fid);
    return JStringToChar(env, jstr);
}

struct s3eAndroidJNIFrame
{
    JNIEnv* env;
    
    s3eAndroidJNIFrame(JNIEnv* env,jint capacity) : env(env)
    {
        env->PushLocalFrame(capacity);
    }
    ~s3eAndroidJNIFrame()
    {
        env->PopLocalFrame(NULL);
    }
    JNIEnv* operator->()
    {
        return env;
    }
    JNIEnv* operator()()
    {
        return env;
    }
};

static jobjectArray MakeStringArray(const char** strings, int n)
{
    //the parent holds an s3eAndroidJNIFrame
    JNIEnv* env = s3eEdkJNIGetEnv();
    jobjectArray j_strings = env->NewObjectArray(n, env->FindClass("java/lang/String"), NULL);
    for (int i=0; i<n; ++i)
    {
        env->SetObjectArrayElement(j_strings,i,env->NewStringUTF(strings[i]));
    }
    return j_strings;
}

static char* JStringFieldToChar(jobject obj, jfieldID field)
{
    JNIEnv* env=s3eEdkJNIGetEnv();
    jstring str=(jstring)env->GetObjectField(obj,field);
    if(!str)
    {
        return NULL;
    }
    jsize len = env->GetStringUTFLength(str);
    const char* utf = env->GetStringUTFChars(str, NULL);
    char* retval = new char[len+1];
    memcpy(retval, utf, len+1);
    env->ReleaseStringUTFChars(str, utf);
    return retval;
}

void InitStoreNames()
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    // Get shop name constants
    jclass cls = s3eEdkAndroidFindClass("org/onepf/oms/OpenIabHelper");
    if (!cls)
    {
        jthrowable exc = env->ExceptionOccurred();
        if (exc)
        {
            env->ExceptionDescribe();
            env->ExceptionClear();
            IwTrace(openiab, ("InitStoreNames: OpenIabHelper class not found"));
        }
    }

    bool fail;
    
    if ((g_openiabStoreNames->GOOGLE = GetStaticJavaString(env, cls, "NAME_GOOGLE")) == NULL)
        fail = true;
        
    if ((g_openiabStoreNames->AMAZON = GetStaticJavaString(env, cls, "NAME_AMAZON")) == NULL)
        fail = true;

    if ((g_openiabStoreNames->SAMSUNG = GetStaticJavaString(env, cls, "NAME_SAMSUNG")) == NULL)
        fail = true;
        
    if ((g_openiabStoreNames->FORTUMO = GetStaticJavaString(env, cls, "NAME_FORTUMO")) == NULL)
        fail = true;	
    
    if ((g_openiabStoreNames->YANDEX = GetStaticJavaString(env, cls, "NAME_YANDEX")) == NULL)
        fail = true;	
    
    if ((g_openiabStoreNames->NOKIA = GetStaticJavaString(env, cls, "NAME_NOKIA")) == NULL)
        fail = true;	

    if ((g_openiabStoreNames->APPLAND = GetStaticJavaString(env, cls, "NAME_APPLAND")) == NULL)
        fail = true;

    if ((g_openiabStoreNames->SLIDEME = GetStaticJavaString(env, cls, "NAME_SLIDEME")) == NULL)
        fail = true;

    if ((g_openiabStoreNames->APTOIDE = GetStaticJavaString(env, cls, "NAME_APTOIDE")) == NULL)
        fail = true;
        
    if (fail)
    {
        jthrowable exc = env->ExceptionOccurred();
        if (exc)
        {
            env->ExceptionDescribe();
            env->ExceptionClear();
            IwTrace(openiab, ("InitStoreNames: One or more java string constants could not be found"));
        }
    }
}

OpenIabStoreNames* openiabStoreNames_platform()
{
    return g_openiabStoreNames;
}

s3eResult openiabInit_platform()
{
    // Get the environment from the pointer
    JNIEnv* env = s3eEdkJNIGetEnv();
    jobject obj = NULL;
    jmethodID cons = NULL;

    // Get the extension class
    jclass cls = s3eEdkAndroidFindClass("org/onepf/openiab/marmalade/OpenIAB");
    if (!cls)
        goto fail;
    
    // Get its constructor
    cons = env->GetMethodID(cls, "<init>", "()V");
    if (!cons)
        goto fail;

    // Construct the java class
    obj = env->NewObject(cls, cons);
    if (!obj)
        goto fail;

    // Get all the extension methods
    g_mapSku = env->GetMethodID(cls, "mapSku", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    if (!g_mapSku)
        goto fail;

    g_getSkuDetails = env->GetMethodID(cls, "getSkuDetails", "(Ljava/lang/String;)Lorg/onepf/oms/appstore/googleUtils/SkuDetails;");
    if (!g_getSkuDetails)
        goto fail;
        
    g_getPurchases = env->GetMethodID(cls, "getPurchases", "()Ljava/util/List;");
    if (!g_getPurchases)
        goto fail;
	
    g_init = env->GetMethodID(cls, "init", "(Lorg/onepf/oms/OpenIabHelper$Options;[Ljava/lang/String;)V");
    if (!g_init)
        goto fail;

    g_purchaseProduct = env->GetMethodID(cls, "purchaseProduct", "(Ljava/lang/String;Ljava/lang/String;)V");
    if (!g_purchaseProduct)
        goto fail;

    g_purchaseSubscription = env->GetMethodID(cls, "purchaseSubscription", "(Ljava/lang/String;Ljava/lang/String;)V");
    if (!g_purchaseSubscription)
        goto fail;

    g_consume = env->GetMethodID(cls, "consume", "(Ljava/lang/String;)V");
    if (!g_consume)
        goto fail;

    
/////////////////////////////////////////////////////
///// Non-autogenerated - register the native hooks
    {
        static const JNINativeMethod methods[]=
        {
            {"native_INIT_CALLBACK",        "(ILjava/lang/String;)V",                                               (void*) native_OPENIAB_INIT_CALLBACK},
            {"native_PURCHASE_CALLBACK",    "(ILjava/lang/String;Lorg/onepf/oms/appstore/googleUtils/Purchase;)V",  (void*) native_OPENIAB_PURCHASE_CALLBACK},
            {"native_CONSUME_CALLBACK",     "(ILjava/lang/String;)V",                                               (void*) native_OPENIAB_CONSUME_CALLBACK},
        };
        jint ret = env->RegisterNatives(cls, methods, sizeof(methods)/sizeof(methods[0]));
        if (ret)
        {
            IwTrace(openiab, ("RegisterNatives failed error:%d",ret));
            goto fail;
        }
    }
/////////////////////////////////////////////////////

    IwTrace(openiab, ("OPENIAB platform init success"));
    g_Obj = env->NewGlobalRef(obj);
    env->DeleteLocalRef(obj);
    env->DeleteGlobalRef(cls);

    // platform-specific initialisation
    InitStoreNames();
    return S3E_RESULT_SUCCESS;

fail:
    jthrowable exc = env->ExceptionOccurred();
    if (exc)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        IwTrace(openiab, ("One or more java methods could not be found"));
    }
    return S3E_RESULT_ERROR;
}

void openiabTerminate_platform()
{
    // Add any platform-specific termination code here
}

void mapSku_platform(const char* sku, const char* storeName, const char* storeSku)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring sku_jstr = env->NewStringUTF(sku);
    jstring storeName_jstr = env->NewStringUTF(storeName);
    jstring storeSku_jstr = env->NewStringUTF(storeSku);
    env->CallVoidMethod(g_Obj, g_mapSku, sku_jstr, storeName_jstr, storeSku_jstr);
}

OpenIabSkuDetails* getSkuDetails_platform(const char* sku)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring sku_jstr = env->NewStringUTF(sku);
    jobject data = env->CallObjectMethod(g_Obj, g_getSkuDetails, sku_jstr);   
    OpenIabSkuDetails* sd = new OpenIabSkuDetails;
    ConvertSkuDetails(env, data, sd);
    return sd;
}

OpenIabSkuDetails** getSkuListDetails_platform(const char** skuList, int skuListCount)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    OpenIabSkuDetails** sdList = new OpenIabSkuDetails*[skuListCount];
	
	for (int i = 0; i < skuListCount; ++i)
	{
		sdList[i] = getSkuDetails_platform(skuList[i]);
	}
    return sdList;
}

OpenIabPurchase** getPurchases_platform()
{
    JNIEnv* env = s3eEdkJNIGetEnv();

    jobject j_purchaseList = env->CallObjectMethod(g_Obj, g_getPurchases);      
    jclass list_cls = env->FindClass("java/util/List");
    jmethodID get_mid = env->GetMethodID(list_cls, "get", "(I)Ljava/lang/Object;");
    jmethodID size_mid = env->GetMethodID(list_cls, "size", "()I");
    int length = (int) env->CallIntMethod(j_purchaseList, size_mid);

    OpenIabPurchase** purchaseList = new OpenIabPurchase*[length];
    
    for (int i = 0; i < length; ++i)
    {
        OpenIabPurchase* p = new OpenIabPurchase;       
        jobject j_purchase = env->CallObjectMethod(j_purchaseList, get_mid, i);
        ConvertPurchase(env, j_purchase, p);
        purchaseList[i] = p;
    }
    return purchaseList;
}

int getPurchasesCount_platform()
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    
    jobject j_purchaseList = env->CallObjectMethod(g_Obj, g_getPurchases);
    jclass list_cls = env->FindClass("java/util/List");
    jmethodID size_mid = env->GetMethodID(list_cls, "size", "()I");
    return (int) env->CallIntMethod(j_purchaseList, size_mid);
}

void init_platform(OpenIabOptions* options, const char** skuList, int skuListCount)
{
    s3eAndroidJNIFrame env(s3eEdkJNIGetEnv(), skuListCount);
	
	jclass builderClass = env->FindClass("org/onepf/oms/OpenIabHelper$Options$Builder");
	
	jmethodID builderConstructor = env->GetMethodID(builderClass, "<init>", "()V");
	jobject builder = env->NewObject(builderClass, builderConstructor);
	
	// Options.discoveryTimeoutMs
    if (options->discoveryTimeoutMs > -1)
    {
		jmethodID mid = env->GetMethodID(builderClass, "setDiscoveryTimeout", "(I)Lorg/onepf/oms/OpenIabHelper$Options$Builder;");
		env->CallObjectMethod(builder, mid, options->discoveryTimeoutMs);
	}
	
	// Options.checkInventory
	jmethodID mid_checkInventory = env->GetMethodID(builderClass, "setCheckInventory", "(Z)Lorg/onepf/oms/OpenIabHelper$Options$Builder;");
	env->CallObjectMethod(builder, mid_checkInventory, options->checkInventory);
	
	// Options.checkInventoryTimeoutMs
    if (options->checkInventoryTimeoutMs > -1)
    {
		jmethodID mid = env->GetMethodID(builderClass, "setCheckInventoryTimeout", "(I)Lorg/onepf/oms/OpenIabHelper$Options$Builder;");
		env->CallObjectMethod(builder, mid, options->checkInventoryTimeoutMs);
    }
	
	// Options.verifyMode
    jmethodID mid_verifyMode = env->GetMethodID(builderClass, "setVerifyMode", "(I)Lorg/onepf/oms/OpenIabHelper$Options$Builder;");
	env->CallObjectMethod(builder, mid_verifyMode, (jint) options->verifyMode);
	
	// Options.storeSearchStrategy
	jmethodID mid_storeSearchStrategy = env->GetMethodID(builderClass, "setStoreSearchStrategy", "(I)Lorg/onepf/oms/OpenIabHelper$Options$Builder;");
	env->CallObjectMethod(builder, mid_storeSearchStrategy, (jint) options->storeSearchStrategy);
		
	// Options.storeKeys
    if (options->numStores > 0)
    {
		jmethodID mid = env->GetMethodID(builderClass, "addStoreKey", "(Ljava/lang/String;Ljava/lang/String;)Lorg/onepf/oms/OpenIabHelper$Options$Builder;");
		for (int i = 0; i < options->numStores; ++i)
		{
			jstring store = env->NewStringUTF(options->storeNames[i]);
            jstring key = env->NewStringUTF(options->storeKeys[i]);
			
			env->CallObjectMethod(builder, mid, store, key);
			
			env->DeleteLocalRef(store);
            env->DeleteLocalRef(key);
		}
    }
	
	// Options.preferredStoreNames
    if (options->numPreferredStoreNames > 0)
    {
		jmethodID mid = env->GetMethodID(builderClass, "addPreferredStoreName", "([Ljava/lang/String;)Lorg/onepf/oms/OpenIabHelper$Options$Builder;");
		jobjectArray preferredStoreNames = MakeStringArray(options->preferredStoreNames, options->numPreferredStoreNames);
		env->CallObjectMethod(builder, mid, preferredStoreNames);
		env->DeleteLocalRef(preferredStoreNames);
    }

	// Options.availableStoreNames
    if (options->numAvailableStoreNames > 0)
    {
		jmethodID mid = env->GetMethodID(builderClass, "addAvailableStoreNames", "([Ljava/lang/String;)Lorg/onepf/oms/OpenIabHelper$Options$Builder;");
		jobjectArray availableStoreNames = MakeStringArray(options->availableStoreNames, options->numAvailableStoreNames);
		env->CallObjectMethod(builder, mid, availableStoreNames);
		env->DeleteLocalRef(availableStoreNames);
    }
	
	// Options.samsungCertificationRequestCode
	if (options->samsungCertificationRequestCode > -1)
	{
		jmethodID mid = env->GetMethodID(builderClass, "setSamsungCertificationRequestCode", "(I)Lorg/onepf/oms/OpenIabHelper$Options$Builder;");
		env->CallObjectMethod(builder, mid, (jint) options->samsungCertificationRequestCode);
	}
	
	// Build Options instance
	jmethodID mid_build = env->GetMethodID(builderClass, "build", "()Lorg/onepf/oms/OpenIabHelper$Options;");
	jobject j_options = env->CallObjectMethod(builder, mid_build);
		
	jobjectArray j_skuList = MakeStringArray(skuList, skuListCount);
		
    // Initialize at last
    env->CallVoidMethod(g_Obj, g_init, j_options, j_skuList);
    
    env->DeleteLocalRef(j_options);
	env->DeleteLocalRef(j_skuList);
}

void purchaseProduct_platform(const char* sku, const char* payload)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring sku_jstr = env->NewStringUTF(sku);
    jstring payload_jstr = env->NewStringUTF(payload);
    env->CallVoidMethod(g_Obj, g_purchaseProduct, sku_jstr, payload_jstr);
}

void purchaseSubscription_platform(const char* sku, const char* payload)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring sku_jstr = env->NewStringUTF(sku);
    jstring payload_jstr = env->NewStringUTF(payload);
    env->CallVoidMethod(g_Obj, g_purchaseSubscription, sku_jstr, payload_jstr);
}

void consume_platform(const char* sku)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring sku_jstr = env->NewStringUTF(sku);
    env->CallVoidMethod(g_Obj, g_consume, sku_jstr);
}

void ConvertSkuDetails(JNIEnv* env, jobject data, OpenIabSkuDetails* sd)
{
    jclass details = env->FindClass("org/onepf/oms/appstore/googleUtils/SkuDetails");
    
    jfieldID j_sku              = env->GetFieldID(details, "mSku",           "Ljava/lang/String;");
    jfieldID j_type             = env->GetFieldID(details, "mType",          "Ljava/lang/String;");
    jfieldID j_price            = env->GetFieldID(details, "mPrice",         "Ljava/lang/String;");
    jfieldID j_title            = env->GetFieldID(details, "mTitle",         "Ljava/lang/String;");
    jfieldID j_description      = env->GetFieldID(details, "mDescription",   "Ljava/lang/String;");
    
    sd->productId               = JStringFieldToChar(data, j_sku);
    sd->type                    = JStringFieldToChar(data, j_type);
    sd->price                   = JStringFieldToChar(data, j_price);
    sd->title                   = JStringFieldToChar(data, j_title);
    sd->description             = JStringFieldToChar(data, j_description);
}

void ConvertPurchase(JNIEnv* env, jobject purchaseData, OpenIabPurchase* p)
{
    jclass purchase = env->FindClass("org/onepf/oms/appstore/googleUtils/Purchase");
    
    jfieldID j_orderID          = env->GetFieldID(purchase, "mOrderId",         "Ljava/lang/String;");
    jfieldID j_packageName      = env->GetFieldID(purchase, "mPackageName",     "Ljava/lang/String;");
    jfieldID j_sku              = env->GetFieldID(purchase, "mSku",             "Ljava/lang/String;");
    jfieldID j_purchaseTime     = env->GetFieldID(purchase, "mPurchaseTime",    "J"                 );
    jfieldID j_purchaseState    = env->GetFieldID(purchase, "mPurchaseState",   "I"                 );
    jfieldID j_token            = env->GetFieldID(purchase, "mToken",           "Ljava/lang/String;");
    jfieldID j_developerPayload = env->GetFieldID(purchase, "mDeveloperPayload","Ljava/lang/String;");
    jfieldID j_originalJson     = env->GetFieldID(purchase, "mOriginalJson",    "Ljava/lang/String;");
    jfieldID j_signature        = env->GetFieldID(purchase, "mSignature",       "Ljava/lang/String;");
    jfieldID j_appstoreName     = env->GetFieldID(purchase, "appstoreName",     "Ljava/lang/String;");
    
    p->orderId                  = JStringFieldToChar(purchaseData, j_orderID);
    p->packageName              = JStringFieldToChar(purchaseData, j_packageName);
    p->productId                = JStringFieldToChar(purchaseData, j_sku);
    p->purchaseTime             = env->GetLongField(purchaseData,  j_purchaseTime);
    p->purchaseState            = env->GetIntField(purchaseData,   j_purchaseState);
    p->purchaseToken            = JStringFieldToChar(purchaseData, j_token);
    p->developerPayload         = JStringFieldToChar(purchaseData, j_developerPayload);
    p->originalJson             = JStringFieldToChar(purchaseData, j_originalJson);
    p->signature                = JStringFieldToChar(purchaseData, j_signature);
    p->appstoreName             = JStringFieldToChar(purchaseData, j_appstoreName);
}

void DeletePurchase(OpenIabPurchase* p)
{
    delete []p->orderId;
    delete []p->packageName;
    delete []p->productId;
    delete []p->developerPayload;
    delete []p->purchaseToken;
    delete []p->originalJson;
    delete []p->signature;
    delete []p->appstoreName;
}

/////////////////// GC

static void GC_DeallocateInit(uint32 extID, int32 notification, void* systemData, void* instance, int32 returnCode, void* completeData)
{
    OpenIabInitResponse* cr = (OpenIabInitResponse*) systemData;
    if (cr->error)
        delete []cr->error;
    delete cr;
}

static void GC_DeallocatePurchase(uint32 extID, int32 notification, void* systemData, void* instance, int32 returnCode, void* completeData)
{
    OpenIabPurchaseResponse* pr = (OpenIabPurchaseResponse*) systemData;
    if (pr->purchase)
        DeletePurchase(pr->purchase);
    if (pr->error)
        delete []pr->error;
    delete pr;
}

static void GC_DeallocateConsume(uint32 extID, int32 notification, void* systemData, void* instance, int32 returnCode, void* completeData)
{
    OpenIabConsumeResponse* cr = (OpenIabConsumeResponse*) systemData;
    if (cr->error)
        delete []cr->error;
    delete cr;
}

////////////////// Callbacks

void JNICALL native_OPENIAB_INIT_CALLBACK(JNIEnv* env, jobject obj, jint status, jstring error)
{
    OpenIabInitResponse* ir = new OpenIabInitResponse;
    ir->error = JStringToChar(env, error);
    ir->status = (OpenIabResponseResult) status;
    
    s3eEdkCallbacksEnqueue(S3E_EXT_OPENIAB_HASH, OPENIAB_INIT_CALLBACK, ir, 0, NULL, false, GC_DeallocateInit, ir);
}

void JNICALL native_OPENIAB_PURCHASE_CALLBACK(JNIEnv* env, jobject obj, jint status, jstring error, jobject purchaseData)
{
    OpenIabPurchaseResponse* pr = new OpenIabPurchaseResponse;
    pr->error = JStringToChar(env, error);
    pr->status = (OpenIabResponseResult) status;
    
    if (purchaseData)
    {
        pr->purchase = new OpenIabPurchase;
        ConvertPurchase(env, purchaseData, pr->purchase);
    }
    else
        pr->purchase = NULL;
    
    s3eEdkCallbacksEnqueue(S3E_EXT_OPENIAB_HASH, OPENIAB_PURCHASE_CALLBACK, pr, 0, NULL, false, GC_DeallocatePurchase, pr);
}

void JNICALL native_OPENIAB_CONSUME_CALLBACK(JNIEnv* env, jobject obj, jint status, jstring error)
{
    OpenIabConsumeResponse* cr = new OpenIabConsumeResponse;
    cr->error = JStringToChar(env, error);
    cr->status = (OpenIabResponseResult) status;
    
    s3eEdkCallbacksEnqueue(S3E_EXT_OPENIAB_HASH, OPENIAB_CONSUME_CALLBACK, cr, 0, NULL, false, GC_DeallocateConsume, cr);
}