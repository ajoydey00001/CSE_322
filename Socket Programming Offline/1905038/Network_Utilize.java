import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;


public class Network_Utilize {

    private Socket s;
    private ObjectOutputStream os;
    private ObjectInputStream is;
    Network_Utilize(String s1, int port)throws IOException{
        s = new Socket(s1,port);
        os = new ObjectOutputStream(s.getOutputStream());
        is = new ObjectInputStream(s.getInputStream());
    }

    Network_Utilize(Socket s1)throws IOException{
        s=s1;
        os=new ObjectOutputStream(s.getOutputStream());
        is=new ObjectInputStream(s.getInputStream());
    }

    public Object read() throws IOException, ClassNotFoundException{
        return is.readUnshared();
    }
    public void write(Object k)throws IOException{
        os.writeUnshared(k);


    }
    public void write_for_file(byte[] k, int n1, int n2) throws IOException{
        os.write(k,n1,n2);
    }
    public void write_for_file_2(byte[] k) throws IOException{
        os.write(k);
    }

    public int read_for_file(byte[] k,int n1, int n2) throws IOException{

        return is.read(k,n1,n2);
    }
    public int read_for_file_2(byte[] b) throws IOException{
        return is.read(b);
    }
    public Socket getSocket() {
        return this.s;
    }
    public void flush_for_write() throws IOException{
        os.flush();
    }
    public void closeConnection() throws IOException{
        is.close();
        os.close();
    }





}
