package com.app.taximeter;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.fragment.app.Fragment;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;
import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Enumeration;

import static java.lang.Boolean.FALSE;

public class Fragment1 extends Fragment {

    private int order=0;
    private double profit_s=0;
    private double day_profit_s=0;
    private int order_num_s;

    private Button profit;
    private Button day_profit;
    private Button order_num;

    private TextView profit_v;
    private TextView day_profit_v;
    private TextView order_num_v;
    final String filePath = "data/data/com.app.taximeter/";
    final String fileName = "taximeter.txt";

    /* 格式
     * order|   start date |   end   date | meter/mi |money/yuan
     * 10000|20200411221034|20200411221034|0000082000|1111.1
     * 10001|20210411221034|20200411221034|0000082000|1111.3
     */

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        Orders_info  tmp_order;
        File tax = new File(filePath+fileName);
        View view = inflater.inflate(R.layout.frame_layout, container,false);
        profit = view.findViewById(R.id.profit);
        day_profit_v = view.findViewById(R.id.day_profit_v);
        profit_v = view.findViewById(R.id.profit_v);
        order_num_v = view.findViewById(R.id.order_num_v);
        tmp_order = getFileContent(tax);
        if (!tax.exists()) {
            writeTxtToFile("order|   start date |   end   date | meter/mi |money/yuan", filePath, fileName);
        }
        Log.d("order","tmp_order"+tmp_order);
        update_content(tmp_order);
        profit.setOnClickListener(new View.OnClickListener() {
            Orders_info  tmp_order_click;
            @Override
            public void onClick(View v) {
                tmp_order_click = getFileContent(tax);
                /* 更新页面显示 */
                if (tmp_order != null)
                    update_content(tmp_order);
            }
        });

        return view;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onActivityCreated(savedInstanceState);
    }

    private void update_content(Orders_info info) {
        if(info.money > 0) {
            DecimalFormat df = new DecimalFormat(".#");
            day_profit_v.setText(df.format(info.day_profit) + "元");
            profit_v.setText(df.format(info.money) + "元");
            order_num_v.setText(Integer.toString(info.order_num) + "单");
        }
    }

    /*读取指定目录下的所有TXT文件的文件内容并解析相关数据*/
    private Orders_info getFileContent(File file) {
        String content = "";
        String str_order = "";
        int i = 0;

        Date d = new Date();
        SimpleDateFormat sdf = new SimpleDateFormat("MMdd");
        String today = sdf.format(d);
        Log.d("date", today+"xxx");

        Orders_info ordersInfo = new Orders_info();
        Log.d("date", " 104");
        if (!file.isDirectory()) {  //检查此路径名的文件是否是一个目录(文件夹)
            Log.d("date", " 106");
            if (file.getName().endsWith("txt")) {//文件格式为""文件
                Log.d("date", " 108");
                try {
                    InputStream instream = new FileInputStream(file);
                    if (instream != null) {
                        InputStreamReader inputreader
                                = new InputStreamReader(instream, "UTF-8");
                        BufferedReader buffreader = new BufferedReader(inputreader);
                        String line = "";
                        //分行读取
                        while ((line = buffreader.readLine()) != null) {
                            content = line + "\n";
                            if (i >=1) {
                                str_order = content.substring(0, 5);
                                Log.d("str_order", i+" "+str_order);
                                ordersInfo.order = Integer.parseInt(str_order);
                                ordersInfo.start_date = content.substring(6, 15);
                                Log.d("str_order", i+" "+str_order);
                                ordersInfo.end_date = content.substring(16, 23);

                                str_order = content.substring(32, 42);
                                Log.d("str_order", i+" "+str_order);
                                ordersInfo.meter = Long.parseLong(str_order);

                                str_order = content.substring(43, content.indexOf("\n"));
                                Log.d("str_order", i+" "+str_order);
                                ordersInfo.money += Double.parseDouble(str_order);
                                Log.d("equals","a"+today.equals(content.substring(6, 14))+content.substring(6, 14));
                                if (today.equals(content.substring(6, 10))) {
                                    ordersInfo.day_profit += Double.parseDouble(str_order);
                                    Log.d("date", content.substring(6, 10));
                                }
                                Log.d("readline", "content" + content);
                                ordersInfo.order_num++;
                            }
                            i++;
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
        return ordersInfo;
    }

    class Orders_info {
        int order=0;
        String start_date="";
        String end_date="";
        long meter=0;
        double money=0;
        int order_num=0;
        double day_profit=0;
        void call_order(String order1, String profit1, String date){

            Log.d("Orders","order"+order+"profit"+profit);
        }
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
