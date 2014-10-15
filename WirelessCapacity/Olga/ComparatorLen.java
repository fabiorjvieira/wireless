import java.util.Comparator;

public class ComparatorLen implements Comparator<Link>{

	public int compare(Link l1, Link l2) {
		
//		Link l1 = (Link) arg0;
//		Link l2 = (Link) arg1;
		if (l1.id == l2.id) return 0;
		if (l1.length > l2.length) return -1;
		if (l1.length < l2.length) return 1;
		if (l1.length == l2.length) return 0;
		
		return 0;
	}	
}
