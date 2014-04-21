package org.mencus.game;

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.InterstitialAd;

import android.app.Dialog;
import android.app.NativeActivity;
import android.os.Bundle;
import android.widget.PopupWindow;
import android.widget.TextView;
import android.widget.Toast;

public class MencusNativeActivity extends NativeActivity {
	PopupWindow popUp;

	MencusNativeActivity _activity;
	private InterstitialAd mInterstitial;
	private Runnable mThread;
	private boolean mAddClosed = false;
	private boolean mShowAdWhenLoaded = true;
	private Dialog mLoadDialog = null;
	
	InterstitialAd getInterstitialAd() {return mInterstitial;}
	void setAdClosed(boolean addClosed) {mAddClosed = addClosed;}
	boolean showAdWhenLoaded() {return mShowAdWhenLoaded;}

	public void onCreate(Bundle savedInstanceState) {
		_activity = this;
		super.onCreate(savedInstanceState);
        //Toast.makeText(this, "Loading, please wait...", Toast.LENGTH_LONG).show();
		//showAdPopup();
        //Toast.makeText(this, "popup done", Toast.LENGTH_SHORT).show();

        mLoadDialog = new Dialog(this, android.R.style.Theme_Black_NoTitleBar_Fullscreen);
        mLoadDialog.setContentView(R.layout.activity_mencus);
        mLoadDialog.show();
	}

	// Our popup window, you will call it from your C/C++ code later
	public void showAdPopup() {
		_activity.runOnUiThread(new Runnable() {
			public void run() {
				mAddClosed = false;
				if (mInterstitial != null && mInterstitial.isLoaded()) {
					mInterstitial.show();
				}
				else {
					closeAd();
					loadAd(true);
				}
			}
		});
	}
	
	public boolean adPopupClosed() {
		return mAddClosed;
	}
	
	public void closeAd() {
		mShowAdWhenLoaded = false;
		mInterstitial = null;
		mThread = null;
		mAddClosed = true;
	}
	
	public void preloadAd() {
		loadAd(false);
	}
	private void loadAd(boolean showWhenLoaded) {
		mAddClosed = false;
		mShowAdWhenLoaded = showWhenLoaded;
		mThread = new Runnable()  {
			 @Override
			 public void run()  {
			    //Toast.makeText(_activity, "Loading, please wait...", Toast.LENGTH_LONG).show();
			    mInterstitial = new InterstitialAd(_activity);
		        mInterstitial.setAdUnitId("ca-app-pub-1316406434207336/9422601805");
		        mInterstitial.setAdListener(new MencusAdListener(_activity));
		        mInterstitial.loadAd(new AdRequest.Builder()
	        		//.addTestDevice(AdRequest.DEVICE_ID_EMULATOR)
	        		//.addTestDevice("5379191936D78128252BAEC2CA476429")
	        		.addTestDevice("5379191936D78128252BAEC2CA476429")
	        		.build());
			 }
		 };
		 _activity.runOnUiThread(mThread);
	}
	
	public void setLoadText(String text) {
		TextView tv = (TextView)mLoadDialog.findViewById(R.id.mencus_load_text);
		tv.setText(text);
	}
	public void closeLoadDialog() {
		mLoadDialog.dismiss();
		mLoadDialog = null;
	}
}
