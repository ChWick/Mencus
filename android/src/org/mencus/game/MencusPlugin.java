package org.mencus.game;

import android.app.Activity;
import android.os.Bundle;

public abstract class MencusPlugin {
	protected Activity mActivity;
	
	public MencusPlugin(Activity activity) {
		mActivity = activity;
	}
	
	public abstract void onCreate(Bundle savedInstanceState);
	public abstract void onPause();
	public abstract void onResume();
}
