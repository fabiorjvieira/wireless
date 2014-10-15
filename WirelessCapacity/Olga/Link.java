import java.util.*;
import java.io.*;

public class Link {
	
	int id;
	Node receiver;
	Node sender;
	double length;
	double power = 1;
	
	double interferenceNumber;
	double affectedness;

	public Link(int id)
	{
		receiver = new Node(0,0,0);
		sender = new Node(0,0,0);
		length = 0;
	}

	public Link(int pid, Node pReceiver, Node pSender)
	{
		id = pid;
		receiver = pReceiver;
		sender = pSender;
		length = Utils.getDist2D(sender, receiver);
	}
    
    public static ArrayList<Link> load(ArrayList<Node> nodes)
    {
        ArrayList<Link> links= new ArrayList<Link>();
        Link link = null;
        int index;
 
        for (int nodeIndex = 0; nodeIndex <= nodes.size() -1; nodeIndex+=2)
        {
	    index = nodeIndex/2;
            link = new Link(index, nodes.get(nodeIndex), nodes.get(nodeIndex +1));
            links.add(link);
        }
        return links;
    }
}
