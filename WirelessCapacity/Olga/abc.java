import java.util.*;
import java.io.*;

public class abc {
    public static ArrayList<ArrayList<Link>> algoPhyGreedy(ArrayList<Link> input)
    {
        double alpha = 3, beta = 1.2, noise = 0;
        ArrayList<Link> links = new ArrayList<Link>();
        for (int i = 0; i < input.size(); i++)
        {
            Link l = input.get(i);
            double iNum = Utils.getIterferenceNumber(l, input, alpha, beta, noise);
            l.interferenceNumber = iNum;
            links.add(l);
        }
        Collections.sort(links, new ComparatorINum());
        
        ArrayList<ArrayList<Link>> schedule = new ArrayList<ArrayList<Link>>();
        
        int time = 0;
        schedule.add(new ArrayList<Link>());
        
        for (int i = 0; i < links.size(); i++)
        {
            Link l = links.get(i);
            //check affectedness of already scheduled links is l was inserted
            ArrayList<Link> test = new ArrayList<Link>(schedule.get(time));
            test.add(l);
            boolean ok = true;
            double a;
            for (int j = 0; j < test.size(); j++)
            {
                a = Utils.getAffectedness(test.get(j), test, alpha, beta, noise);
                if (a > 1)
                {
                    ok = false;
                    break;
                }
            }
            if (ok)
            {
                schedule.get(time).add(l);
            }
            else
            {
                time++;
                schedule.add(new ArrayList<Link>());
                schedule.get(time).add(l);
            }
        }
        return schedule;
    }


    public static ArrayList<ArrayList<Link>> algoApproxLogN(ArrayList<Link> input)
    {
        double alpha = 3, beta = 1.2, noise = 0;
        double constantC = (double)
        Math.pow((double) 288 * beta * (double) (alpha-1)/(alpha-2), (double) 1/(alpha));
        
        if (constantC < 2) constantC = 2;
        System.out.println("Constant C = " + constantC);
        
        ArrayList<Link> links = new ArrayList<Link>(input);
        Collections.sort(links, new ComparatorLen());
        
        ArrayList<ArrayList<Link>> schedule = new ArrayList<ArrayList<Link>>();
        
        int time = 0;
        schedule.add(new ArrayList<Link>());
        
        int cntDel1 = 0;
        int cntDel2 = 0;
        
        while (!links.isEmpty())
        {
            ArrayList<Link> delLinks = new ArrayList<Link>();
            for (int i = 0; i < links.size(); i++)
            {
                Link l = links.get(i);
                if (delLinks.contains(l))
                    continue;
                
                schedule.get(time).add(l);
                
                for (int j = i+1; j < links.size(); j++)
                {
                    //delete step 1
                    Link l2 = links.get(j);
                    
                    if (delLinks.contains(l2))
                        continue;
                    
                    double d = Utils.getDist2D(l.receiver, l2.sender);
                    if (d <= constantC * l.length)
                    {
                        delLinks.add(l2);
                        cntDel1++;
                    }
                    else
                        //delete step 2
                    {
                        double a = Utils.getAffectedness(l2, schedule.get(time), alpha, beta, noise);
                        if (a >= (double)2/3)
                        {
                            delLinks.add(l2);
                            cntDel2++;
                        }
                    }
                }
                cntDel1 = cntDel1;
            }
            links.removeAll(schedule.get(time));
            if (!links.isEmpty())
            {
                schedule.add(new ArrayList<Link>());
                time++;
            }
        }
        System.out.println("LOGN DELETED PHASE ONE:"+cntDel1);
        System.out.println("LOGN DELETED PHASE TWO:"+cntDel2);
        return schedule;
    }

public static void main(String [ ] args)
{
    abc algos = new abc();
    ArrayList<Node> nodes = Node.load(args[0]);
    ArrayList<Link> links = Link.load(nodes);
    ArrayList<ArrayList<Link>> schedule = abc.algoApproxLogN(links);
    System.out.println("Result: " + schedule.size());
}

}
