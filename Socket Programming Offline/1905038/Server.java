import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;


public class Server {

    private ServerSocket s_socket;
    private int max_buffer_size;
    private int max_chunk_size;
    private int min_chunk_size;
    private Total_Chunk_In_Buffer tcib;
    public HashMap<String, Network_Utilize> connected_clients;
    public List<String> list_of_client;

    public HashMap<String,List<String>> inbox;
    public HashMap<Integer,String> request_id_map;

    Server(int n1,int n2,int n3){

        max_buffer_size = n1;
        min_chunk_size = n2;
        max_chunk_size = n3;


        connected_clients = new HashMap<>();
        list_of_client = new ArrayList<>();
        inbox = new HashMap<>();
        request_id_map = new HashMap<>();
        tcib = new Total_Chunk_In_Buffer(0);

        try {
            s_socket = new ServerSocket(33333);

            while (true){
                Socket c_socket = s_socket.accept();
                service(c_socket);
            }
        } catch (Exception e){
            //System.out.println(e);
        }



    }

    public void service(Socket c_socket)throws IOException, ClassNotFoundException{
        Network_Utilize network_utilize = new Network_Utilize(c_socket);
        String c_name = (String) network_utilize.read();
        Network_Utilize nu = connected_clients.get(c_name);

        if(nu == null){

            if(list_of_client.contains(c_name)){
                network_utilize.write("Valid_2, You are not newly connected");
                connected_clients.put(c_name,network_utilize);
                System.out.println(c_name+", you are not newly connected");
                network_utilize.write(Integer.toString(max_chunk_size));

                new Read_Thread_Server(connected_clients,list_of_client,inbox,request_id_map,network_utilize, max_buffer_size,min_chunk_size,max_chunk_size,c_name,tcib);



            }
            else{
                network_utilize.write("Valid_1, You are newly connected");
                connected_clients.put(c_name,network_utilize);
                String k2 = new String(c_name);
                list_of_client.add(k2);
                k2 = new String(k2);
                inbox.put(k2,new ArrayList<>());
                System.out.println(c_name + ", you are connected");
                network_utilize.write(Integer.toString(max_chunk_size));

                String directoryPath = "E:/" + c_name;
                File directory = new File(directoryPath);
                directory.mkdir();
                directoryPath = directoryPath + "/Uploaded file";
                directory = new File(directoryPath);
                directory.mkdir();
                directoryPath = directoryPath +"/Public";
                directory = new File(directoryPath);
                directory.mkdir();
                directoryPath = "E:/" + c_name + "/Uploaded file/Private";
                directory = new File(directoryPath);
                directory.mkdir();


                new Read_Thread_Server(connected_clients,list_of_client,inbox,request_id_map,network_utilize, max_buffer_size,min_chunk_size,max_chunk_size,c_name,tcib);

            }


        }
        else {
            String s = "There exists a client whose name is same as your. This connection is terminated.";
            s = "Invalid, "+s;
            network_utilize.write(s);
            network_utilize.closeConnection();

        }



    }

    public static void main(String[] args) throws IOException{
        System.out.println("Please enter max buffer size");
        BufferedReader br =new BufferedReader(new InputStreamReader(System.in));
        int n1 = Integer.parseInt(br.readLine());
        System.out.println("Please enter min chunk size");
        int n2 = Integer.parseInt(br.readLine());
        System.out.println("Please enter max chunk size");
        int n3= Integer.parseInt(br.readLine());

        Server server = new Server(n1,n2,n3);

    }


}
