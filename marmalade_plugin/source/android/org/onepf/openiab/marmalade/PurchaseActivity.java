package org.onepf.openiab.marmalade;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import com.ideaworks3d.marmalade.LoaderAPI;
import com.ideaworks3d.marmalade.LoaderActivity;
import android.content.Intent;
import android.util.Log;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import org.onepf.oms.appstore.googleUtils.*;
import org.onepf.oms.*;

public class PurchaseActivity extends Activity
{
    private static final String TAG = "OpenIAB-Marmalade-PurchaseActivity";
    private static final String ACTION_FINISH = "org.onepf.openiab.ACTION_FINISH";
    private BroadcastReceiver _broadcastReceiver;
   
	@Override
	public void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
        
        _broadcastReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                Log.d(TAG, "Finish broadcast was received");
                if (!PurchaseActivity.this.isFinishing()) {
                    finish();
                }
            }
        };
        registerReceiver(_broadcastReceiver, new IntentFilter(ACTION_FINISH));
        
		OpenIAB.startPurchase(this);
	}    
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) 
	{
		OpenIAB.onActivityResult(requestCode, resultCode, data);	
		finish();
	}
};
