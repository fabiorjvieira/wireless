import java.util.*;
import java.io.*;

public class Node {

	public double x;
	public double y;
	public double z;
	
	public Node(double px, double py, double pz)
	{
		x = px; y = py; z = pz;
	}
    
    public static ArrayList<Node> load(String fileName)
    {
        ArrayList<Node> nodeList = new ArrayList<Node>();
        float px, py;
        String[] parts = null;
        String line = null;
        BufferedReader reader = null;;
        try{reader = new BufferedReader(new FileReader(fileName));}catch (java.io.FileNotFoundException e){}
        
        try{line = reader.readLine();}catch (java.io.IOException e){}
        while (line != null) {
            parts = line.split("\\s");
            px = Float.parseFloat(parts[0]);
            py = Float.parseFloat(parts[1]);
            nodeList.add(new Node(px, py, 0));
            try{line = reader.readLine();}catch (java.io.IOException e){}
        }
        return nodeList;
    }
}
