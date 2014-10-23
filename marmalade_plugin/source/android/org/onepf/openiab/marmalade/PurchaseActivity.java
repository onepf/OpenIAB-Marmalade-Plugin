package org.onepf.openiab.marmalade;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import com.ideaworks3d.marmalade.LoaderAPI;
import com.ideaworks3d.marmalade.LoaderActivity;
import android.content.Intent;
import android.util.Log;
import org.onepf.oms.appstore.googleUtils.*;
import org.onepf.oms.*;

public class PurchaseActivity extends Activity
{
    private static final String TAG = "OpenIAB-Marmalade-PurchaseActivity";
   
	@Override
	public void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		OpenIAB.startPurchase(this);
	}    
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) 
	{
		OpenIAB.onActivityResult(requestCode, resultCode, data);	
		finish();
	}
};
