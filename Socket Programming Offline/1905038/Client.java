
import java.io.File;
import java.util.Scanner;
public class Client {




    Client(String serverAddress, int port) {
        try {
            System.out.println("Please , enter your name for login");
            Scanner sc = new Scanner(System.in);
            String name = sc.nextLine();
            Network_Utilize nu = new Network_Utilize(serverAddress, port);
            nu.write(name);
            String s =(String) nu.read();
            String[] tokens = s.split(",");
            System.out.println(tokens[0]);
            if(tokens[0].equalsIgnoreCase("Valid_1"))
            {
                nu.getSocket().setSoTimeout(30000);
                String directoryPath = "F:/" + name;
                File directory = new File(directoryPath);
                directory.mkdir();
                directoryPath = directoryPath + "/Downloaded file";
                directory = new File(directoryPath);
                directory.mkdir();
                String k5 = (String) nu.read();

                new Thread_Client(nu,name,Integer.parseInt(k5));
            }
            else if(tokens[0].equalsIgnoreCase("Valid_2")){
                nu.getSocket().setSoTimeout(30000);
                String k5 = (String) nu.read();

                new Thread_Client(nu,name,Integer.parseInt(k5));
            }
            else if(tokens[0].equalsIgnoreCase("Invalid")){
                System.out.println(tokens[1]);
                nu.closeConnection();
            }


        } catch (Exception e){
            //System.out.println(e);
        }
    }


    public static void main(String[] args){
        String serverAddress = "127.0.0.1";
        int port = 33333;
        Client client = new Client(serverAddress,port);

    }

}
