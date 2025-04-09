
import java.io.IOException;
import java.util.*;
import java.io.*;
import java.net.*;


public class Read_Thread_Server implements Runnable{
    private int max_buffer_size, min_chunk_size,max_chunk_size;
    private String client_name;
    private Thread thread;
    private Network_Utilize network_utilize;
    private List<String> list_of_client;
    public HashMap<String,Network_Utilize> connected_clients;
    public HashMap<String,List<String>> inbox;
    public HashMap<Integer,String> request_id_map;
    public Total_Chunk_In_Buffer tcib;

    private int file_id = 0;


    public Read_Thread_Server(HashMap<String,Network_Utilize> map, List<String> list,HashMap<String,List<String>> inb,HashMap<Integer,String> request, Network_Utilize nu, int n1, int n2, int n3, String name,Total_Chunk_In_Buffer tci){
        connected_clients = map;
        list_of_client = list;
        inbox = inb;
        request_id_map = request;
        tcib = tci;

        network_utilize = nu;
        max_buffer_size = n1;
        min_chunk_size = n2;
        max_chunk_size = n3;
        client_name = name;
        thread = new Thread(this);
        thread.start();

    }

    public void run(){

        try {
            Random random = new Random();


            while (true){

                Object o = network_utilize.read();

                if(o instanceof Message){
                    Message obj = (Message) o;
                    String to = obj.getText();
                    String[] tokens = to.split(",");

                    if(tokens[0].equalsIgnoreCase("UPLOAD")){
                        if(max_buffer_size>= tcib.getN() +Integer.parseInt(tokens[3])){
                            tcib.setN(tcib.getN() + Integer.parseInt(tokens[3]) );
                            network_utilize.write("buffer_overflow_not_occur");
                            int tttt= Integer.parseInt(tokens[3]);


                            file_id++;
                            int r1 = random.nextInt(max_chunk_size - min_chunk_size +1) + min_chunk_size ;
                            network_utilize.write(Integer.toString(r1) + ","+Integer.toString(file_id));
                            String sr;
                            if(tokens[2].equalsIgnoreCase("Public")){
                                    sr = "E:/" + client_name + "/Uploaded file/Public/"+tokens[1];

                            }
                            else {
                                sr = "E:/" + client_name + "/Uploaded file/Private/"+tokens[1];

                            }
                            File file = new File(sr);
                            file.createNewFile();

                            FileOutputStream fr = new FileOutputStream(sr);
                            long f_size = Long.parseLong(tokens[3]);
                            byte[] b = new byte[r1];
                            long n1 = 0;
                            int n2 = 0;
                            byte[] finalarr = new byte[(int)f_size];
                            int i=0;
                            while  ((n2 = network_utilize.read_for_file_2(b)) != -1){
                                n1+=n2;

                                for(int j=0;j<n2;j++){
                                    finalarr[i] = b[j];
                                    i++;
                                }

                               network_utilize.write("yes");
                                if(n1>=f_size){

                                    break;
                                }



                            }


                            if(n1 == f_size){
                                network_utilize.write("Successfully transmit file");
                                fr.write(finalarr,0,(int)f_size);


                            }
                            else {
                                network_utilize.write("Failure in transmit file");
                            }

                            fr.close();
                            tcib.setN(tcib.getN() - tttt );

                        }
                        else {
                            network_utilize.write("buffer_overflow_occur");

                        }


                    }
                    else if(tokens[0].equalsIgnoreCase("Look_Up_Clients")){

                            List<String> ln = new ArrayList<>();

                            for(int i=0;i<list_of_client.size();i++){
                                String na = list_of_client.get(i);
                                if(connected_clients.get(na) != null){
                                    ln.add(na+" (Online)");
                                }
                                else {
                                    ln.add(na + " (Offline)");
                                }

                            }
                        Message mk = new Message(ln);
                            network_utilize.write(mk);


                    }
                    else if(tokens[0].equalsIgnoreCase("Look_Up_files_me")){
                        List<String> publist = new ArrayList<>();
                        List<String> private_list = new ArrayList<>();
                        File public_directory = new File("E:/"+client_name+"/Uploaded file/Public");
                        File private_directory = new File("E:/"+client_name+"/Uploaded file/Private");

                        File[] files = public_directory.listFiles();
                        if(files != null){
                            for(File file : files){
                                publist.add(file.getName());

                            }
                        }
                         files = private_directory.listFiles();
                        if(files != null){
                            for(File file : files){
                                private_list.add(file.getName());

                            }
                        }

                        Message mk = new Message(publist,private_list);
                        network_utilize.write(mk);

                        String sdo = (String) network_utilize.read();
                        String[] arr = sdo.split(",");
                        if(arr[0].equalsIgnoreCase("download_my_uploaded_file")){
                            String fname;
                            if(arr[2].equalsIgnoreCase("Public")){
                                fname = "E:/"+client_name+"/Uploaded file/Public/" + arr[1];
                            }
                            else{
                                fname = "E:/"+client_name+"/Uploaded file/Private/" + arr[1];
                            }

                            File myfile = new File(fname);
                            long filesize = myfile.length();
                            network_utilize.write(Long.toString(filesize));
                            FileInputStream fr = new FileInputStream(fname);
                            byte[] bk = new byte[max_chunk_size];
                            long n3 =0, n7 =0;
                            while (true){
                                n3 = fr.read(bk,0,max_chunk_size);
                                if(n3 == -1) break;
                                int n4 = (int) n3;

                                n7+=n3;
                                byte[] b2 = new byte[n4];
                                for(int i=0;i<n4;i++){
                                    b2[i] = bk[i];
                                }
                                network_utilize.write_for_file(b2,0,n4);
                                network_utilize.flush_for_write();


                            }



                            if(n7 == filesize){
                                network_utilize.write("Complete");
                            }
                            else {
                                network_utilize.write("Incomplete");
                            }

                            fr.close();

                        }

                    }
                    else if(tokens[0].equalsIgnoreCase("Look_Up_files_Other")){
                        HashMap<String, List<String>> hm = new HashMap<>();
                       for(int i=0;i<list_of_client.size();i++){
                           if(!list_of_client.get(i).equalsIgnoreCase(client_name)){
                               List<String> publist = new ArrayList<>();

                               File public_directory = new File("E:/"+list_of_client.get(i)+"/Uploaded file/Public");


                               File[] files = public_directory.listFiles();
                               if(files != null){
                                   for(File file : files){
                                       publist.add(file.getName());

                                   }
                               }
                               if(publist.size()!= 0){
                                   hm.put(list_of_client.get(i),publist);
                               }

                           }
                       }


                        Message mk = new Message(hm);
                        network_utilize.write(mk);
                        String sdo = (String) network_utilize.read();

                        String[] arr = sdo.split(",");
                        if(arr[0].equalsIgnoreCase("download")){
                            String fname = "E:/"+arr[1]+"/Uploaded file/Public/"+arr[2];

                            File myfile = new File(fname);
                            long filesize = myfile.length();
                            network_utilize.write(Long.toString(filesize));

                            FileInputStream fr = new FileInputStream(fname);
                            byte[] bk = new byte[max_chunk_size];
                            long n3 =0, n7 =0;
                            while (true){
                                n3 = fr.read(bk,0,max_chunk_size);
                                if(n3 == -1) break;
                                int n4 = (int) n3;

                                n7+=n3;
                                byte[] b2 = new byte[n4];
                                for(int i=0;i<n4;i++){
                                    b2[i] = bk[i];
                                }
                                network_utilize.write_for_file(b2,0,n4);
                                network_utilize.flush_for_write();


                            }



                            if(n7 == filesize){
                                network_utilize.write("Complete");
                           }
                           else {
                                network_utilize.write("Incomplete");
                           }
                            fr.close();




                        }




                    }
                    else if(tokens[0].equalsIgnoreCase("file_request")){

                            int request_id = request_id_map.size() + 1;
                            request_id_map.put(request_id,client_name);
                            String dpath = "E:/"+request_id;
                            File dir = new File(dpath);
                            dir.mkdir();

                            String mes = "File_request/"+request_id+"/"+tokens[1];


                        for (Map.Entry<String, List<String>> entry : inbox.entrySet()) {
                            String key = entry.getKey();
                            if(!key.equalsIgnoreCase(client_name)){
                                List<String> value = entry.getValue();
                                value.add(mes);
                            }
                        }


                    }
                    else if(tokens[0].equalsIgnoreCase("View_unread_message")){
                                List<String> msg = inbox.get(client_name);

                                Message mk = new Message(msg);
                                network_utilize.write(mk);
                                inbox.remove(client_name);
                                inbox.put(client_name,new ArrayList<>());
                                mk = (Message) network_utilize.read();
                                String s55 = mk.getText();

                                String[] tokens1 = s55.split("/");
                                if(tokens1[0].equalsIgnoreCase("Uploaded_file_according_to_request")){

                                    if(max_buffer_size>= tcib.getN() +Integer.parseInt(tokens1[3])){
                                        tcib.setN(tcib.getN() + Integer.parseInt(tokens1[3]) );
                                        network_utilize.write("buffer_overflow_not_occur");
                                        int tttt= Integer.parseInt(tokens1[3]);

                                        file_id++;
                                        int r1 = random.nextInt(max_chunk_size - min_chunk_size +1) + min_chunk_size ;
                                        network_utilize.write(Integer.toString(r1) + ","+Integer.toString(file_id));
                                        String sr = "E:/"+tokens1[1]+"/"+tokens1[2];
                                        // System.out.println(sr);
                                        File file = new File(sr);
                                        file.createNewFile();

                                        FileOutputStream fr = new FileOutputStream(sr);
                                        long f_size = Long.parseLong(tokens1[3]);
                                        byte[] b = new byte[r1];
                                        long n1 = 0;
                                        int n2 = 0;
                                        byte[] finalarr = new byte[(int)f_size];
                                        int i=0;
                                        while ((n2 = network_utilize.read_for_file_2(b)) != -1){
                                            n1+=n2;
                                            //System.out.println(n2);
                                            for(int j=0;j<n2;j++){
                                                finalarr[i] = b[j];
                                                i++;
                                            }
                                            network_utilize.write("yes");

                                            if(n1 >= f_size){
                                                break;
                                            }

                                        }

                                        if(n1 == f_size){
                                            network_utilize.write("Successfully transmit file");
                                            fr.write(finalarr,0,(int)f_size);
                                            String s66 = request_id_map.get(Integer.parseInt(tokens1[1]));
                                            List<String> arlist = inbox.get(s66);
                                            s66 = "Uploaded_file_according_to_request" + "/"+tokens1[1];
                                            arlist.add(s66);


                                        }
                                        else {
                                            network_utilize.write("Failure in transmit file");
                                        }

                                        fr.close();

                                        tcib.setN(tcib.getN() - tttt);

                                    }
                                    else {
                                        network_utilize.write("buffer_overflow_occur");

                                    }




                                }



                    }





                }

            }

        } catch (Exception e){
            //System.out.println(e);
        } finally {
            try {
                System.out.println(client_name+"  is disconnected");
                connected_clients.remove(client_name);
                network_utilize.closeConnection();
            } catch (IOException e){
                e.printStackTrace();
            }
        }


    }



}
