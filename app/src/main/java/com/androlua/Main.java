package com.androlua;

import android.content.Intent;
import android.os.Bundle;

public class Main extends LuaActivity
{

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if(savedInstanceState==null && getIntent().getData()!=null)
			runFunc("onNewIntent", getIntent());
		if(getIntent().getBooleanExtra("isVersionChanged",false) && (savedInstanceState==null)){
			onVersionChanged(getIntent().getStringExtra("newVersionName"),getIntent().getStringExtra("oldVersionName"));
		}
		//StatService.start(this);
	}

	@Override
	protected void onNewIntent(Intent intent)
	{
		runFunc("onNewIntent", intent);
		super.onNewIntent(intent);
	}
	
	@Override
	public String getLuaDir()
	{
		// TODO: Implement this method
		return getLocalDir();
	}

	@Override
	public String getLuaPath()
	{
		// TODO: Implement this method
		initMain();
		return getLocalDir()+"/main.lua";
	}

	private void onVersionChanged(String newVersionName, String oldVersionName) {
		// TODO: Implement this method
		runFunc("onVersionChanged", newVersionName, oldVersionName);

	}



}
