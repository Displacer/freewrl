/*
$Id: FreeWRLAssets.java,v 1.7 2012/08/08 15:04:54 crc_canada Exp $

*/

/****************************************************************************
This file is part of the FreeWRL/FreeX3D Distribution.

Copyright 2012 CRC Canada. (http://www.crc.gc.ca)

FreeWRL/FreeX3D is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

FreeWRL/FreeX3D is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FreeWRL/FreeX3D.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

/* Notes:

An Asset, in FreeWRL terms, is a resource (vrml file, jpg file, etc) that resides
*SOMEWHERE*.

It can reside within the FreeWRL apk (eg, font files reside there), or it can
reside on the SD card.

We ALWAYS look in the assets in the apk file first; if not there, then we go elsewhere.

*/


package org.freewrl;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

import java.io.IOException;

import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.BufferedInputStream;
import android.content.res.AssetManager;
import android.content.res.AssetFileDescriptor;
import java.io.FileDescriptor;
import android.content.res.Resources;

import android.content.Context;


import java.io.File;
import java.io.FileReader;
import java.io.FileNotFoundException;
import java.net.URLConnection; //file type guessing
import java.io.InputStream;


public class FreeWRLAssets {

	private static String TAG = "FreeWRLView";
	
	// open a file. 
	// first, see if it is within the FreeWRL apk "assets" directory.
	// second, go through the file system and open it.
	// the FileDescriptor is ONLY used for getting font files from the
	// apk assets directory; libfreetype wants the file compressed
	// and un touched.

	public FreeWRLAssetData openAsset(Context context, String path )
	{
		Integer offset = 0;
		Integer length = 0;
		FileDescriptor fd = null;
		InputStream imgFile = null;

		// Dave Joubert sent this in 5 August 2011
		Log.w(TAG,"---------------------- ** START ** -------------------------");
		Log.w(TAG, path);;

		Log.w(TAG, "file " + new Throwable().getStackTrace()[0].getFileName() +
			" class " + new Throwable().getStackTrace()[0].getClassName() +
			" method " + new Throwable().getStackTrace()[0].getMethodName() +
			" line " + new Throwable().getStackTrace()[0].getLineNumber());
		Log.w(TAG, "....From : " +
			" class " + new Throwable().getStackTrace()[1].getClassName() +
			" method " + new Throwable().getStackTrace()[1].getMethodName() +
			" line " + new Throwable().getStackTrace()[1].getLineNumber());
		Log.w(TAG, "......From : " +
			" class " + new Throwable().getStackTrace()[2].getClassName() +
			" method " + new Throwable().getStackTrace()[2].getMethodName() +
			" line " + new Throwable().getStackTrace()[2].getLineNumber());

		// Step 1 - is this in the FreeWRL Assets folder??
		if (path.indexOf('/') == 0) {
			Log.w(TAG,"---------------- GOING TO OPEN ASSET FILE ------------------");
			Log.w(TAG," guessing it is a " + URLConnection.guessContentTypeFromName(path));

			Log.w(TAG, "file " + new Throwable().getStackTrace()[0].getFileName() +
				" class " + new Throwable().getStackTrace()[0].getClassName() +
				" method " + new Throwable().getStackTrace()[0].getMethodName() +
				" line " + new Throwable().getStackTrace()[0].getLineNumber());
			// remove slash at the beginning, if it exists
			// as Android assets are not root based but getwd returns root base.
			// And we know it is not an URL based filename
			String tryInAppAssets = path.substring(1);

			try {
				AssetFileDescriptor ad = context.getResources().getAssets().openFd(tryInAppAssets);
				if (ad != null) {
					ad.close();
					Log.w(TAG," Best guess: "+path+" is an asset ");
					FreeWRLAssetData rv = new FreeWRLAssetData(context, tryInAppAssets, 1);
					if(null != rv) {
						Log.w(TAG, "file " + new Throwable().getStackTrace()[0].getFileName() +
							" class " + new Throwable().getStackTrace()[0].getClassName() +
							" method " + new Throwable().getStackTrace()[0].getMethodName() +
							" line " + new Throwable().getStackTrace()[0].getLineNumber());
						Log.w(TAG, "rv: offset = "+rv.offset+" , length= "+rv.length+" ,  knownType= "+rv.knownType) ;
						return rv;
					}
				}
			} catch( IOException e ) {
				// this is not really an error - it just indicates that the file is not
				// within the FreeWRL apk - we'll look elsewhere
				Log.e( TAG, "openAsset - not an Asset: " + e.toString() );
			}
		}

		// Step 2 - if not an Asset in the FreeWRL Apk, try the exact path
	
		//Log.w(TAG,"openAsset: Obviously NOT in the applications asset area");
		InputStream in = null;
		try {
			in = new BufferedInputStream(new FileInputStream(path));
		} catch (FileNotFoundException e) {
			Log.e(TAG, "Couldn't find or open this file " + path);
			return new FreeWRLAssetData(0,0,null,null);
		}
	
		Log.w (TAG,"successfully opened " + path);

		// got this in the /mnt/sdcard (or wherever it resides) directory.
		return new FreeWRLAssetData(0,0,fd,in);
	}
}
