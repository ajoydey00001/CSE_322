
import java.io.BufferedReader;
import java.io.*;
import java.net.*;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Thread_Client implements Runnable{

    private Thread thread;
    private Network_Utilize network_utilize;
    String name;
    int max_chunk_size;

    Thread_Client(Network_Utilize nu, String nam,int ma){
        max_chunk_size = ma;

        network_utilize = nu;
        name = nam;
        thread = new Thread(this);
        thread.start();
    }

    public void run(){
        try {
            BufferedReader br =new BufferedReader(new InputStreamReader(System.in));

            while (true){
                System.out.println("Here is the list of your capability");

                System.out.println("1. Look up the list of clients who were connected to the server at least once");
                System.out.println("2. Look up the list of my uploaded files");
                System.out.println("3. Look up the list of other uploaded files whose access is public");
                System.out.println("4. Make a file request");
                System.out.println("5. View your unread message");
                System.out.println("6. Upload a file");
                System.out.println("7. Close connection with Server");
                System.out.println("Please select any one by input number");
                int n1 = Integer.parseInt(br.readLine());

                if(n1==6){
                    System.out.println("Please enter a file name");
                    String fname = br.readLine();
                    System.out.println("Please enter the path of the file name");
                    String fpath = br.readLine();
                    System.out.println("What is the access mode of the file? (public/private)");
                    String mode= br.readLine();
                    File MyFile = new File(fpath);
                    long FileSize = MyFile.length();

                    String se = "UPLOAD,"+fname+","+mode+","+ Long.toString(FileSize);

                    Message msg = new Message(se);
                    network_utilize.write(msg);

                    String buff = (String) network_utilize.read();

                    if(buff.equalsIgnoreCase("buffer_overflow_not_occur")){
                        String chstring = (String) network_utilize.read();
                        String[] tokens = chstring.split(",");
                        int chunk = Integer.parseInt(tokens[0]);
                        int file_id = Integer.parseInt(tokens[1]);
                        FileInputStream fr = new FileInputStream(fpath);
                        byte[] b = new byte[chunk];

                        long n3=0 , n7=0;


                            while (true){
                                n3 = fr.read(b,0,chunk);
                                if(n3 == -1){

                                    break;
                                }
                                int n4 = (int) n3;

                                n7+=n3;
                                byte[] b2 = new byte[n4];
                                for(int i1=0;i1<n4;i1++){
                                    b2[i1] = b[i1];
                                }
                                network_utilize.write_for_file(b2,0,n4);
                                network_utilize.flush_for_write();

                            try {
                                String ack = (String) network_utilize.read();

                            } catch (Exception e){
                                break;
                            }




                            }


                        fr.close();
                        String s88 ;

                        while (true){
                            s88 = (String) network_utilize.read();
                            if(s88.equalsIgnoreCase("yes")){

                            }
                            else {
                                break;
                            }
                        }

                        System.out.println(s88);

                    }
                    else {
                        System.out.println();

                        System.out.println("Server buffer's overflow occur. This file cannot be uploaded");
                        System.out.println();
                    }


                }
                else if(n1 == 7){
                    break;
                }
                else if(n1 ==1){
                    String u1 = "Look_Up_Clients,All";
                    Message m3 = new Message(u1);
                    network_utilize.write(m3);

                    Message mk = (Message) network_utilize.read();
                    List<String> lk = mk.getList();
                    System.out.println("List of Clients : ");
                    for(int i=0;i<lk.size();i++){
                        System.out.println(lk.get(i));
                    }
                    System.out.println();

                }
                else if(n1 == 2){
                    String u1 = "Look_Up_files_me,All";
                    Message m3 = new Message(u1);
                    network_utilize.write(m3);
                    Message mk = (Message) network_utilize.read();
                    List<String> lr = mk.get_public_list();
                    List<String> lk = mk.get_private_list();
                    if(lr.size()==0){
                        System.out.println("There is no uploaded file which's access mode is public");
                        System.out.println();
                    }
                    else{
                        System.out.println("List of uploaded public file list : ");
                        for(int i=0;i<lr.size();i++){
                            System.out.println(lr.get(i));
                        }
                        System.out.println();

                    }
                    if(lk.size()==0){
                        System.out.println("There is no uploaded file which's access mode is private");
                        System.out.println();
                    }
                    else{
                        System.out.println("List of uploaded private file list : ");
                        for(int i=0;i<lk.size();i++){
                            System.out.println(lk.get(i));
                        }
                        System.out.println();

                    }
                    System.out.println();
                    System.out.println("Do you want to choose any file to download");
                    System.out.println("Enter 1 for yes");
                    System.out.println("Enter 2 for no");
                    int option = Integer.parseInt(br.readLine());
                    if(option == 1){
                        System.out.println("Please enter the name of the file");
                        String fname = br.readLine();
                        System.out.println("Access mode of this file");
                        String acc = br.readLine();
                        String st1 = "download_my_uploaded_file,"+fname +","+acc;

                        network_utilize.write(st1);
                        String dPath = "F:/" + name + "/Downloaded file/"+fname;
                        FileOutputStream fr = new FileOutputStream(dPath);
                        String y33 = (String) network_utilize.read();
                        long f_size = Long.parseLong(y33);
                        byte[] b3 = new byte[max_chunk_size];
                        long n12 = 0;
                        int n22 = 0;
                        byte[] finalarr = new byte[(int) f_size];
                        int i=0;
                        while ((n12<f_size) && (n22 = network_utilize.read_for_file_2(b3)) != -1){

                            n12 += n22;
                            for (int j=0;j<n22;j++){
                                finalarr[i] = b3[j];
                                i++;
                            }

                        }
                        String fe = (String) network_utilize.read();
                        if(fe.equalsIgnoreCase("Complete")){
                            System.out.println("Successfully download this file");
                            System.out.println();
                            fr.write(finalarr,0,(int) f_size);
                        }
                        else{
                            System.out.println("Fail to download this file");
                            System.out.println();
                        }
                        fr.close();


                    }
                    else {
                        network_utilize.write("NO,No,NO,NO");
                    }



                }
                else if(n1 == 3){
                    String u1 = "Look_Up_files_Other,public";
                    Message m3 = new Message(u1);
                    network_utilize.write(m3);
                    Message mk = (Message) network_utilize.read();
                    HashMap<String,List<String>> hm = mk.get_other_filelist();
                    if(hm.size()==0){
                        System.out.println("There is no such file in other directories");
                        System.out.println();
                        network_utilize.write("No,NO,no");
                    }
                    else{
                        System.out.println("Here is the list of uploaded public files of other :");
                        System.out.println();
                        for (Map.Entry<String,List<String>> entry : hm.entrySet()) {
                            String n45 = entry.getKey();
                            System.out.println("The uploaded files of "+n45+" is :");
                            List<String> value = entry.getValue();
                            for(String s : value){
                                System.out.println(s);
                            }

                        }

                        System.out.println();
                        System.out.println("Do you want to choose any file to download");
                        System.out.println("Enter 1 for yes");
                        System.out.println("Enter 2 for no");
                        int option = Integer.parseInt(br.readLine());
                        if(option == 1){
                            System.out.println("Please enter the name of the client whose file you want to download");
                            String st1 = br.readLine();
                            System.out.println("Please enter the name of the file");
                            String fname = br.readLine();
                            st1 = "download,"+st1 +","+fname;

                            network_utilize.write(st1);
                            String dPath = "F:/" + name;

                            dPath = dPath + "/Downloaded file";

                            dPath = dPath+"/"+fname;

//                            File file = new File(dPath);
//                          file.createNewFile();

                            FileOutputStream fr = new FileOutputStream(dPath);
                            String y33 = (String) network_utilize.read();
                            long f_size = Long.parseLong(y33);


                            byte[] b3 = new byte[max_chunk_size];
                            long n12 = 0;
                            int n22 = 0;
                            byte[] finalarr = new byte[(int)f_size];

                            int i=0;
                            while ((n12<f_size) && (n22 = network_utilize.read_for_file_2(b3)) != -1){

                                n12+=n22;
                                for(int j=0;j<n22;j++){
                                    finalarr[i] = b3[j];
                                    i++;
                                }

                            }

                            String fe = (String) network_utilize.read();
                            if(fe.equalsIgnoreCase("Complete")){
                                System.out.println("Successfully download this file");
                                System.out.println();
                                fr.write(finalarr,0,(int) f_size);
                            }
                            else{
                                System.out.println("Fail to download this file");
                                System.out.println();
                           }
                            fr.close();



                        }else {
                            network_utilize.write("No,NO,No");
                        }



                    }


                }
                else if(n1 == 4){
                    System.out.println("Give a short description");
                    String description = br.readLine();
                    description = "file_request," + description;
                    Message mk = new Message(description);
                    network_utilize.write(mk);


                }
                else if(n1 == 5){
                    String msf = "View_unread_message,ALL";
                    Message mk = new Message(msf);
                    network_utilize.write(mk);
                    mk = (Message) network_utilize.read();
                    List<String> lk = mk.getList();
                    int k=0,l=1;
                    System.out.println();
                    if(lk.size()==0){
                        System.out.println("There is no unread message for you");
                        System.out.println();
                        System.out.println();
                    }
                    else {
                        System.out.println("List of Unread message");

                        for(String s : lk){

                            System.out.print(l+".  ");
                            System.out.print(s);
                            System.out.println();
                            l++;
                            String[] tokens = s.split("/");
                            if(tokens[0].equalsIgnoreCase("File_request")){
                                k++;
                            }

                        }
                        System.out.println();
                    }

                    if(k!=0){

                            System.out.println("What do you want to upload a file according to request ? (Press 1 for yes & Press 0 for NO )");
                            int ds = Integer.parseInt(br.readLine());
                            if(ds == 0){
                                Message mt = new Message("no/no/no");
                                network_utilize.write(mt);

                            }
                            else{

                                System.out.println("Please enter file request id");
                                int id = Integer.parseInt(br.readLine());
                                System.out.println("Please enter your file name");
                                String na = br.readLine();
                                na = "Uploaded_file_according_to_request/"+id+"/"+na;

                                System.out.println("Please enter the path of your file");
                                String path =br.readLine();
                                File myfile = new File(path);
                                long f_size = myfile.length();
                                na = na+"/"+f_size;

                                Message mt = new Message(na);
                                network_utilize.write(mt);

                                String buff = (String) network_utilize.read();

                                if(buff.equalsIgnoreCase("buffer_overflow_not_occur")){
                                    String chstring = (String) network_utilize.read();

                                    String[] tokens = chstring.split(",");

                                    int chunk = Integer.parseInt(tokens[0]);

                                    int file_id = Integer.parseInt(tokens[1]);
                                    FileInputStream fr = new FileInputStream(path);
                                    byte[] b = new byte[chunk];

                                    long n3=0 , n7=0;

                                    while (true){
                                        n3 = fr.read(b,0,chunk);
                                        if(n3 == -1) {
                                            break;
                                        }

                                        int n4 = (int) n3;

                                        n7+=n3;
                                        byte[] b2 = new byte[n4];
                                        for(int i1=0;i1<n4;i1++){
                                            b2[i1] = b[i1];
                                        }
                                        network_utilize.write_for_file(b2,0,n4);
                                        network_utilize.flush_for_write();
                                        try {
                                            String ack = (String) network_utilize.read();

                                        } catch (Exception e){
                                            break;
                                        }



                                    }

                                    fr.close();
                                    String s88 ;

                                    while (true){
                                        s88 = (String) network_utilize.read();
                                        if(s88.equalsIgnoreCase("yes")){

                                        }
                                        else {
                                            break;
                                        }
                                    }

                                    System.out.println(s88);

                                }
                                else {
                                    System.out.println();

                                    System.out.println("Server buffer's overflow occur. This file cannot be uploaded");
                                    System.out.println();
                                }


                            }



                    }
                    else{
                        Message mt = new Message("no/no/no");
                        network_utilize.write(mt);
                    }



                }




            }



        } catch (Exception e){
           // System.out.println(e);
        } finally {
            try {
                network_utilize.closeConnection();
            } catch (IOException e){
                e.printStackTrace();
            }
        }


    }


}
