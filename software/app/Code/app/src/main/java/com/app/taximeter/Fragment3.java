package com.app.taximeter;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Toast;

import androidx.fragment.app.Fragment;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;

public class Fragment3 extends Fragment {
    private ImageButton bind;
    private EditText inputmac;
    final String filePath = "data/data/com.app.taximeter/";
    final String fileName = "blue_mac.txt";
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.frame_layout3, container,false);
        bind = (ImageButton) view.findViewById(R.id.btn_bind);
        inputmac = view.findViewById(R.id.mac_bind);
        inputmac.setVisibility(8);
        return view;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onActivityCreated(savedInstanceState);

        bind.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                inputmac.setVisibility(0); //0--->可见  4--->不可见占着空间   8--->不可见，不占空间
//                Toast.makeText(getActivity(), "11", Toast.LENGTH_LONG).show();

                Log.d("MAC Write", "Length:" + inputmac.getText().toString().length() );
                if (inputmac.getText().toString().length() == 17) {
                    //写入文件中
                    writeTxtToFile(inputmac.getText().toString(), filePath, fileName);
                }

                File mac_file = new File(filePath+fileName);
                if (mac_file != null) {
                    String mac_content = getFileContent(mac_file);

                    Toast.makeText(getContext(),mac_content, Toast.LENGTH_LONG).show();
                    Log.d("MAC Write", "MAC:" + mac_content );
//                    if (mac_content.length() >= 12) {
//                        inputmac.setVisibility(8);
//                    }
                }
            }
        });
    }

    //写入文件案例
//    private void writeData() {
//        String filePath = "/sdcard/Gyt/";
//        String fileName = "data.txt";
//        writeTxtToFile("Wx:lcti1314", filePath, fileName);
//    }

    // 将字符串写入到文本文件中
    private void writeTxtToFile(String strcontent, String filePath, String fileName) {
        //生成文件夹之后，再生成文件，不然会出错
        makeFilePath(filePath, fileName);

        String strFilePath = filePath + fileName;
        // 每次写入时，都换行写
        String strContent = strcontent + "\r\n";
        try {
            File file = new File(strFilePath);
            /* 如果存在则删除重新创建 */
            if (file.exists()) {
                file.delete();
            }

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
