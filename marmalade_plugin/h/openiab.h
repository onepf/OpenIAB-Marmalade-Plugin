/*
 * (C) 2001-2012 Marmalade. All Rights Reserved.
 *
 * This document is protected by copyright, and contains information
 * proprietary to Marmalade.
 *
 * This file consists of source code released by Marmalade under
 * the terms of the accompanying End User License Agreement (EULA).
 * Please do not use this program/source code before you have read the
 * EULA and have agreed to be bound by its terms.
 */
/*
 * WARNING: this is an autogenerated file and will be overwritten by
 * the extension interface script.
 */
#ifndef S3E_EXT_OPENIAB_H
#define S3E_EXT_OPENIAB_H

#include "s3eTypes.h"

enum openiabCallback
{
    OPENIAB_INIT_CALLBACK,

    OPENIAB_PURCHASE_CALLBACK,

    OPENIAB_CONSUME_CALLBACK,

    /**
     * The number of callbacks. Required by S3E.
     */
    OPENIAB_CALLBACK_MAX
};

enum OpenIabResponseResult
{
    /**
     * Success
     */
    OPENIAB_RESPONSE_RESULT_OK = 0,
    
    /**
     * User pressed back or canceled a dialog
     */
    OPENIAB_RESPONSE_RESULT_USER_CANCELED = 1,
    
    /**
     * Billing API version is not supported for the type requested
     */
    OPENIAB_RESPONSE_RESULT_BILLING_UNAVAILABLE = 3,
    
    /**
     * Requested product is not available for purchase
     */
    OPENIAB_RESPONSE_RESULT_ITEM_UNAVAILABLE = 4,
    
    /**
     * Invalid arguments provided to the API. This error can also indicate that the application was not correctly signed or properly set up for In-app Billing in Google Play, or does not have the necessary permissions in its manifest
     */
    OPENIAB_RESPONSE_RESULT_DEVELOPER_ERROR = 5,
    
    /**
     * Fatal error during the API action
     */
    OPENIAB_RESPONSE_RESULT_ERROR = 6,
    
    /**
     * Failure to purchase since item is already owned
     */
    OPENIAB_RESPONSE_RESULT_ITEM_ALREADY_OWNED = 7,
    
    /**
     * Failure to consume since item is not owned
     */
    OPENIAB_RESPONSE_RESULT_ITEM_NOT_OWNED = 8
};

struct OpenIabStoreNames
{
    char* GOOGLE;
    char* AMAZON;
    char* SAMSUNG;
    char* FORTUMO;
    char* YANDEX;
    char* NOKIA;
    char* APPLAND;
    char* SLIDEME;
    char* APTOIDE;
};

/**
 * OpenIAB public keys verification mode
 */
enum OpenIabVerifyMode
{
    /**
     * Verify signatures in any store. 
     * This is set by defaul. Throws exception if key is not available or invalid.
     * To prevent crashes OpenIAB wouldn't connect to OpenStore if no publicKey provided
     */
    VERIFY_EVERYTHING = 0,

    /** Don't verify signatures. To perform verification on server side */
    VERIFY_SKIP = 1,

    /** Verify signatures only if publicKey is available. Otherwise skip verification. */
    VERIFY_ONLY_KNOWN = 2
};

enum OpenIabStoreSearchStrategy
{
	/**
	 * Look for package installer and try to use it as a billing provider.
	 */
	SEARCH_STRATEGY_INSTALLER = 0,

	/**
	 * Look among available billing providers and select one that fits best.
	 */
	SEARCH_STRATEGY_BEST_FIT = 1,

	/**
	 * If package installer is not available or is not suited for billing, look for the best fit store.
	 */
	SEARCH_STRATEGY_INSTALLER_THEN_BEST_FIT = 2
};

struct OpenIabOptions
{
	OpenIabOptions() : 
		discoveryTimeoutMs(-1),
		checkInventory(false),
		checkInventoryTimeoutMs(-1),
		verifyMode((OpenIabVerifyMode) 0),
		storeSearchStrategy((OpenIabStoreSearchStrategy) 0),
		numStores(0),
		numPreferredStoreNames(0),
		numAvailableStoreNames(0),
		samsungCertificationRequestCode(-1)
	{}

    /** Wait specified amount of ms to find all OpenStores on device. 5000 by default */
    int discoveryTimeoutMs;
    
    /** 
     * Check user inventory in every store to select proper store
     * Will try to connect to each billingService and extract user's purchases.
     * If purchases have been found in the only store that store will be used for further purchases. 
     * If purchases have been found in multiple stores only such stores will be used for further elections    
     */
    bool checkInventory;
    
    /** Wait specified amount of ms to check inventory in all stores. 10000 by default */
    int checkInventoryTimeoutMs;

    OpenIabVerifyMode verifyMode;
	
	OpenIabStoreSearchStrategy storeSearchStrategy;
    
    /** Store 'public key' keys */
    const char** storeNames;
    /** Store 'public key' values */
    const char** storeKeys;
    int numStores;
    
    /**
     * Used as priority list if store that installed app is not found and there are 
     * multiple stores installed on device that supports billing.
     */
    const char** preferredStoreNames;
    int numPreferredStoreNames;
	   
    const char** availableStoreNames;
    int numAvailableStoreNames;
	
	int samsungCertificationRequestCode;
};

struct OpenIabPurchase
{
    /**
     * A unique order identifier for the transaction. This corresponds to the Google Wallet Order ID.
     */
    char* orderId;
    
    /**
     * The application package from which the purchase originated.
     */
    char* packageName;
    
    /**
     * The item's product identifier. Every item has a product ID, which you must specify in the application's product list on the Google Play Developer Console.
     */
    char* productId;
    
    /**
     * The time the product was purchased, in milliseconds since the epoch (Jan 1, 1970).
     */
    unsigned int purchaseTime;
    
    /**
     * The purchase state of the order. Possible values are 0 (purchased), 1 (canceled), or 2 (refunded).
     */
    int purchaseState;
    
    /**
     * A developer-specified string that contains supplemental information about an order. You can specify a value for this field when you make a getBuyIntent request.
     */
    char* developerPayload;
    
    /**
     * A token that uniquely identifies a purchase for a given item and user pair. 
     */
    char* purchaseToken;
    
    /**
     * JSON sent by the current store
     */
    char* originalJson;
    
    /**
     * Signature of the JSON string
     */ 
    char* signature;
    
    /**
     * Current store name
     */
    char* appstoreName;
};

struct OpenIabSkuDetails
{
    /**
     * The product ID for the product. 
     */
    char* productId;
    
    /**
     * Value must be “inapp�? for an in-app product or "subs" for subscriptions.
     */
    char* type;

    /**
     * Formatted price of the item, including its currency sign. The price does not include tax.
     */
    char* price;

    /**
     * Title of the product.
     */
    char* title;

    /**
     * Description of the product.
     */
    char* description;
};

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
// \cond HIDDEN_DEFINES
S3E_BEGIN_C_DECL
// \endcond

/**
 * Returns S3E_TRUE if the openiab extension is available.
 */
s3eBool openiabAvailable();

/**
 * Registers a callback to be called for an operating system event.
 *
 * The available callback types are listed in @ref openiabCallback.
 * @param cbid ID of the event for which to register.
 * @param fn callback function.
 * @param userdata Value to pass to the @e userdata parameter of @e NotifyFunc.
 * @return
 *  - @ref S3E_RESULT_SUCCESS if no error occurred.
 *  - @ref S3E_RESULT_ERROR if the operation failed.\n
 *
 * @see openiabUnRegister
 * @note For more information on the system data passed as a parameter to the callback
 * registered using this function, see the @ref openiabCallback enum.
 */
s3eResult openiabRegister(openiabCallback cbid, s3eCallback fn, void* userData);

/**
 * Unregister a callback for a given event.
 * @param cbid ID of the callback to unregister.
 * @param fn Callback Function.
 * @return
 * - @ref S3E_RESULT_SUCCESS if no error occurred.
 * - @ref S3E_RESULT_ERROR if the operation failed.\n
 * @note For more information on the systemData passed as a parameter to the callback
 * registered using this function, see the openiabCallback enum.
 * @note It is not necessary to define a return value for any registered callback.
 * @see openiabRegister
 */
s3eResult openiabUnRegister(openiabCallback cbid, s3eCallback fn);

void mapSku(const char* sku, const char* storeName, const char* storeSku);

OpenIabSkuDetails* getSkuDetails(const char* sku);

void init(OpenIabOptions* options, const char** skuList, int skuListCount);

void purchaseProduct(const char* sku, const char* payload);

void purchaseSubscription(const char* sku, const char* payload);

void consume(const char* sku);

OpenIabStoreNames* openiabStoreNames();

OpenIabSkuDetails** getSkuListDetails(const char** skuList, int skuListCount);

OpenIabPurchase** getPurchases();

int getPurchasesCount();

S3E_END_C_DECL

#endif /* !S3E_EXT_OPENIAB_H */
