import java.io.Serializable;
import java.util.HashMap;
import java.util.List;

public class Message implements Serializable {

    private String text;
    private List<String> list;
    private List<String> publist;
    private List<String> prilist;
    private HashMap<String,List<String>> hm;

    Message(){

    }
    Message(String s){
        text = s;
    }
    Message(List<String> l){
        list = l;
    }
    Message(List<String> l1,List<String> l2){
        prilist = l2;
        publist = l1;
    }
    Message(HashMap<String,List<String>> l){
        hm = l;
    }
    public HashMap<String,List<String>> get_other_filelist(){
        return hm;
    }
    public List<String> get_public_list(){
        return publist;
    }
    public List<String> get_private_list(){
        return prilist;
    }
    public List<String> getList(){
        return list;
    }

    public String getText(){
        return text;
    }
    public void setText(String s){
        text = s;
    }





}
