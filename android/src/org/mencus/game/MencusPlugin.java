package org.mencus.game;

import android.app.Activity;
import android.os.Bundle;

public abstract class MencusPlugin {
	protected Activity mActivity;
	private boolean mStarted;
	
	public MencusPlugin(Activity activity) {
		mActivity = activity;
		mStarted = false;
	}
	
	public boolean isStarted() {return mStarted;}
	public void start() {
		mStarted = true;
		onCreate();
		onResume();
	}
	
	public void onCreate() {
		if (mStarted) {
			onCreateImpl();
		}
	}
	public void onDestroy() {
		if (mStarted) {
			onDestroyImpl();
		}
	}
	public void onPause() {
		if (mStarted) {
			onPauseImpl();
		}
	}
	public void onResume() {
		if (mStarted) {
			onResumeImpl();
		}
	}
	
	protected abstract void onCreateImpl();
	protected abstract void onDestroyImpl();
	protected abstract void onPauseImpl();
	protected abstract void onResumeImpl();
}
