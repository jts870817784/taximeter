package com.app.taximeter;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.fragment.app.FragmentTransaction;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;
import java.util.UUID;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    //定义三个fragment
    private Fragment f1;
    private Fragment2 f2;
    private Fragment3 f3;
    //定义底部3个按钮
    private Button  btn_home;
    private Button  btn_oth;
    private Button  btn_set;

    private FragmentTransaction mFragmentTransaction;

    private static final int DATA = 48;
    private static final String TAG = "Bluetooth";
    private static final int REQUEST_ENABLE_BT = 2;
    private BluetoothAdapter mBluetoothAdapter = null;
    private BluetoothDevice mdevice = null;
    private BluetoothSocket msocket = null;
//        private String macAddr = "00:20:12:08:32:F2";
    private String macAddr = "";
    public int his_order = 0;
    private UUID uuid = null;
    private boolean blue_connect_status = false;
    private ConnectedThread mConnectedThread = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        getmacAddr();

        gethistoryOrder();
        Log.d("main","his"+his_order);

        initFragment();

        Log.d("main","mac_addr"+macAddr);
        openBluetooth();
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

    private void gethistoryOrder() {
        File order;
        String data = "";
        int startIndex = -1;
        try {
            order = new File("data/data/com.app.taximeter/taximeter.txt");
            data = getFileContent(order);
            Log.d("gethistoryOrder", ""+data);
            if ((data.length() - DATA) > 105) {
                startIndex = data.length() - DATA;
            }
            Log.d("gethistoryOrder",""+startIndex+"|"+data.substring(startIndex,startIndex+5));
            if (startIndex > 0) {
                his_order = Integer.parseInt(data.substring(startIndex,startIndex+5));
            }

            Log.d("his_order", "his_order:" + his_order);
            writeTxtToFile("get order \n", "data/data/com.app.taximeter/", "log.txt");

        } catch (Exception e) {
            Log.i("error:", e + "");
            writeTxtToFile(e.toString()+"get orfer \n", "data/data/com.app.taximeter/", "log.txt");
        }
    }

    private void getmacAddr() {
        File file;
        try {
            file = new File("data/data/com.app.taximeter/blue_mac.txt");
            macAddr = getFileContent(file);
            writeTxtToFile("getmac \n", "data/data/com.app.taximeter/", "log.txt");
            Log.d("getmacAddr", "mac_addr:" + macAddr);

        } catch (Exception e) {
            Log.i("error:", e + "");
            writeTxtToFile(e.toString()+"getmac \n", "data/data/com.app.taximeter/", "log.txt");
        }
    }

    private void openBluetooth() {
        /* 打开蓝牙功能 */
        open_bluetooth();
        String log = "";
        Log.d(TAG, "open bluetooth"+macAddr.length());
        Log.d("openBlue", "mac_addr:" + macAddr);
        if  (macAddr.length() <= 0) {
            return;
        }
        mdevice = mBluetoothAdapter.getRemoteDevice(macAddr);
        uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");

        try {
            msocket = mdevice.createRfcommSocketToServiceRecord(uuid);
            Log.d(TAG,"msocket create");
            writeTxtToFile("msocket no e\n", "data/data/com.app.taximeter/", "log.txt");
        } catch (IOException e) {
            e.printStackTrace();
            writeTxtToFile(e.toString()+" e0 \n", "data/data/com.app.taximeter/", "log.txt");
        }

        new Thread() {
            @Override
            public void run() {
//                log += "cancelDiscovery\n";
                mBluetoothAdapter.cancelDiscovery();
                try {
                    Log.d(TAG,"connect status");
                    blue_connect_status = true;
                    msocket.connect();
                    writeTxtToFile("msocket connect \n", "data/data/com.app.taximeter/", "log.txt");

                    Log.d(TAG,"connect blue_connect_status:" + blue_connect_status);
                } catch (IOException e) {
//                    try {
//                        msocket.close();
//                        blue_connect_status = false;
//                        Log.d(TAG,"close");
//                    } catch (IOException e1) {
//                        e1.printStackTrace();
//                    }
                    e.printStackTrace();
                    writeTxtToFile(e.toString() + "e1", "data/data/com.app.taximeter/", "log.txt");
                }
                super.run();
                Log.d(TAG,"run");
            }
        }.start();

        log += "new connected thread \n" + blue_connect_status;
        mConnectedThread = new ConnectedThread(msocket);
        mConnectedThread.last_order = his_order;
        writeTxtToFile(log, "data/data/com.app.taximeter/", "log.txt");
        mConnectedThread.start();


//        while (true) {
//            if (msocket.isConnected()) {
//                if (his_order > 0) {
//                } else if (his_order == 0) {
//                    String s = "AA553B" + "0000" + "000000000000000000000000000000000000003B55AA";
//                    Log.d("connteted", "get bytes his order");
//                    mConnectedThread.write(ConnectedThread.toBytes(s));
//                }
//                break;
//            }
//            Log.d(TAG, "mConnectedThread.start()");
//        }

    }

    private void initFragment() {
        btn_home = (Button) findViewById(R.id.btn_home);
        btn_oth = (Button) findViewById(R.id.btn_oth);
        btn_set = (Button) findViewById(R.id.btn_set);

        btn_home.setOnClickListener(this);
        btn_set.setOnClickListener(this);
        btn_oth.setOnClickListener(this);

        /*初始化首界面*/
        mFragmentTransaction = getSupportFragmentManager().beginTransaction();
        if (f1 == null) {
            f1 = new Fragment1();
        }
        Toast.makeText(MainActivity.this, "initFragment1", Toast.LENGTH_SHORT).show();
        mFragmentTransaction.replace(R.id.Frame, f1);
//        mFragmentTransaction.addToBackStack(null); /*添加fragment到返回栈*/
        mFragmentTransaction.commit();
    }

    @Override
    public void onClick(View v) {//点击哪个按钮就显示哪个fragment;
        mFragmentTransaction = getSupportFragmentManager().beginTransaction();
        switch (v.getId()) {
            case R.id.btn_home:
                if (f1 == null) {
                    f1 = new Fragment();
                }
                Toast.makeText(MainActivity.this, "initFragment1", Toast.LENGTH_SHORT).show();
                mFragmentTransaction.replace(R.id.Frame, f1);
//                mFragmentTransaction.addToBackStack(null);//添加fragment到返回栈
                mFragmentTransaction.commit();
                break;
            case R.id.btn_set:
                if (f2 == null) {
                    f2 = new Fragment2();
                }
                Toast.makeText(MainActivity.this, "initFragment2", Toast.LENGTH_SHORT).show();
                mFragmentTransaction.replace(R.id.Frame, f2);
//                mFragmentTransaction.addToBackStack(null);//添加fragment到返回栈
                mFragmentTransaction.commit();
                break;
            case R.id.btn_oth:
                if (f3 == null) {
                    f3 = new Fragment3();
                }
                Toast.makeText(MainActivity.this, "initFragment3", Toast.LENGTH_SHORT).show();
                mFragmentTransaction.replace(R.id.Frame, f3);
//                mFragmentTransaction.addToBackStack(null);//添加fragment到返回栈
                mFragmentTransaction.commit();
                break;
        }

    }

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
                            content += line;
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

    // 将字符串写入到文本文件中
    private void writeTxtToFile(String strcontent, String filePath, String fileName) {
        //生成文件夹之后，再生成文件，不然会出错
        makeFilePath(filePath, fileName);

        String strFilePath = filePath + fileName;
        // 每次写入时，都换行写
        String strContent = strcontent + "\n";
        try {
            File file = new File(strFilePath);
            if (!file.exists()) {
                Log.d("TestFile", "Create the file:" + strFilePath);
                file.getParentFile().mkdirs();
                file.createNewFile();
            }
            RandomAccessFile raf = new RandomAccessFile(file, "rwd");
            raf.seek(file.length());
            raf.write(strContent.getBytes());
            raf.close();
        } catch (Exception e) {
            Log.e("TestFile", "Error on write File:" + e);
        }
    }

    //生成文件
    private File makeFilePath(String filePath, String fileName) {
        File file = null;
        makeRootDirectory(filePath);
        try {
            file = new File(filePath + fileName);
            if (!file.exists()) {
                file.createNewFile();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return file;
    }

    //生成文件夹
    private static void makeRootDirectory(String filePath) {
        File file = null;
        try {
            file = new File(filePath);
            if (!file.exists()) {
                file.mkdir();
            }
        } catch (Exception e) {
            Log.i("error:", e + "");
        }
    }


}
