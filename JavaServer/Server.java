import java.net.*;
import java.io.*;
import java.util.*;
 
public class Server {
    static int uniqueId = 0;
    static float x = 0;
    //static String serverMsg = "";
    static int strId = 0;
    static ArrayList<String> msgList = new ArrayList<String>();
    //static String[] msgList = {"","","",""};
    static boolean newMsg = false;
    int port = 1234;
    
    ArrayList<sysThread> al = new ArrayList<sysThread>();
    private boolean keepRunning;
    
    public void start(){
        keepRunning = true;
         
        try {
            ServerSocket serverSocket = new ServerSocket(port);
            while (keepRunning){
            	Socket socket = serverSocket.accept();
            	if (!keepRunning)
            		break;
            	sysThread st = new sysThread(socket);
            	al.add(st);
            	st.start();
            }
            try{
            	serverSocket.close();
            	for(int i = 0; i < al.size(); i++){
            		sysThread st = al.get(i);
            		try{
            			st.close();
            		}catch(Exception e){}
            	}
            }catch(Exception e){
            	System.out.println("Error when closing: " + e);
            }
            
        } catch (IOException e) {
            System.out.println("Exception caught when trying to listen on port "
                + port + " or listening for a connection");
            System.out.println(e.getMessage());
        }
        System.out.println("Server stopped");
    }
    
    public static void main(String[] args) throws IOException {
    	Server s = new Server();
    	s.start();
    }
    synchronized void remove(int id) {
    	for (int i = 0; i < al.size(); i++){
    		sysThread st = al.get(i);
    		if (st.id == id){
    			al.remove(i);
    			break;
    			//return;
    		}
    	}
    	if (al.size() == 0){
    		keepRunning = false;
    		System.out.println("Stoping server");
    		try{
    			new Socket("10.19.190.253", port);
    		}catch (Exception e){
    			System.out.println("Error when closing server");
    		}
    	}
	}
    
    public static synchronized void setMsg(String s, int i){
    	//serverMsg = s;
    	msgList.set(i, s);
    }
    public static synchronized String getMsg(){
    	String temp = "";
    	for (int i = 0; i < msgList.size(); i++){
    		if (msgList.get(i).length() > 1){
				temp += msgList.get(i);
				temp += '@';
    		}
    	}
    	return temp;
    	//return serverMsg;
    }
    
    public static synchronized void setMsgFlag(boolean f){
    	newMsg = f;
    }
    public static synchronized boolean getMsgFlag(){
    	return newMsg;
    }
    
    public class sysThread extends Thread{
    	Socket socket;
    	BufferedReader in;
    	//ObjectInputStream in;
    	PrintWriter out;
    	//ObjectOutputStream out;
    	int id;
    	int inputId;
    	int threadStatus;    	
    	
    	public sysThread(Socket s){
    		socket = s;
    		id = uniqueId;
    		uniqueId++;
    		inputId = strId;
    		msgList.add("");
    		strId++;
    		threadStatus = 0;
    		try{
    			//out = new ObjectOutputStream(socket.getOutputStream());
    			out= new PrintWriter(socket.getOutputStream(), true);
    			//in = new ObjectInputStream(socket.getInputStream());
    			in = new BufferedReader(
    					new InputStreamReader(socket.getInputStream()));
    		}catch (Exception e){
    			System.out.println("Error creating thread: " + e);
    		}
    	}
    	
    	public void run(){
    		//System.out.println(inputId);
    		String s = "";
    		String temp;
    		char[] buf = new char[32];
    		//StringBuffer sb = new StringBuffer();
    		while (true){
    			if (threadStatus == 0){
	    			try{
	    				if (in.ready()){
		    				s = in.readLine();
		    				//in.read(buf, 0, 32);
	    				}
	    			}catch (Exception e){
	    				System.out.println(id + " Error reading " + e);
	    				break;
	    			}
	    			if (s != null){
		    			if (s.equals("closeServer")){
		    				break;
		    			}else if (threadStatus == 0 && s.equals("Input")){
		    				threadStatus = 1;
		    			}else if (threadStatus == 0 && s.equals("Output")){
		    				threadStatus = 2;
		    				continue;
		    			}
						//if (!s.equals(serverMsg)){
		    			if (!s.equals(msgList.get(inputId))){
							//serverMsg = s;
							Server.setMsg(s, inputId);
							//newMsg = true;
							Server.setMsgFlag(true);
							System.out.println("User: " + id + " " + s);
							if (threadStatus == 1){
							//out.println("User: " + id + " " + newMsg + " " + s);
								out.println(getMsg());
							}
						}
	    			}else{
	    				System.out.println(buf);
	    			}
    			}else if (threadStatus == 2){
    				if (Server.getMsgFlag()){
    					System.out.println(Server.getMsg());
    					out.println(Server.getMsg());
    					//newMsg = false;
    					Server.setMsgFlag(false);
    				}
    			}
    		}
    		System.out.println("Exiting user: " + id);
    		remove(id);
    		close();
    	}
    	
    	public void close(){
    		try{
    			if (out != null)
    				out.close();
    		}catch(Exception e){}
    		
    		try{
    			if (in != null)
    				in.close();
    		}catch(Exception e){}
    		
    		try{
    			if (socket != null)
    				socket.close();
    		}catch(Exception e){}
    	}
    }
}
