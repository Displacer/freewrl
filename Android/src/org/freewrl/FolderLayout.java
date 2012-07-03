/*
  $Id: FolderLayout.java,v 1.4 2012/07/03 19:14:12 crc_canada Exp $

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

package org.freewrl;

import android.content.Context;
import android.widget.LinearLayout;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.TextView;
import android.widget.ListView;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.util.AttributeSet;
import android.view.View;
import android.view.LayoutInflater;
import android.util.Log;

import java.io.File;
import java.util.List;
import java.util.ArrayList;
import android.widget.Button;

// for setContentView
//import android.app.Activity;
//import android.os.Bundle;




   public class FolderLayout extends LinearLayout implements OnItemClickListener {

    Context context;
    IFolderItemListener folderListener;
    private List<String> item = null;
    private List<String> path = null;
    private String root = "/";
    private TextView myPath;
    private ListView lstView;
   //private FreeWRLView mView;
    private Button CancelButton;

    private View newFileView;

    private static String TAG = "FreeWRLView";

    public FolderLayout(Context context, AttributeSet attrs) {
        super(context, attrs);

        this.context = context;


        LayoutInflater layoutInflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        newFileView = layoutInflater.inflate(R.layout.folderview, this);

        myPath = (TextView) findViewById(R.id.path);
        lstView = (ListView) findViewById(R.id.list);
	CancelButton = (Button) findViewById(R.id.CancelButton);
	CancelButton.setOnClickListener(new View.OnClickListener() {
             public void onClick(View v) {
                 // Perform action on click
		newFileView.setVisibility(View.GONE);
             }
         });

        getDir(root, lstView);

    }

    public void setIFolderItemListener(IFolderItemListener folderItemListener) {
        this.folderListener = folderItemListener;
    }

    //Set Directory for view at anytime
    public void setDir(String dirPath){
        getDir(dirPath, lstView);
    }


    private void getDir(String dirPath, ListView v) {

        myPath.setText("Location: " + dirPath);
        item = new ArrayList<String>();
        path = new ArrayList<String>();
        File f = new File(dirPath);
        File[] files = f.listFiles();

        if (!dirPath.equals(root)) {

            item.add(root);
            path.add(root);
            item.add("../");
            path.add(f.getParent());

        }
        for (int i = 0; i < files.length; i++) {
            File file = files[i];
            //path.add(file.getPath());
            if (file.isDirectory()) {
            	path.add(file.getPath());
                item.add(file.getName() + "/");
            } else {
		if (!file.isHidden()) {
            		path.add(file.getPath());
                	item.add(file.getName());
		}
	    }
        }

        //Log.w(TAG, "local folders getDir returns " + files.length + "");

        setItemList(item);

    }

    //can manually set Item to display, if u want
    public void setItemList(List<String> item){
        ArrayAdapter<String> fileList = new ArrayAdapter<String>(context,
                R.layout.row, item);

        lstView.setAdapter(fileList);
        lstView.setOnItemClickListener(this);
    }


    public void onListItemClick(ListView l, View v, int position, long id) {
        File file = new File(path.get(position));
	// Log.w(TAG,"onListItemClick, listView " + l + " view " + v + " position " + position + " id " + id);

        if (file.isDirectory()) {
	//Log.w(TAG,"onListItemClick, directory");
            if (file.canRead())
                getDir(path.get(position), l);
            else {
		//what to do when folder is unreadable
                if (folderListener != null) {
                    folderListener.OnCannotFileRead(file);

                }

            }
        } else {

	//what to do when file is clicked
	//You can add more,like checking extension,and performing separate actions
	//Log.w(TAG,"onListItemClick, NOT directory");
            if (folderListener != null) {
		//Log.w(TAG,"onListItemClick, NOT dir + folderListener not null");
                folderListener.OnFileClicked(file);
            }

        }
    }

    public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
        // TODO Auto-generated method stub
        onListItemClick((ListView) arg0, arg0, arg2, arg3);
    }

}

