package org.mencus.game;

import java.util.EnumSet;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Toast;

import com.amazon.ags.api.AmazonGamesCallback;
import com.amazon.ags.api.AmazonGamesClient;
import com.amazon.ags.api.AmazonGamesFeature;
import com.amazon.ags.api.AmazonGamesStatus;

public class AmazonGameCirclePlugin extends MencusPlugin {
	public AmazonGameCirclePlugin(Activity activity) {
		super(activity);
	}

	//reference to the agsClient	
	AmazonGamesClient agsClient = null;
	 
	AmazonGamesCallback callback;
	 
	//list of features your game uses (in this example, achievements and leaderboards)
	EnumSet<AmazonGamesFeature> myGameFeatures;
		
	@Override
	public void onPause() {
	    if (agsClient != null) {
	        AmazonGamesClient.release();
	    }
	}

	@Override
	public void onResume() {
	    AmazonGamesClient.initialize(mActivity, callback, myGameFeatures);
	}
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
	    callback = new AmazonGamesCallback() {
	        @Override
	        public void onServiceNotReady(AmazonGamesStatus status) {
	            //unable to use service
	        	Toast.makeText(mActivity, "Unable to use GameCircle (" + status.toString() + ")", Toast.LENGTH_LONG).show();
	        }
	        @Override
	        public void onServiceReady(AmazonGamesClient amazonGamesClient) {
	            agsClient = amazonGamesClient;
	        	Toast.makeText(mActivity, "Ready to use GameCircle", Toast.LENGTH_LONG).show();
	            //ready to use GameCircle
	        }
	    };
	    myGameFeatures = EnumSet.of(
	        AmazonGamesFeature.Achievements, AmazonGamesFeature.Leaderboards);
	}

}
