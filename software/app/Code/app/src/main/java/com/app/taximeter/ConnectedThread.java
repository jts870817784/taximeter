package com.app.taximeter;

import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.util.Log;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.text.SimpleDateFormat;
import java.util.Date;

public class ConnectedThread extends Thread {
    private final BluetoothSocket mmSocket;
    private final InputStream mmInStream;
    private final OutputStream mmOutStream;
    private final Handler mHandler = null;
    public static final int MESSAGE_READ = 2;
    public static int connected_status = 0;

    /*
     * @ 报文格式
     *  HEAD  |  type/len  |            data                  |  sum  | TAIl
     * AA 55  |  xxx xxxxx | D0 D1 D2 D3 D4 D5 D6 D7 D8 ... DN|   +   | 55 AA
     *  D0  D1                        订单号
     *  D2  D3  D4  D5  D6  D7        起始时间(2000+D2=年 ，月日时分秒)
     *  D8  D9  D10 D11 D12 D13       结束时间
     *  D14 D15 D16 D17               里程数(米)
     *  D18 D19                       订单金额
     */

    /* packet */
    private final int HEAD[] = {170, 85};  /* AA 55 */
    private final int TAIL[] = {85, 170};  /* 55 AA */
    private final int REQUEST = 1;
    private int type_len = 0;
    private int order = 0;
    private int year = 2000;
    private int month = 0;
    private int day = 0;
    private int week = 0;
    private int clock = 0;
    private int minute = 0;
    private int seconds = 0;
    private double money = 0;
    private long meter = 0;

    public int last_order = 0;
    final String filePath = "data/data/com.app.taximeter/";
    final String fileName = "taximeter.txt";

    public ConnectedThread(BluetoothSocket socket) {
        mmSocket = socket;
        InputStream tmpIn = null;
        OutputStream tmpOut = null;

        // Get the input and output streams, using temp objects because
        // member streams are final
        try {
            tmpIn = socket.getInputStream();
            tmpOut = socket.getOutputStream();
        } catch (IOException e) { }

        if (tmpOut == null) {
            try {
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();

            } catch (IOException e) { }
            Log.d("write", "tmpout");
        }
        mmInStream = tmpIn;
        mmOutStream = tmpOut;

        Log.d("later_order", ""+last_order);
        File is_empty = new File(fileName+filePath);
        getFileContent(is_empty);
        Log.d("later_order", ""+last_order);

    }

    public void run() {

        Log.d("Bluetooth_connected","run");
        int [] sarry = new int[1000];
        int bytes; // bytes returned from read()
        int ch;

        bytes = 0;

        // Keep listening to the InputStream until an exception occurs
        while (true) {

//                 Send the obtained bytes to the UI activity
//                mHandler.obtainMessage(MESSAGE_READ, bytes, -1, buffer)
//                        .sendToTarget();
            if(mmSocket.isConnected()) {
                //Read from the InputStream
                try {
                    if (connected_status == 0) {
                        String s = "AA550555AA";
                        Log.d("First connected", "send request");
                        write(toBytes(s));
                        connected_status = 1;
                    }
                    ch = 0;
                    String data = "";
                    while (true) {
                        bytes = mmInStream.read();
                        sarry[ch] = bytes;
                        Log.d("sarry[ch]","sarry[" + ch + "]=" + sarry[ch]);
                        if (ch >= 27 && sarry[27] == TAIL[0]) {
                            Log.d("TAIL", "TAIL[0]");
                            if(ch == 28 && sarry[28] == TAIL[1]) {
                                Log.d("TAIL1","TAIL1");
                                break;
                            }
                        }
                        ch++;
                        if (ch > 100) {
                            break;
                        }
                        Log.d("data", bytes+"\n");
                    }

                    if(ch >= 25) {
                        data = packetParse(sarry);
                    } else {
                        Log.d("Packet", "Packet len err");
                    }

                    sarry = new int[1000];

                    Log.d("Packet", "Packet "+ data);

                    /* TODO：订单数据写入文件，需检测数据完整性 */
                    if (data.length() > 10) {
                        writeData(data);
                    }

                    Log.d("connteted", (data));

                } catch (IOException e) {

                }

//                if (bytes > 0) {
//                    String s = "AA553B"+Integer.toHexString(last_order)+"000000000000000000000000000000000000"+Integer.toHexString(last_order+59)+"55AA";
//                    Log.d("connteted", "write bytes");
//                    write(toBytes(s));
//                    bytes = 0;
//                }


            }
        }
    }

    /**
     * 将16进制字符串转换为byte[]
     *
     * @param str
     * @return
     */
    public static byte[] toBytes(String str) {
        if(str == null || str.trim().equals("")) {
            return new byte[0];
        }

        byte[] bytes = new byte[str.length() / 2];
        for(int i = 0; i < str.length() / 2; i++) {
            String subStr = str.substring(i * 2, i * 2 + 2);
            bytes[i] = (byte) Integer.parseInt(subStr, 16);
        }

        return bytes;
    }

    /* Call this from the main activity to send data to the remote device */
    public void write(byte[] bytes) {
        try {
            mmOutStream.write(bytes);
        } catch (IOException e) { }
    }

    /* Call this from the main activity to shutdown the connection */
    public void cancel() {
        try {
            mmSocket.close();
        } catch (IOException e) { }
    }

    private void writeData(String data) {
        writeTxtToFile(data, filePath, fileName);
    }
    //写入文件案例
//    private void writeData() {
//        String filePath = "/sdcard/Gyt/";
//        String fileName = "data.txt";
//        writeTxtToFile("Wx:lcti1314", filePath, fileName);
//    }


    /* 检测数据是否合法 */
    private String packetParse(int[] buffer) {
        String data = "";
        int i;
        int sum = 0;

        year = 2000;
        money = 0;
        order = 0;
        meter = 0;
        for(i = 0; i < 25; i++) {
            Log.d("buffer","buffer[" + i + "]=" + buffer[i]);
            if(buffer[0] != HEAD[0] && buffer[1] != HEAD[1]) {
                Log.d("Packet", "HEAD ERR");
                return "-1";
            }

            if(i == 3) {
                order = buffer[i] + buffer[i+1]*256;
                data += order + "|";
                if (order > last_order) {
                    last_order = order;
                } else  {
                    return "-1";
                }

                Log.d("Order","order" + order);
            }

            if(i == 5) {
                seconds = buffer[i];
                minute = buffer[i+1];
                clock = buffer[i+2];
                week = buffer[i+3];
                day = buffer[i+4];
                month = buffer[i+5];
                year = buffer[i+6]+2000;
                data +=  String.format("%02d", month)+String.format("%02d", day)+String.format("%02d", week);
                data += String.format("%02d", clock)+String.format("%02d", minute)+String.format("%02d", seconds)+ "|";
                Log.d("date", year + "year" + month + "month" + day + "day\n");
            }
            if(i==12) {
                seconds = buffer[i];
                minute = buffer[i+1];
                clock = buffer[i+2];
                week = buffer[i+3];
                day = buffer[i+4];
                month = buffer[i+5];
                year = buffer[i+6]+2000;
                data += String.format("%02d", month)+String.format("%02d", day)+String.format("%02d", week);
                data += String.format("%02d", clock)+String.format("%02d", minute)+String.format("%02d", seconds) + "|";
                Log.d("date", year + "year" + month + "month" + day + "day\n");
            }

            if(i == 19) {
                meter = buffer[i+3]*256*256*256 + buffer[i+2]*256*256 + buffer[i+1]*256 + buffer[i];
                data += String.format("%010d", meter) + "|";
                Log.d("meter", meter + "meter");
            }

            if(i == 23) {
                money = buffer[i]*256/100 + buffer[i+1]/100;
                Log.d("money", money + String.format("%4.1f", (double)((double)(buffer[i]*256 + buffer[i+1])/100))+"元");
                data += String.format("%5.1f", (double)((double)(buffer[i]*256 + buffer[i+1])/100));
            }

            sum += buffer[i];
        }

        if (sum != (buffer[26]*256 + buffer[27])) {
            Log.d("SUM","error");
        }

        return data;
    }

    /* 将数据写入taximeter文件库 */
    private int ifDataExist(int order) {
        order = 0;
        return 0;
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
    private void getFileContent(File file) {
        String content = "";
        String str_order = "";
        Log.d("getFileContent", "start");
        int i = 0;
        if (!file.isDirectory()) {  //检查此路径名的文件是否是一个目录(文件夹)
            Log.d("getFileContent", "start 364");
            if (file.getName().endsWith("txt")) {//文件格式为""文件
                Log.d("getFileContent", "start 366 1");
                try {
                    Log.d("getFileContent", "start 366");
                    InputStream instream = new FileInputStream(file);
                    if (instream != null) {
                        InputStreamReader inputreader
                                = new InputStreamReader(instream, "UTF-8");
                        BufferedReader buffreader = new BufferedReader(inputreader);
                        String line = "";
                        //分行读取
                        Log.d("getFileContent", "start 374");
                        while ((line = buffreader.readLine()) != null) {
                            Log.d("getFileContent", "start 376");
                            content = line + "\n";
                            Log.d("is_empty", "" + line);
                            if (i >= 1) {
                                str_order = content.substring(0, 5);
                                Log.d("getFileContent", i+" "+str_order);
                                last_order = Integer.parseInt(str_order);
                            }
                            i++;
                        }
                        instream.close();//关闭输入流
                        Log.d("is_empty", "" +last_order);
                    }
                } catch (java.io.FileNotFoundException e) {
                    Log.d("TestFile", "The File doesn't not exist.");
                } catch (IOException e) {
                    Log.d("TestFile", e.getMessage());
                }
            }
        }
        return;
    }
}
