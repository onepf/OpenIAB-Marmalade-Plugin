OpenIAB-Marmalade-Plugin
========================

OpenIAB plugin for [Marmalade SDK](https://www.madewithmarmalade.com/products/marmalade-sdk)

Integration
-----------

1) Build plugin

* Java library
```
mkb extension/MyExtension/MyExtension_android_java.mkb
```
* Native library
```
mkb extension/MyExtension/MyExtension_android.mkb --debug
mkb extension/MyExtension/MyExtension_android.mkb --release 
```

2) Include ```openiab.mkf``` to the application ```.mkb``` file.

```
subprojects
{
  ../../marmalade_plugin/openiab.mkf
}
```

Please refer to [Marmalade EDK documentation](http://docs.madewithmarmalade.com/display/MD741/Using+and+updating+extensions) for details.

Build and run application
-----
1) Setup everything required in the dependency checker

![Dependency checker](http://take.ms/IZumK)

2) Build the project in the Marmalade Hub

![Hub build and run](http://take.ms/Qvzth)

API
---

Retrieve supported store names.
```C++
OpenIabStoreNames* openiabStoreNames();
```

Map you SKUs. This is optional.
```C++
void mapSku(const char* sku, const char* storeName, const char* storeSku);
```

Initialize plugin.
```C++
void init(OpenIabOptions* options, const char** skuList, int skuListCount);
```

Start purchase process.
```C++
void purchaseProduct(const char* sku, const char* payload);
void purchaseSubscription(const char* sku, const char* payload);
```

Consume SKU.
```C++
void consume(const char* sku);
```

Retrive product details from the current store for single SKU or for a list of SKUs.
```C++
OpenIabSkuDetails* getSkuDetails(const char* sku);
OpenIabSkuDetails** getSkuListDetails(const char** skuList, int skuListCount);
```

Options
-------

Create Options object.
```
OpenIabOptions* options = new OpenIabOptions();
```

Set store search strategy.
```
options->storeSearchStrategy = SEARCH_STRATEGY_INSTALLER_THEN_BEST_FIT;
```

Set available stores to restrict the set of stores to check.
```
options->numAvailableStoreNames = 1;
options->availableStoreNames = new const char*[options->numAvailableStoreNames];
options->availableStoreNames[0] = openiabStoreNames()->GOOGLE;
```

Set preferred store names (works only for store search strategy ```OpenIabHelper.Options.SEARCH_STRATEGY_BEST_FIT``` and ```OpenIabHelper.Options.SEARCH_STRATEGY_INSTALLER_THEN_BEST_FIT```).
```
options->numPreferredStoreNames = 1;
options->preferredStoreNames = new const char*[options->numPreferredStoreNames];
options->preferredStoreNames[0] = openiabStoreNames()->GOOGLE;
```

Set store keys.
```
const char *publicKey = "publicKey";

options->numStores = 1;
options->storeNames = new const char*[options->numStores];
options->storeKeys = new const char*[options->numStores];
options->storeNames[0] = openiabStoreNames()->GOOGLE;
options->storeKeys[0] = publicKey;
```

Set verifying mode (applicable only for Google Play, Appland, Aptoide, AppMall, SlideMe, Yandex.Store).
```
options->verifyMode = VERIFY_SKIP;
```

Callbacks
---------

Callbacks are buil-in functionality.
In order to subscribe you need to pass callback ID and your handler function.
```C++
s3eResult openiabRegister(openiabCallback cbid, s3eCallback fn, void* userData);
```

Here are OpenIAB callback IDs.
```C++
OPENIAB_INIT_CALLBACK
OPENIAB_PURCHASE_CALLBACK
OPENIAB_CONSUME_CALLBACK
```

User data should be casted to the corresponding structure in the callback handler.
```C++
struct OpenIabInitResponse
{
    OpenIabResponseResult status;
    char* error;
};

struct OpenIabPurchaseResponse
{
    OpenIabResponseResult status;
    char* error;
    OpenIabPurchase* purchase;
};

struct OpenIabConsumeResponse
{
    OpenIabResponseResult status;
    char* error;
};
```
