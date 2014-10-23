package org.onepf.openiab.marmalade;
/*
java implementation of the openiab extension.

Add android-specific functionality here.

These functions are called via JNI from native code.
*/
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
import com.ideaworks3d.marmalade.LoaderAPI;
import com.ideaworks3d.marmalade.LoaderActivity;
import android.content.Intent;
import android.util.Log;
import org.onepf.oms.appstore.googleUtils.*;
import org.onepf.oms.*;
import android.app.Activity;

import java.util.Arrays;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class OpenIAB
{
	private static final String TAG = "OpenIAB-Marmalade";

	static final int RC_REQUEST = 10001; /**< (arbitrary) request code for the purchase flow */
	
	private static OpenIabHelper _helper = null;
	private static BillingCallback _billingCallback;
	private static Inventory _inventory = null;
	
	private static String _sku = null;
	private static String _payload = null;
	private static boolean _inapp = true;
		
	public static void startPurchase(Activity activity) 
	{
		if (_inapp)
			_helper.launchPurchaseFlow(activity, _sku, RC_REQUEST, _billingCallback, _payload);
		else
			_helper.launchSubscriptionPurchaseFlow(activity, _sku, RC_REQUEST, _billingCallback, _payload);
			
		_sku = null;
		_payload = null;
	}
	
	public static void onActivityResult(int requestCode, int resultCode, Intent data) 
	{
		Log.i(TAG, "onActivityResult: " + requestCode + "," + resultCode);
		_helper.handleActivityResult(requestCode, resultCode, data);
	}
	
    public void mapSku(String sku, String storeName, String storeSku)
    {
        SkuManager.getInstance().mapSku(sku, storeName, storeSku);
    }
	
    public SkuDetails getSkuDetails(String sku)
    {
        if (!_inventory.hasDetails(sku)) {
            return null;
        }
        return _inventory.getSkuDetails(sku);
    }
	
    public void init(final OpenIabHelper.Options options, final String[] skus)
    {
	    if (options == null) {
            Log.e(TAG, "No options sent.");
            return;
        }
		
		_billingCallback = new BillingCallback();
		
		_helper = new OpenIabHelper(LoaderAPI.getActivity(), options);
		
		// Start setup. This is asynchronous and the specified listener
		// will be called once setup completes.
        Log.i(TAG, "Starting setup.");
		_helper.startSetup(new IabHelper.OnIabSetupFinishedListener() {
            public void onIabSetupFinished(IabResult result) {
				Log.i(TAG, "Setup finished.");
				if (result.isFailure()) {
					// Oh noes, there was a problem.
					Log.e(TAG, "Problem setting up in-app billing: " + result);
					safe_native_INIT_CALLBACK(result);
					return;
				}

				List<String> skuList = Arrays.asList(skus);
				
				Log.i(TAG, "Querying inventory.");
				// TODO: find alternative to this workaround
				_helper.queryInventoryAsync(true, skuList, skuList, _billingCallback);
			}
        });
    }
	
	private void purchase(String sku, boolean inapp, String payload)
	{
		_sku = sku;
		_payload = payload;
		_inapp = inapp;
        
		Intent intent = new Intent(LoaderAPI.getActivity(), PurchaseActivity.class);
        LoaderAPI.getActivity().startActivity(intent);
	}
	
    public void purchaseProduct(String sku, String payload)
    {
        purchase(sku, true, payload);
    }
    
	public void purchaseSubscription(String sku, String payload)
    {
        purchase(sku, false, payload);
    }
    
	public void consume(String sku)
    {
		if (!_inventory.hasPurchase(sku))
		{
			safe_native_CONSUME_CALLBACK(new IabResult(IabHelper.BILLING_RESPONSE_RESULT_ITEM_NOT_OWNED, "Product haven't been purchased: " + sku));
			return;
		}

		Purchase purchase = _inventory.getPurchase(sku);
		_helper.consumeAsync(purchase, _billingCallback);
    }
	
	/**
     * Callback class for when a purchase or consumption process is finished
     */
    public class BillingCallback implements
            IabHelper.QueryInventoryFinishedListener,
            IabHelper.OnIabPurchaseFinishedListener,
            IabHelper.OnConsumeFinishedListener {

        @Override
        public void onQueryInventoryFinished(IabResult result, Inventory inventory) {
            Log.i(TAG, "Query inventory process finished.");
            if (result.isFailure()) {
                safe_native_INIT_CALLBACK(result);
                return;
            }

            Log.i(TAG, "Query inventory was successful. Init finished.");
            _inventory = inventory;
            safe_native_INIT_CALLBACK(result);
        }

        @Override
        public void onIabPurchaseFinished(IabResult result, Purchase purchase) {
            Log.i(TAG, "Purchase process finished: " + result + ", purchase: " + purchase);
            if (result.isFailure()) {
                Log.e(TAG, "Error purchasing: " + result);
                safe_native_PURCHASE_CALLBACK(result, null);
                return;
            }
            Log.i(TAG, "Purchase successful.");
            _inventory.addPurchase(purchase);
            safe_native_PURCHASE_CALLBACK(result, purchase);
        }

        @Override
        public void onConsumeFinished(Purchase purchase, IabResult result) {
            Log.i(TAG, "Consumption process finished. Purchase: " + purchase + ", result: " + result);

            if (result.isFailure()) {
                Log.e(TAG, "Error while consuming: " + result);
                safe_native_CONSUME_CALLBACK(result);
                return;
            }
            _inventory.erasePurchase(purchase.getSku());
            Log.d(TAG, "Consumption successful. Provisioning.");
            safe_native_CONSUME_CALLBACK(result);
        }
    }

    public static native void native_INIT_CALLBACK(int status, String error);
	
	public static native void native_PURCHASE_CALLBACK(int status, String error, Purchase purchase);

    public static native void native_CONSUME_CALLBACK(int status, String error);
	

	private static void safe_native_INIT_CALLBACK(IabResult res) {
        try {
            native_INIT_CALLBACK(res.getResponse(), res.getMessage());
        } catch (UnsatisfiedLinkError e) {
            Log.v(TAG, "No native handlers installed for native_INIT_CALLBACK, we received " + res.getResponse() + " " + res.getMessage());
        }
    }

    private static void safe_native_PURCHASE_CALLBACK(IabResult res, Purchase purchase) {
        try {
            native_PURCHASE_CALLBACK(res.getResponse(), res.getMessage(), purchase);
        } catch (UnsatisfiedLinkError e) {
            Log.v(TAG, "No native handlers installed for safe_native_PURCHASE_CALLBACK, we received " + res.getResponse() + " " + res.getMessage());
        }
    }
	
	private static void safe_native_CONSUME_CALLBACK(IabResult res) {
        try {
            native_CONSUME_CALLBACK(res.getResponse(), res.getMessage());
        } catch (UnsatisfiedLinkError e) {
            Log.v(TAG, "No native handlers installed for native_CONSUME_CALLBACK, we received " + res.getResponse() + " " + res.getMessage());
        }
    }
}
