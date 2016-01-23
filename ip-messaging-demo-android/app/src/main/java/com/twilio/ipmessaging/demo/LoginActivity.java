package com.twilio.ipmessaging.demo;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.provider.Settings.Secure;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Toast;
import android.util.Log;

import com.google.android.gms.iid.InstanceID;
import com.twilio.ipmessaging.Constants.StatusListener;
import com.twilio.ipmessaging.TwilioIPMessagingSDK;
import com.twilio.ipmessaging.demo.BasicIPMessagingClient.LoginListener;

import java.io.IOException;
import java.net.URLEncoder;


public class LoginActivity extends Activity implements LoginListener {
	private static final String ACCESS_TOKEN_SERVICE_URL = BuildConfig.ACCESS_TOKEN_SERVICE_URL;
	private static final String DEFAULT_CLIENT_NAME = "TestUser";
	private ProgressDialog progressDialog;
	private Button login;
	private Button logout;
	private CheckBox gcmCxbx;
	private Button stopGCM;
	private String accessToken = null;
	private EditText clientNameTextBox;
	private BasicIPMessagingClient chatClient;
	private String endpoint_id = "";
	public static String local_author = DEFAULT_CLIENT_NAME;
    private String PROJECT_NUMBER = "215048275735";
    private EditText etRegId;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_login);

		
		this.clientNameTextBox = (EditText) findViewById(R.id.client_name);
		this.clientNameTextBox.setText(DEFAULT_CLIENT_NAME);
		this.endpoint_id = Secure.getString(this.getApplicationContext().getContentResolver(), Secure.ANDROID_ID);

		this.login = (Button) findViewById(R.id.register);
		this.login.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				String idChosen = clientNameTextBox.getText().toString();
				String endpointIdFull = idChosen + "-" + LoginActivity.this.endpoint_id + "-android-"+ getApplication().getPackageName();

				StringBuilder url = new StringBuilder();
				url.append(ACCESS_TOKEN_SERVICE_URL);

				Log.v("Log", "url string : " + url.toString());
				new GetCapabilityTokenAsyncTask().execute(url.toString());
			}
		});

		this.logout = (Button) findViewById(R.id.logout);
		etRegId = (EditText) findViewById(R.id.etRegId);
		chatClient = TwilioApplication.get().getBasicClient();

		gcmCxbx = (CheckBox) findViewById(R.id.gcmcxbx);

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.login, menu);
		return true;
	}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		int id = item.getItemId();
		if (id == R.id.action_about) {
			showAboutDialog();
		}
		return super.onOptionsItemSelected(item);
	}

	private void showAboutDialog() {
		AlertDialog.Builder builder = new AlertDialog.Builder(LoginActivity.this);
		builder.setTitle("About").setMessage("Version: " + TwilioIPMessagingSDK.getVersion()).setPositiveButton("OK",
				new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						dialog.cancel();
					}
				});

		AlertDialog aboutDialog = builder.create();
		aboutDialog.show();
	}

	private class GetCapabilityTokenAsyncTask extends AsyncTask<String, Void, String> {
		private String urlString;
		@Override
		protected void onPostExecute(String result) {
			super.onPostExecute(result);
			LoginActivity.this.chatClient.doLogin(accessToken, LoginActivity.this, urlString);
		}

		@Override
		protected void onPreExecute() {
			super.onPreExecute();
			LoginActivity.this.progressDialog = ProgressDialog.show(LoginActivity.this, "",
					"Logging in. Please wait...", true);
		}

		@Override
		protected String doInBackground(String... params) {
			try {

				urlString = params[0];
				accessToken = "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiIsImN0eSI6InR3aWxpby1mcGE7dj0xIn0.eyJqdGkiOiJTSzZjMzY4M2VmZTAyNmFmNjA4MDNiY2ZkZjg5ODYwOTkxLTE0NTM1MDgwOTUiLCJpc3MiOiJTSzZjMzY4M2VmZTAyNmFmNjA4MDNiY2ZkZjg5ODYwOTkxIiwic3ViIjoiQUNlM2MxZDU0ODgwMTMxODEwOWRlNTA5MzY1NDRiZWU4NiIsIm5iZiI6MTQ1MzUwODA5NSwiZXhwIjoxNDUzNTExNjk1LCJncmFudHMiOnsiaWRlbnRpdHkiOiJZYXduaW5nT3dlblV0aWNhIiwiaXBfbWVzc2FnaW5nIjp7InNlcnZpY2Vfc2lkIjoiSVM0NTRhOGY3ZWFmNWE0ZjRmOTU4ZjAxZDAxMWMwNDhmMyIsImVuZHBvaW50X2lkIjoiVHdpbGlvQ2hhdERlbW86WWF3bmluZ093ZW5VdGljYToifX19.0kQujTOFowgkazU41GWY3ov1q0iUVkevNBBuPpqFp6A";
				//accessToken = HttpHelper.httpGet(params[0]);

				chatClient.setAccessToken(accessToken);
			} catch (Exception e) {
				e.printStackTrace();
			}
			return accessToken;
		}
	}

	@Override
	public void onLoginStarted() {
		Log.v("Log", "Log in started");
	}
	
		@Override
		public void onLoginFinished() {
			LoginActivity.this.progressDialog.dismiss();
			Intent intent = new Intent(this, ChannelActivity.class);
			this.startActivity(intent);
		}

	@Override
	public void onLoginError(String errorMessage) {
		LoginActivity.this.progressDialog.dismiss();
		Toast.makeText(getBaseContext(), errorMessage, Toast.LENGTH_LONG).show();
	}

	@Override
	public void onLogoutFinished() {
		// TODO Auto-generated method stub
	}
	


}
