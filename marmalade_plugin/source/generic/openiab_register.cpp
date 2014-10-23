/*
 * WARNING: this is an autogenerated file and will be overwritten by
 * the extension interface script.
 */
/*
 * This file contains the automatically generated loader-side
 * functions that form part of the extension.
 *
 * This file is awlays compiled into all loaders but compiles
 * to nothing if this extension is not enabled in the loader
 * at build time.
 */
#include "IwDebug.h"
#include "openiab_autodefs.h"
#include "s3eEdk.h"
#include "openiab.h"
//Declarations of Init and Term functions
extern s3eResult openiabInit();
extern void openiabTerminate();


// On platforms that use a seperate UI/OS thread we can autowrap functions
// here.   Note that we can't use the S3E_USE_OS_THREAD define since this
// code is oftern build standalone, outside the main loader build.
#if defined I3D_OS_IPHONE || defined I3D_OS_OSX || defined I3D_OS_LINUX || defined I3D_OS_WINDOWS

static void mapSku_wrap(const char* sku, const char* storeName, const char* storeSku)
{
    IwTrace(OPENIAB_VERBOSE, ("calling openiab func on main thread: mapSku"));
    s3eEdkThreadRunOnOS((s3eEdkThreadFunc)mapSku, 3, sku, storeName, storeSku);
}

static void init_wrap(OpenIabOptions* options, const char** skuList, int skuListCount)
{
    IwTrace(OPENIAB_VERBOSE, ("calling openiab func on main thread: init"));
    s3eEdkThreadRunOnOS((s3eEdkThreadFunc)init, 3, options, skuList, skuListCount);
}

static void purchaseProduct_wrap(const char* sku, const char* payload)
{
    IwTrace(OPENIAB_VERBOSE, ("calling openiab func on main thread: purchaseProduct"));
    s3eEdkThreadRunOnOS((s3eEdkThreadFunc)purchaseProduct, 2, sku, payload);
}

static void purchaseSubscription_wrap(const char* sku, const char* payload)
{
    IwTrace(OPENIAB_VERBOSE, ("calling openiab func on main thread: purchaseSubscription"));
    s3eEdkThreadRunOnOS((s3eEdkThreadFunc)purchaseSubscription, 2, sku, payload);
}

static void consume_wrap(const char* sku)
{
    IwTrace(OPENIAB_VERBOSE, ("calling openiab func on main thread: consume"));
    s3eEdkThreadRunOnOS((s3eEdkThreadFunc)consume, 1, sku);
}

#define mapSku mapSku_wrap
#define init init_wrap
#define purchaseProduct purchaseProduct_wrap
#define purchaseSubscription purchaseSubscription_wrap
#define consume consume_wrap

#endif

s3eResult openiabRegister(openiabCallback cbid, s3eCallback fn, void* pData)
{
    return s3eEdkCallbacksRegister(S3E_EXT_OPENIAB_HASH, OPENIAB_CALLBACK_MAX, cbid, fn, pData, 0);
};

s3eResult openiabUnRegister(openiabCallback cbid, s3eCallback fn)
{
    return s3eEdkCallbacksUnRegister(S3E_EXT_OPENIAB_HASH, OPENIAB_CALLBACK_MAX, cbid, fn);
}

void openiabRegisterExt()
{
    /* fill in the function pointer struct for this extension */
    void* funcPtrs[9];
    funcPtrs[0] = (void*)openiabRegister;
    funcPtrs[1] = (void*)openiabUnRegister;
    funcPtrs[2] = (void*)mapSku;
    funcPtrs[3] = (void*)getSkuDetails;
    funcPtrs[4] = (void*)init;
    funcPtrs[5] = (void*)purchaseProduct;
    funcPtrs[6] = (void*)purchaseSubscription;
    funcPtrs[7] = (void*)consume;
    funcPtrs[8] = (void*)openiabStoreNames;

    /*
     * Flags that specify the extension's use of locking and stackswitching
     */
    int flags[9] = { 0 };

    /*
     * Register the extension
     */
s3eEdkRegister("openiab", funcPtrs, sizeof(funcPtrs), flags, openiabInit, openiabTerminate, 0);
}

#if !defined S3E_BUILD_S3ELOADER

#if defined S3E_EDK_USE_STATIC_INIT_ARRAY
int openiabStaticInit()
{
    void** p = g_StaticInitArray;
    void* end = p + g_StaticArrayLen;
    while (*p) p++;
    if (p < end)
        *p = (void*)&openiabRegisterExt;
    return 0;
}

int g_openiabVal = openiabStaticInit();

#elif defined S3E_EDK_USE_DLLS
S3E_EXTERN_C S3E_DLL_EXPORT void RegisterExt()
{
    openiabRegisterExt();
}
#endif

#endif
