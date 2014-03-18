package org.mencus.game;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.InterstitialAd;

import android.app.NativeActivity;
import android.content.Intent;
import android.os.Bundle;
import android.widget.PopupWindow;
import android.widget.Toast;

public class MencusNativeActivity extends NativeActivity {
	PopupWindow popUp;

	MencusNativeActivity _activity;
	private InterstitialAd mInterstitial;
	private boolean mAddClosed = false;
	
	InterstitialAd getInterstitialAd() {return mInterstitial;}
	void setAdClosed(boolean addClosed) {mAddClosed = addClosed;}

	public void onCreate(Bundle savedInstanceState) {
		_activity = this;
		super.onCreate(savedInstanceState);
        Toast.makeText(this, "Loading, please wait...", Toast.LENGTH_LONG).show();
		//showAdPopup();
        //Toast.makeText(this, "popup done", Toast.LENGTH_SHORT).show();


	}

	// Our popup window, you will call it from your C/C++ code later
	public void showAdPopup() {
		mAddClosed = false;
		 _activity.runOnUiThread(new Runnable()  {
			 @Override
			 public void run()  {
			    Toast.makeText(_activity, "Loading, please wait...", Toast.LENGTH_LONG).show();
			    mInterstitial = new InterstitialAd(_activity);
		        mInterstitial.setAdUnitId("ca-app-pub-1316406434207336/9422601805");
		        mInterstitial.setAdListener(new MencusAdListener(_activity));
		        mInterstitial.loadAd(new AdRequest.Builder()
	        		//.addTestDevice(AdRequest.DEVICE_ID_EMULATOR)
	        		//.addTestDevice("5379191936D78128252BAEC2CA476429")
	        		.addTestDevice("5379191936D78128252BAEC2CA476429")
	        		.build());
			 }
		 });
	}
	
	public boolean adPopupClosed() {
		return mAddClosed;
	}
}
