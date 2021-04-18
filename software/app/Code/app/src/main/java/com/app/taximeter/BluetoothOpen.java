package com.app.taximeter;

import androidx.appcompat.app.AppCompatActivity;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Set;
import java.util.UUID;

public class BluetoothOpen extends AppCompatActivity {
    /* Debug */
    private static final String TAG = "Bluetooth";
    private static final int REQUEST_ENABLE_BT = 2;
    private BluetoothAdapter mBluetoothAdapter = null;
    private BluetoothDevice mdevice = null;
    private BluetoothSocket msocket = null;
    private String macAddr = "00:20:12:08:32:F2";
    private UUID uuid = null;
    private boolean blue_connect_status = false;
    private ConnectedThread mConnectedThread = null;
    final String filePath = "data/data/com.app.taximeter/";
    final String fileName = "blue_mac.txt";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        File mac_file = new File(filePath+fileName);

        if (mac_file != null) {
            macAddr = getFileContent(mac_file);
        }

        super.onCreate(savedInstanceState);

        /* 打开蓝牙功能 */
        open_bluetooth();
        Log.d(TAG, "open bluetooth");
        if (macAddr.length() == 17) {
            mdevice = mBluetoothAdapter.getRemoteDevice(macAddr);
        }
        uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");

        try {
            msocket = mdevice.createRfcommSocketToServiceRecord(uuid);
            Log.d(TAG,"msocket create");
        } catch (IOException e) {
            e.printStackTrace();
        }

        new Thread() {
            @Override
            public void run() {
                mBluetoothAdapter.cancelDiscovery();
                try {
                    Log.d(TAG,"connect");
                    blue_connect_status = true;
                    msocket.connect();
                    Log.d(TAG,"connect blue_connect_status:" + blue_connect_status);
                } catch (IOException e) {
//                    try {
//                        msocket.close();
//                        blue_connect_status = false;
//                        Log.d(TAG,"close");
//                    } catch (IOException e1) {
//                        e1.printStackTrace();
//                    }
//                    e.printStackTrace();
                }
                super.run();
                Log.d(TAG,"run");
            }
        }.start();

        Log.d(TAG, "new thread!");
        String s = "come from app";
        byte[] bytes = s.getBytes();

        mConnectedThread = new ConnectedThread(msocket);
        mConnectedThread.start();
        if (blue_connect_status == true) {
            Log.d(TAG, "every is ok");
//        mConnectedThread.write(bytes);
        }
    }

    private void open_bluetooth() {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
            // Device does not support Bluetooth
            Log.d(TAG, "Device does not support Bluetooth");
        } else {
            Log.d(TAG, "Get Device Bluetooth Adapter");
        }

        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
        Log.d(TAG, "Bluetooth is Enable");
    }

    //读取指定目录下的所有TXT文件的文件内容
    private String getFileContent(File file) {
        String content = "";
        if (!file.isDirectory()) {  //检查此路径名的文件是否是一个目录(文件夹)
            if (file.getName().endsWith("txt")) {//文件格式为""文件
                try {
                    InputStream instream = new FileInputStream(file);
                    if (instream != null) {
                        InputStreamReader inputreader
                                = new InputStreamReader(instream, "UTF-8");
                        BufferedReader buffreader = new BufferedReader(inputreader);
                        String line = "";
                        //分行读取
                        while ((line = buffreader.readLine()) != null) {
                            content += line + "\n";
                        }
                        instream.close();//关闭输入流
                    }
                } catch (java.io.FileNotFoundException e) {
                    Log.d("TestFile", "The File doesn't not exist.");
                } catch (IOException e) {
                    Log.d("TestFile", e.getMessage());
                }
            }
        }
        return content;
    }
}