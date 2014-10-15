import java.util.*;
import java.io.*;

public class Utils {
	
	static public double getDist2D(Node p1, Node p2)
	{
		double d = (double) (Math.pow(p2.x-p1.x, 2) + Math.pow(p2.y-p1.y, 2));
		return (double) Math.pow(d, .5);
	}
	
	
	//counts how many links l1 disturbs
	static public int getIterferenceNumber(Link l1, ArrayList<Link> pLinks,
                                           double alpha, double beta, double noise)
	{
		int iNum = 0;
		
		for (int i = 0; i < pLinks.size(); i++)
		{
			Link l = pLinks.get(i);
			if (l.id == l1.id) continue;
			double d = Utils.getDist2D(l.receiver, l.sender);
			double p = (double) (l.power/Math.pow(d, alpha));
			double d1 = Utils.getDist2D(l1.sender, l.receiver);
			double p1 = (double) (l1.power/Math.pow(d1, alpha));
			if (p/p1 < beta)
				iNum++;
		}
		return iNum;
	}
    
	
	static public double getAffectedness(Link l1, ArrayList<Link> pLinks,
                                         double alpha, double beta, double noise)
	{
		double interference = 0;
		double d = Utils.getDist2D(l1.receiver, l1.sender);
		double myPower = (double) (l1.power/Math.pow(d, alpha));
		
		for (int i = 0; i < pLinks.size(); i++)
		{
			Link l = pLinks.get(i);
			if (l.id == l1.id) continue;
			d = Utils.getDist2D(l1.receiver, l.sender);
			double p = (double) (l.power/Math.pow(d, alpha));
			interference += p;
		}
		return (double) beta * (double) (interference + noise) / myPower;
	}
    
}
